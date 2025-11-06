#pragma once

#include <QListWidget>

#include "Define.h"

class MenuAnimation;
class QFLUENT_EXPORT MenuActionListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit MenuActionListWidget(QWidget* parent = nullptr);
    void setItemHeight(int height);
    void setMaxVisibleItems(int num);
    int maxVisibleItems() const;
    void adjustSize(const QPoint& pos = QPoint(), MenuAnimationType::MenuAnimation aniType = MenuAnimationType::MenuAnimation::NONE);
    int itemHeight() const;
    int itemsHeight() const;

    int heightForAnimation(const QPoint& pos, MenuAnimationType::MenuAnimation aniType = MenuAnimationType::MenuAnimation::NONE);
    QPoint availableViewSize(const QPoint& pos, MenuAnimationType::MenuAnimation aniType = MenuAnimationType::MenuAnimation::NONE);

protected:
    void wheelEvent(QWheelEvent* e) override;
};
