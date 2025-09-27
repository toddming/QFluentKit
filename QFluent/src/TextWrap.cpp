// textwrap.cpp
#include "textwrap.h"
#include <QRegularExpression>
#include <QDebug>

// 静态成员定义
QCache<TextWrap::WidthKey, int> TextWrap::m_widthCache;
QCache<QString, int> TextWrap::m_charWidthCache;
QCache<TextWrap::TokenizeKey, QVector<QString>> TextWrap::m_tokenizeCache;
QCache<TextWrap::SplitKey, QVector<QString>> TextWrap::m_splitCache;

void TextWrap::setCacheSize(int size)
{
    m_widthCache.setMaxCost(size);
    m_charWidthCache.setMaxCost(size * 2); // 字符更多
    m_tokenizeCache.setMaxCost(size / 2);
    m_splitCache.setMaxCost(size / 2);
}

void TextWrap::clearCache()
{
    m_widthCache.clear();
    m_charWidthCache.clear();
    m_tokenizeCache.clear();
    m_splitCache.clear();
}

void TextWrap::initCacheIfNeeded()
{
    static bool initialized = false;
    if (!initialized) {
        setCacheSize(1000); // 默认缓存 1000 条
        initialized = true;
    }
}


uint qHash(const TextWrap::WidthKey &key, uint seed)
{
    return qHashMulti(seed, key.str);
}

uint qHash(const TextWrap::TokenizeKey &key, uint seed)
{
    return qHashMulti(seed, key.line);
}

uint qHash(const TextWrap::SplitKey &key, uint seed)
{
    return qHashMulti(seed, key.token, key.width);
}

// textwrap.cpp 中的 getCharWidth 函数（修复版）
int TextWrap::getCharWidth(QChar ch)
{
    initCacheIfNeeded();

    QString key = QString(ch);
    if (int* cached = m_charWidthCache.object(key)) {
        return *cached;
    }

    // 获取 Unicode 码点
    uint ucs4 = ch.unicode();

    // 基本 ASCII 空白和控制字符（宽度为 1）
    if (ucs4 < 0x20 || ucs4 == 0x7F) {
        auto *value = new int(1);
        m_charWidthCache.insert(key, value);
        return 1;
    }

    // 参考 Unicode TR#11: East Asian Width
    // https://www.unicode.org/reports/tr11/

    // 常见全角（Wide）字符范围
    if (
        (ucs4 >= 0x1100 && ucs4 <= 0x115F) || // Hangul Jamo
        (ucs4 >= 0x2329 && ucs4 <= 0x232A) || // ⟨ ⟩
        (ucs4 >= 0x2E80 && ucs4 <= 0x303E) ||
        (ucs4 >= 0x3040 && ucs4 <= 0x3247) ||
        (ucs4 >= 0x3250 && ucs4 <= 0x4DBF) ||
        (ucs4 >= 0x4E00 && ucs4 <= 0x9FFF) || // CJK Unified Ideographs
        (ucs4 >= 0xA960 && ucs4 <= 0xA97F) || // Hangul Jamo Extended-A
        (ucs4 >= 0xAC00 && ucs4 <= 0xD7AF) || // Hangul Syllables
        (ucs4 >= 0xF900 && ucs4 <= 0xFAFF) || // CJK Compatibility Ideographs
        (ucs4 >= 0xFE10 && ucs4 <= 0xFE19) || // Vertical forms
        (ucs4 >= 0xFE30 && ucs4 <= 0xFE6F) || // CJK Compatibility Forms
        (ucs4 >= 0xFF00 && ucs4 <= 0xFF60) || // Fullwidth ASCII variants
        (ucs4 >= 0xFFE0 && ucs4 <= 0xFFE6) || // Fullwidth symbol variants
        (ucs4 >= 0x20000 && ucs4 <= 0x2FFDF) || // Supplementary Ideographic Plane
        (ucs4 >= 0x30000 && ucs4 <= 0x3FFDF)
        ) {
        auto *value = new int(2);
        m_charWidthCache.insert(key, value);
        return 2;
    }

    // 半角片假名等（宽度为 1）
    if ((ucs4 >= 0xFF61 && ucs4 <= 0xFF9F)) { // Halfwidth Katakana
        auto *value = new int(1);
        m_charWidthCache.insert(key, value);
        return 1;
    }

    // 默认：窄字符（Latin, 数字, 符号等）
    auto *value = new int(1);
    m_charWidthCache.insert(key, value);
    return 1;
}

int TextWrap::getTextWidth(const QString &str)
{
    if (str.isEmpty()) return 0;

    initCacheIfNeeded();
    WidthKey key{str};
    if (int* cached = m_widthCache.object(key)) {
        return *cached;
    }

    int width = 0;
    for (QChar ch : str) {
        width += getCharWidth(ch);
    }

    auto *value = new int(width);
    m_widthCache.insert(key, value);
    return width;
}

