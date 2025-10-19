#ifndef SCREEN_H
#define SCREEN_H

#include <QRect>

class QScreen;
class Screen
{
public:
    /**
     * @brief 获取当前鼠标所在的屏幕
     * @return QScreen* 如果找到，否则返回主屏幕
     */
    static QScreen* getCurrentScreen();

    /**
     * @brief 获取当前屏幕的几何区域
     * @param available 是否返回可用区域（避开任务栏等），默认 true
     * @return 屏幕的 QRect 区域
     */
    static QRect getCurrentScreenGeometry(bool available = true);
};

#endif // SCREEN_H
