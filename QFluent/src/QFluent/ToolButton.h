#pragma once

#include <QToolButton>

#include "Define.h"

class QFLUENT_EXPORT ToolButton : public QToolButton
{
    Q_OBJECT

public:
    explicit ToolButton(QWidget* parent = nullptr);
    ToolButton(FluentIconType::IconType icon, QWidget* parent = nullptr);
    ToolButton(const QString& templatePath, QWidget* parent = nullptr);
    ToolButton(QIcon icon, QWidget* parent = nullptr);

    void setIcon(FluentIconType::IconType icon);
    void setIcon(const QString& templatePath);
    bool isPressed() const { return m_isPressed; }
    bool isHover() const { return m_isHover; }
    FluentIconType::IconType iconType() const { return m_iconType; }
    QString templatePath() const { return m_templatePath; }

signals:
    // No additional signals defined in the Python base ToolButton.

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    virtual void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);

private:
    virtual void postInit();

    bool m_isPressed;
    bool m_isHover;
    FluentIconType::IconType m_iconType;

    QString m_templatePath;
};


class QFLUENT_EXPORT PrimaryToolButton : public ToolButton
{
    Q_OBJECT
public:
    using ToolButton::ToolButton;

protected:
    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
};


class QFLUENT_EXPORT TransparentToolButton : public ToolButton
{
    Q_OBJECT
public:
    using ToolButton::ToolButton;
};





class QFLUENT_EXPORT ToggleToolButton : public ToolButton
{
    Q_OBJECT
public:
    using ToolButton::ToolButton;
protected:
    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;

private:
    void postInit() override;
};


class QFLUENT_EXPORT TransparentToggleToolButton : public ToggleToolButton
{
    Q_OBJECT
public:
    using ToggleToolButton::ToggleToolButton;

};


class QFLUENT_EXPORT PillToolButton : public ToggleToolButton
{
  Q_OBJECT
public:
    using ToggleToolButton::ToggleToolButton;

protected:
    void paintEvent(QPaintEvent* event) override;
};
