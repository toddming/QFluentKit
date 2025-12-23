#pragma once

#include <QString>
#include <QVector>
#include <QCache>
#include <QHash>
#include <QChar>

class TextWrap;

class TextWrapPrivate
{
    Q_DECLARE_PUBLIC(TextWrap)

public:
    explicit TextWrapPrivate(TextWrap *q);
    ~TextWrapPrivate();

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
        bool operator==(const SplitKey &other) const {
            return token == other.token && width == other.width;
        }
    };

    // 哈希函数友元声明
    friend uint qHash(const WidthKey &key, uint seed);
    friend uint qHash(const TokenizeKey &key, uint seed);
    friend uint qHash(const SplitKey &key, uint seed);

    // 缓存实例
    QCache<WidthKey, int> widthCache;
    QCache<QString, int> charWidthCache;
    QCache<TokenizeKey, QVector<QString>> tokenizeCache;
    QCache<SplitKey, QVector<QString>> splitCache;

    // 成员函数
    void initCache();
    int getCharWidth(QChar ch);
    int getTextWidth(const QString &str);
    CharType getCharType(QChar ch);
    QString processWhitespace(const QString &text);
    QVector<QString> splitLongToken(const QString &token, int width);
    QVector<QString> tokenize(const QString &line);
    std::pair<QString, bool> wrapLine(const QString &text, int width, bool once);
    std::pair<QString, bool> wrapImpl(const QString &text, int width, bool once);

private:
    TextWrap *q_ptr;
};

// 哈希函数声明
uint qHash(const TextWrapPrivate::WidthKey &key, uint seed = 0);
uint qHash(const TextWrapPrivate::TokenizeKey &key, uint seed = 0);
uint qHash(const TextWrapPrivate::SplitKey &key, uint seed = 0);
