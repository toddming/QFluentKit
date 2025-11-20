#pragma once

#include <QPointer>
#include "FluentGlobal.h"
#include "QFluent/FluentTitleBar.h"

class QLabel;
class QWidget;
class QPushButton;
class FluentTitleBarPrivate
{
    Q_DECLARE_PUBLIC(FluentTitleBar)
public:
    FluentTitleBar *q_ptr{nullptr};

private:
    QPushButton *_minButton;
    QPushButton *_maxButton;
    QPushButton *_closeButton;
    QPushButton *_themeButton;
    QPushButton *_backButton;
    QPushButton *_iconButton;
    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QPointer<QWidget> _hostWidget;
    Fluent::WindowButtonHints _buttonFlags;
};
