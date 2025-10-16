#pragma once

#include <QPointer>
#include "Define.h"

class QLabel;
class QWidget;
class QPushButton;
class FluentTitleBar;
class FluentTitleBarPrivate
{
    Q_D_CREATE(FluentTitleBar)
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
    AppBarType::ButtonFlags _buttonFlags;
};
