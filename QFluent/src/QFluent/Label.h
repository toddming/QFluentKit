#pragma once

#include <QLabel>
#include <QFont>
#include <QColor>
#include <QPixmap>
#include <QMetaObject>

#include "Theme.h"
#include "Property.h"

class QFLUENT_EXPORT FluentLabelBase : public QLabel {
    Q_OBJECT
    Q_PROPERTY(int pixelFontSize READ pixelFontSize WRITE setPixelFontSize)
    Q_PROPERTY(bool strikeOut READ strikeOut WRITE setStrikeOut)
    Q_PROPERTY(bool underline READ underline WRITE setUnderline)

public:
    // 构造函数重载 (模拟 singledispatchmethod)
    explicit FluentLabelBase(int fontSize = 14, QFont::Weight weight = QFont::Normal, QWidget* parent = nullptr);
    explicit FluentLabelBase(const QString& text, int fontSize = 14, QFont::Weight weight = QFont::Normal,  QWidget* parent = nullptr);

    // 字体属性访问器
    int pixelFontSize() const;
    bool strikeOut() const;

    bool underline() const;

public slots:
    // 设置文本颜色
    void setTextColor(const QColor& color);

    // 字体属性设置器
    void setPixelFontSize(int size);
    void setStrikeOut(bool isStrikeOut);
    void setUnderline(bool isUnderline);


protected:
    void init(); // 初始化公共部分

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
    explicit TitleLabel(QWidget* parent = nullptr, QFont::Weight weight = QFont::DemiBold);
    explicit TitleLabel(const QString& text, QWidget* parent = nullptr, QFont::Weight weight = QFont::DemiBold);
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
