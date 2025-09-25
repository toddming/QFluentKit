// screen.cpp
#include "Screen.h"
#include <QCursor>

QScreen* Screen::getCurrentScreen()
{
    const QPoint cursorPos = QCursor::pos();

    for (QScreen* screen : QApplication::screens()) {
        if (screen && screen->geometry().contains(cursorPos)) {
            return screen;
        }
    }

    // 如果没找到，返回主屏幕
    return QApplication::primaryScreen();
}

QRect Screen::getCurrentScreenGeometry(bool available)
{
    QScreen* screen = getCurrentScreen();

    // 理论上不会为空，但安全起见
    if (!screen) {
        return QRect(0, 0, 1920, 1080);
    }

    return available ? screen->availableGeometry() : screen->geometry();
}