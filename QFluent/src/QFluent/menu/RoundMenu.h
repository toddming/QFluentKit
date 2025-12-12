#pragma once

#include <QMenu>
#include <QLayout>
#include <QIcon>
#include <QPainter>
#include <QStyleOption>

#include "FluentGlobal.h"

class QListWidgetItem;
class RoundMenuPrivate;
class MenuActionListWidget;

class QFLUENT_EXPORT RoundMenu : public QMenu
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(dPtr, RoundMenu)

public:
    explicit RoundMenu(const QString &title = QString(), QWidget *parent = nullptr);
    ~RoundMenu() override;

    void setItemHeight(int height);
    void setMaxVisibleItems(int num);

    void addAction(QAction *action);
    void insertAction(QAction *before, QAction *action);
    void removeAction(QAction *action);

    void addMenu(RoundMenu *menu);
    void insertMenu(QAction *before, RoundMenu *menu);
    void removeMenu(RoundMenu *menu);

    void addSeparator();
    void clear();
    void addWidget(QWidget *widget, bool selectable = false);

    QList<QAction *> menuActions() const;
    void setDefaultAction(QAction *action);

    MenuActionListWidget *view() const;
    void adjustMenuSize();
    int itemHeight() const;

    void setHideByClick(bool enabled);

    void hideMenu(bool isHideBySystem = false);
    void exec(const QPoint &pos, bool animate = true,
              Fluent::MenuAnimation aniType = Fluent::MenuAnimation::DROP_DOWN);

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    virtual int adjustItemText(QListWidgetItem *item, QAction *action);

private:
    QScopedPointer<RoundMenuPrivate> dPtr;
};
