#pragma once

#include <QString>
#include <QVector>
#include <QCache>
#include <QHash>
#include <QChar>

#include "FluentGlobal.h"

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
    friend FluentQHashReturnType qHash(const WidthKey &key, FluentQHashReturnType seed);
    friend FluentQHashReturnType qHash(const TokenizeKey &key, FluentQHashReturnType seed);
    friend FluentQHashReturnType qHash(const SplitKey &key, FluentQHashReturnType seed);

    // 缓存实例
    QCache<WidthKey, int> m_widthCache;
    QCache<QString, int> m_charWidthCache;
    QCache<TokenizeKey, QVector<QString>> m_tokenizeCache;
    QCache<SplitKey, QVector<QString>> m_splitCache;

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
FluentQHashReturnType qHash(const TextWrapPrivate::WidthKey &key, FluentQHashReturnType seed = 0);
FluentQHashReturnType qHash(const TextWrapPrivate::TokenizeKey &key, FluentQHashReturnType seed = 0);
FluentQHashReturnType qHash(const TextWrapPrivate::SplitKey &key, FluentQHashReturnType seed = 0);
