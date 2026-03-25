#pragma once

#include <QUrl>
#include <QLabel>
#include <QPushButton>

#include "FluentGlobal.h"

class QFLUENT_EXPORT FluentLabelBase : public QLabel {
    Q_OBJECT

public:
    explicit FluentLabelBase(int fontSize = 14, QFont::Weight weight = QFont::Normal, QWidget* parent = nullptr);
    explicit FluentLabelBase(const QString& text, int fontSize = 14, QFont::Weight weight = QFont::Normal,  QWidget* parent = nullptr);

    int pixelFontSize() const;

    bool strikeOut() const;

    bool underline() const;

    void setTextColor(const QColor& lightColor, const QColor& darkColor);

    void setPixelFontSize(int size);
    void setStrikeOut(bool isStrikeOut);
    void setUnderline(bool isUnderline);

};

class QFLUENT_EXPORT CaptionLabel : public FluentLabelBase {
    Q_OBJECT
public:
    explicit CaptionLabel(QWidget* parent = nullptr);
    explicit CaptionLabel(const QString& text, QWidget* parent = nullptr);

};

class QFLUENT_EXPORT BodyLabel : public FluentLabelBase {
    Q_OBJECT
public:
    explicit BodyLabel(QWidget* parent = nullptr);
    explicit BodyLabel(const QString& text, QWidget* parent = nullptr);
};

class QFLUENT_EXPORT StrongBodyLabel : public FluentLabelBase {
    Q_OBJECT
public:
    explicit StrongBodyLabel(QWidget* parent = nullptr);
    explicit StrongBodyLabel(const QString& text, QWidget* parent = nullptr);
};

class QFLUENT_EXPORT SubtitleLabel : public FluentLabelBase {
    Q_OBJECT
public:
    explicit SubtitleLabel(QWidget* parent = nullptr);
    explicit SubtitleLabel(const QString& text, QWidget* parent = nullptr);
};

class QFLUENT_EXPORT TitleLabel : public FluentLabelBase {
    Q_OBJECT
public:
    explicit TitleLabel(QWidget* parent = nullptr);
    explicit TitleLabel(const QString& text, QWidget* parent = nullptr);
};

class QFLUENT_EXPORT LargeTitleLabel : public FluentLabelBase {
    Q_OBJECT
public:
    explicit LargeTitleLabel(QWidget* parent = nullptr);
    explicit LargeTitleLabel(const QString& text, QWidget* parent = nullptr);
};

class QFLUENT_EXPORT DisplayLabel : public FluentLabelBase {
    Q_OBJECT
public:
    explicit DisplayLabel(QWidget* parent = nullptr);
    explicit DisplayLabel(const QString& text, QWidget* parent = nullptr);
};


class QFLUENT_EXPORT HyperlinkLabel : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl)
    Q_PROPERTY(bool underlineVisible READ isUnderlineVisible WRITE setUnderlineVisible)

public:
    explicit HyperlinkLabel(QWidget *parent = nullptr);
    explicit HyperlinkLabel(const QString &text, QWidget *parent = nullptr);
    HyperlinkLabel(const QUrl &url, const QString &text, QWidget *parent = nullptr);

    QUrl url() const;
    void setUrl(const QUrl &url);

    bool isUnderlineVisible() const;
    void setUnderlineVisible(bool isVisible);

private slots:
    void onClicked();

private:
    void init();

private:
    QUrl m_url;
    bool m_isUnderlineVisible;
};
