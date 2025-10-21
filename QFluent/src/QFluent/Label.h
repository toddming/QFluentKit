#pragma once

#include <QLabel>

#include "Property.h"

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
