#pragma once

#include <QMenu>
#include <QLayout>
#include <QIcon>
#include <QPainter>
#include <QStyleOption>

#include "Define.h"


class QListWidgetItem;
class RoundMenuPrivate;
class MenuActionListWidget;
class QFLUENT_EXPORT RoundMenu : public QMenu {
    Q_OBJECT
    Q_Q_CREATE(RoundMenu)

public:
    explicit RoundMenu(const QString& title = "", QWidget* parent = nullptr);
    ~RoundMenu();

    void setItemHeight(int height);
    void setMaxVisibleItems(int num);
    void addAction(QAction* action);
    void insertAction(QAction* before, QAction* action);
    void removeAction(QAction* action);
    void addMenu(RoundMenu* menu);
    void insertMenu(QAction *before, RoundMenu *menu);
    void removeMenu(RoundMenu* menu);
    void addSeparator();
    void clear();
    QList<QAction *> menuActions();
    void setDefaultAction(QAction *action);
    MenuActionListWidget* view();
    void adjustMenuSize();
    int itemHeight() const;

    void hideMenu(bool isHideBySystem = false);

    void exec(const QPoint& pos, bool animate = true,
              MenuAnimationType::MenuAnimation aniType = MenuAnimationType::MenuAnimation::DROP_DOWN);

    void addWidget(QWidget *widget, bool selectable = false);

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent* e) override;
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    virtual int adjustItemText(QListWidgetItem *item, QAction *action);

};
