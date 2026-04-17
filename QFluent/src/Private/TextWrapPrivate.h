#pragma once

#include <QString>
#include <QVector>
#include <QCache>
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

    // 缓存实例
    QCache<QString, int> m_widthCache;
    QCache<QString, int> m_charWidthCache;
    QCache<QString, QVector<QString>> m_tokenizeCache;
    QCache<QString, QVector<QString>> m_splitCache;

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