TextWrap::CharType TextWrap::getCharType(QChar ch)
{
    if (ch.isSpace()) {
        return CharType::Space;
    }
    if (getCharWidth(ch) == 1) {
        return CharType::Latin;
    }
    return CharType::Asian;
}

QString TextWrap::processWhitespace(const QString &text)
{
    return text.simplified();
}

QVector<QString> TextWrap::splitLongToken(const QString &token, int width)
{
    initCacheIfNeeded();
    SplitKey key{token, width};
    if (auto* cached = m_splitCache.object(key)) {
        return *cached;
    }

    QVector<QString> chunks;
    int len = token.length();
    for (int i = 0; i < len;) {
        QString chunk;
        int chunkWidth = 0;
        while (i < len && chunkWidth + getCharWidth(token[i]) <= width) {
            chunk += token[i];
            chunkWidth += getCharWidth(token[i]);
            ++i;
        }
        if (!chunk.isEmpty()) {
            chunks.append(chunk);
        }
    }

    auto *value = new QVector<QString>(chunks);
    m_splitCache.insert(key, value);
    return chunks;
}

QVector<QString> TextWrap::tokenize(const QString &line)
{
    if (line.isEmpty()) return {};

    initCacheIfNeeded();
    TokenizeKey key{line};
    if (auto* cached = m_tokenizeCache.object(key)) {
        return *cached;
    }

    QVector<QString> tokens;
    QString buffer;
    CharType lastType = getCharType(line.isEmpty() ? QChar(' ') : line[0]);

    for (int i = 0; i < line.length(); ++i) {
        QChar ch = line[i];
        CharType currentType = getCharType(ch);

        if (!buffer.isEmpty() && currentType != lastType) {
            tokens.append(buffer);
            buffer.clear();
        }

        buffer += ch;
        lastType = currentType;
    }

    if (!buffer.isEmpty()) {
        tokens.append(buffer);
    }

    auto *value = new QVector<QString>(tokens);
    m_tokenizeCache.insert(key, value);
    return tokens;
}

std::pair<QString, bool> TextWrap::wrapLine(const QString &text, int width, bool once)
{
    QString lineBuffer;
    QVector<QString> wrappedLines;
    int currentWidth = 0;

    auto tokens = tokenize(text);

    for (const QString &token : tokens) {
        int tokenWidth = getTextWidth(token);

        if (token == " " && currentWidth == 0) {
            continue;
        }

        if (currentWidth + tokenWidth <= width) {
            lineBuffer += token;
            currentWidth += tokenWidth;
            if (currentWidth == width) {
                wrappedLines.append(lineBuffer.trimmed());
                lineBuffer.clear();
                currentWidth = 0;
            }
        } else {
            if (!lineBuffer.isEmpty()) {
                wrappedLines.append(lineBuffer.trimmed());
                lineBuffer.clear();
                currentWidth = 0;
            }

            auto chunks = splitLongToken(token, width);
            for (int i = 0; i < chunks.size() - 1; ++i) {
                wrappedLines.append(chunks[i].trimmed());
            }
            lineBuffer = chunks.last();
            currentWidth = getTextWidth(lineBuffer);
        }
    }

    if (!lineBuffer.isEmpty()) {
        wrappedLines.append(lineBuffer.trimmed());
    }

    if (wrappedLines.isEmpty()) {
        return {QString(), false};
    }

    if (once && wrappedLines.size() > 1) {
        QString first = wrappedLines[0];
        QStringList rest;
        for (int i = 1; i < wrappedLines.size(); ++i) {
            rest.append(wrappedLines[i]);
        }
        return {first + "\n" + rest.join(" "), true};
    }

    QStringList result;
    for (const QString &line : wrappedLines) {
        result.append(line);
    }
    return {result.join("\n"), !wrappedLines.isEmpty()};
}

std::pair<QString, bool> TextWrap::wrap(const QString &text, int width, bool once)
{
    if (width <= 0) {
        return {"", true};
    }

    initCacheIfNeeded();

    QStringList inputLines = text.split('\n', Qt::SkipEmptyParts);
    QStringList outputLines;
    bool isWrapped = false;

    for (const QString &line : inputLines) {
        QString cleaned = processWhitespace(line);
        int lineWidth = getTextWidth(cleaned);

        if (lineWidth > width) {
            auto [wrapped, wrappedFlag] = wrapLine(cleaned, width, once);
            outputLines.append(wrapped);
            isWrapped = isWrapped || wrappedFlag;

            if (once) {
                return {outputLines.join("\n"), true};
            }
        } else {
            outputLines.append(cleaned);
        }
    }

    return {outputLines.join("\n"), isWrapped};
}
