#pragma once

#include <QPushButton>
#include <QIcon>
#include <QColor>
#include <QPoint>

#include "Theme.h"
#include "FluentGlobal.h"
#include "ScrollArea.h"
#include "FluentIcon.h"
#include "ToolButton.h"

class QPropertyAnimation;
class QGraphicsDropShadowEffect;
class QHBoxLayout;

// Tab关闭按钮显示模式
enum class TabCloseButtonDisplayMode {
    Always = 0,
    OnHover = 1,
    Never = 2
};

// Tab工具按钮
class TabToolButton : public TransparentToolButton
{
    Q_OBJECT

public:
    explicit TabToolButton(const FluentIconBase &icon, QWidget *parent = nullptr);
    explicit TabToolButton(QWidget *parent = nullptr);

protected:
    void drawIcon(QPainter *painter, const QRectF &rect,
                  Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
};

class QFLUENT_EXPORT TabItem : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QPoint pos READ pos WRITE move)

public:
    explicit TabItem(const QString &text, QWidget *parent = nullptr,
                     const QIcon &icon = QIcon());

    void slideTo(int x, int duration = 250);
    void setShadowEnabled(bool enabled);
    void setRouteKey(const QString &key);
    QString routeKey() const;
    void setBorderRadius(int radius);
    void setSelected(bool selected);
    void setCloseButtonDisplayMode(TabCloseButtonDisplayMode mode);
    void setTextColor(const QColor &color);
    void setSelectedBackgroundColor(const QColor &light, const QColor &dark);
    QPropertyAnimation *slideAnimation() const { return m_slideAnimation; }

    bool isHover() const { return m_isHover; }
    bool isPressed() const { return m_isPressed; }
    bool isSelected() const { return m_isSelected; }

signals:
    void closed();

protected:
    void resizeEvent(QResizeEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private:
    void initWidget();
    bool canShowShadow() const;
    void forwardMouseEvent(QMouseEvent *event);
    void drawSelectedBackground(QPainter *painter);
    void drawNotSelectedBackground(QPainter *painter);
    void drawText(QPainter *painter);

    // 成员变量
    TabToolButton *m_closeButton;
    QGraphicsDropShadowEffect *m_shadowEffect;
    QPropertyAnimation *m_slideAnimation;

    QString m_routeKey;
    QIcon m_icon;
    QColor m_textColor;
    QColor m_lightSelectedBackgroundColor;
    QColor m_darkSelectedBackgroundColor;

    int m_borderRadius;
    bool m_isHover;
    bool m_isPressed;
    bool m_isSelected;
    bool m_isShadowEnabled;
    TabCloseButtonDisplayMode m_closeButtonDisplayMode;
};

class QFLUENT_EXPORT TabBar : public SingleDirectionScrollArea
{
    Q_OBJECT
    Q_PROPERTY(bool movable READ isMovable WRITE setMovable)
    Q_PROPERTY(bool scrollable READ isScrollable WRITE setScrollable)
    Q_PROPERTY(int tabMaxWidth READ tabMaximumWidth WRITE setTabMaximumWidth)
    Q_PROPERTY(int tabMinWidth READ tabMinimumWidth WRITE setTabMinimumWidth)
    Q_PROPERTY(bool tabShadowEnabled READ isTabShadowEnabled WRITE setTabShadowEnabled)

public:
    explicit TabBar(QWidget *parent = nullptr);

    void setAddButtonVisible(bool visible);

    TabItem *addTab(const QString &routeKey, const QString &text,
                    const QIcon &icon = QIcon(),
                    std::function<void()> onClick = nullptr);
    TabItem *insertTab(int index, const QString &routeKey, const QString &text,
                       const QIcon &icon = QIcon(),
                       std::function<void()> onClick = nullptr);
    void removeTab(int index);
    void removeTabByKey(const QString &routeKey);

    void setCurrentIndex(int index);
    void setCurrentTab(const QString &routeKey);
    int currentIndex() const { return m_currentIndex; }
    TabItem *currentTab() const;

    void setCloseButtonDisplayMode(TabCloseButtonDisplayMode mode);
    TabCloseButtonDisplayMode closeButtonDisplayMode() const { return m_closeButtonDisplayMode; }

    TabItem *tabItem(int index) const;
    TabItem *tab(const QString &routeKey) const;

    QRect tabRegion() const;
    QRect tabRect(int index) const;

    QString tabText(int index) const;
    QIcon tabIcon(int index) const;
    QString tabToolTip(int index) const;

    void setTabsClosable(bool closable);
    bool tabsClosable() const;

    void setTabIcon(int index, const QIcon &icon);
    void setTabText(int index, const QString &text);
    void setTabVisible(int index, bool visible);
    void setTabTextColor(int index, const QColor &color);
    void setTabToolTip(int index, const QString &toolTip);
    void setTabSelectedBackgroundColor(const QColor &light, const QColor &dark);

    void setTabShadowEnabled(bool enabled);
    bool isTabShadowEnabled() const { return m_isTabShadowEnabled; }

    void setMovable(bool movable);
    bool isMovable() const { return m_isMovable; }

    void setScrollable(bool scrollable);
    bool isScrollable() const { return m_isScrollable; }

    void setTabMaximumWidth(int width);
    int tabMaximumWidth() const { return m_tabMaxWidth; }

    void setTabMinimumWidth(int width);
    int tabMinimumWidth() const { return m_tabMinWidth; }

    int count() const { return m_items.size(); }

    TabToolButton *addButton() const { return m_addButton; }

signals:
    void currentChanged(int index);
    void tabBarClicked(int index);
    void tabCloseRequested(int index);
    void tabAddRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onItemPressed();
    void adjustLayout();

private:
    void initWidget();
    void initLayout();
    void swapItem(int index);
    void forceFinishAdjustment();

    // 成员变量
    QWidget *m_view;
    QHBoxLayout *m_hBoxLayout;
    QHBoxLayout *m_itemLayout;
    QHBoxLayout *m_widgetLayout;
    TabToolButton *m_addButton;

    QList<TabItem *> m_items;
    QMap<QString, TabItem *> m_itemMap;

    QColor m_lightSelectedBackgroundColor;
    QColor m_darkSelectedBackgroundColor;
    QPoint m_dragPos;

    int m_currentIndex;
    int m_tabMaxWidth;
    int m_tabMinWidth;

    bool m_isMovable;
    bool m_isScrollable;
    bool m_isTabShadowEnabled;
    bool m_isDragging;

    TabCloseButtonDisplayMode m_closeButtonDisplayMode;

    bool m_isAdjustingLayout;
};
