#ifndef TEXTWRAP_H
#define TEXTWRAP_H

#include <QString>
#include <QVector>
#include <QCache>
#include <QHash>
#include <QChar>

#include "Property.h"

class QFLUENT_EXPORT TextWrap
{
public:
    static std::pair<QString, bool> wrap(const QString &text, int width, bool once = true);

    // 可选：设置缓存大小（单位：条目数）
    static void setCacheSize(int size);
    static void clearCache(); // 清空缓存

private:
    enum class CharType {
        Space,
        Asian,
        Latin
    };

    // 缓存键类型
    struct WidthKey {
        QString str;
        bool operator==(const WidthKey &other) const { return str == other.str; }
    };
    struct TokenizeKey {
        QString line;
        bool operator==(const TokenizeKey &other) const { return line == other.line; }
    };
    struct SplitKey {
        QString token;
        int width;
        bool operator==(const SplitKey &other) const { return token == other.token && width == other.width; }
    };

    // 哈希函数（用于 QHash）
    friend uint qHash(const WidthKey &key, uint seed);
    friend uint qHash(const TokenizeKey &key, uint seed);
    friend uint qHash(const SplitKey &key, uint seed);

    // 缓存实例（静态）
    static QCache<WidthKey, int> m_widthCache;
    static QCache<QString, int> m_charWidthCache;
    static QCache<TokenizeKey, QVector<QString>> m_tokenizeCache;
    static QCache<SplitKey, QVector<QString>> m_splitCache;

    // 初始化缓存（懒加载）
    static void initCacheIfNeeded();

    // 获取单个字符宽度（带缓存）
    static int getCharWidth(QChar ch);

    // 获取字符串总宽度（带缓存）
    static int getTextWidth(const QString &str);

    // 获取字符类型
    static CharType getCharType(QChar ch);

    // 处理空白字符
    static QString processWhitespace(const QString &text);

    // 将长 token 按最大宽度切分（带缓存）
    static QVector<QString> splitLongToken(const QString &token, int width);

    // 分词器（带缓存）
    static QVector<QString> tokenize(const QString &line);

    // 内部换行逻辑
    static std::pair<QString, bool> wrapLine(const QString &text, int width, bool once);
};

#endif // TEXTWRAP_H
