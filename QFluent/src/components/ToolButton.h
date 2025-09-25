#pragma once

#include <QToolButton>
#include <QSize>
#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyleOptionButton>
#include <QApplication>
#include <memory>

#include "../common/Theme.h"
#include "FluentIcon.h"
#include "../Property.h"

class QFLUENT_EXPORT ToolButton : public QToolButton
{
    Q_OBJECT

public:
    explicit ToolButton(QWidget* parent = nullptr);
    ToolButton(Icon::IconType icon, QWidget* parent = nullptr);
    ToolButton(const QString& fillPath, const QString& baseName, const QString& lightSuffix, const QString& darkSuffix,
               QWidget* parent = nullptr);

    void setIcon(Icon::IconType icon);
    void setIcon(const QString& fillPath, const QString& baseName, const QString& lightSuffix, const QString& darkSuffix);
    bool isPressed() const { return m_isPressed; }
    bool isHover() const { return m_isHover; }

signals:
    // No additional signals defined in the Python base ToolButton.

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    virtual void postInit();

    bool m_isPressed;
    bool m_isHover;
    Icon::IconType m_iconType;

    QString m_fillPath;
    QString m_baseName;
    QString m_lightSuffix;
    QString m_darkSuffix;
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

private:
    void postInit() override;
};


class QFLUENT_EXPORT TransparentToggleToolButton : public ToggleToolButton
{
    Q_OBJECT
public:
    using ToggleToolButton::ToggleToolButton;

};
