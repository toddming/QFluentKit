#include "TextWrapPrivate.h"
#include "TextWrap.h"

// ================ 哈希函数实现 ================
uint qHash(const TextWrapPrivate::WidthKey &key, uint seed)
{
    return qHash(key.str, seed);
}

uint qHash(const TextWrapPrivate::TokenizeKey &key, uint seed)
{
    return qHash(key.line, seed);
}

uint qHash(const TextWrapPrivate::SplitKey &key, uint seed)
{
    seed = qHash(key.token, seed);
    seed = qHash(key.width, seed);
    return seed;
}

// ================ TextWrapPrivate 实现 ================
TextWrapPrivate::TextWrapPrivate(TextWrap *q)
    : q_ptr(q)
{
    initCache();
}

TextWrapPrivate::~TextWrapPrivate()
{
}

void TextWrapPrivate::initCache()
{
    widthCache.setMaxCost(1000);
    charWidthCache.setMaxCost(2000);
    tokenizeCache.setMaxCost(500);
    splitCache.setMaxCost(500);
}

int TextWrapPrivate::getCharWidth(QChar ch)
{
    QString key = QString(ch);
    if (int* cached = charWidthCache.object(key)) {
        return *cached;
    }

    // 获取 Unicode 码点
    uint ucs4 = ch.unicode();

    // 基本 ASCII 空白和控制字符(宽度为 1)
    if (ucs4 < 0x20 || ucs4 == 0x7F) {
        auto *value = new int(1);
        charWidthCache.insert(key, value);
        return 1;
    }

    // 常见全角(Wide)字符范围
    // 参考 Unicode TR#11: East Asian Width
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
        charWidthCache.insert(key, value);
        return 2;
    }

    // 半角片假名等(宽度为 1)
    if ((ucs4 >= 0xFF61 && ucs4 <= 0xFF9F)) { // Halfwidth Katakana
        auto *value = new int(1);
        charWidthCache.insert(key, value);
        return 1;
    }

    // 默认:窄字符(Latin, 数字, 符号等)
    auto *value = new int(1);
    charWidthCache.insert(key, value);
    return 1;
}

int TextWrapPrivate::getTextWidth(const QString &str)
{
    if (str.isEmpty()) return 0;

    WidthKey key{str};
    if (int* cached = widthCache.object(key)) {
        return *cached;
    }

    int width = 0;
    for (QChar ch : str) {
        width += getCharWidth(ch);
    }

    auto *value = new int(width);
    widthCache.insert(key, value);
    return width;
}

TextWrapPrivate::CharType TextWrapPrivate::getCharType(QChar ch)
{
    if (ch.isSpace()) {
        return CharType::Space;
    }
    if (getCharWidth(ch) == 1) {
        return CharType::Latin;
    }
    return CharType::Asian;
}

QString TextWrapPrivate::processWhitespace(const QString &text)
{
    return text.simplified();
}

QVector<QString> TextWrapPrivate::splitLongToken(const QString &token, int width)
{
    SplitKey key{token, width};
    if (auto* cached = splitCache.object(key)) {
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
    splitCache.insert(key, value);
    return chunks;
}

QVector<QString> TextWrapPrivate::tokenize(const QString &line)
{
    if (line.isEmpty()) return {};

    TokenizeKey key{line};
    if (auto* cached = tokenizeCache.object(key)) {
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
    tokenizeCache.insert(key, value);
    return tokens;
}

std::pair<QString, bool> TextWrapPrivate::wrapLine(const QString &text, int width, bool once)
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

std::pair<QString, bool> TextWrapPrivate::wrapImpl(const QString &text, int width, bool once)
{
    if (width <= 0) {
        return {"", true};
    }

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
