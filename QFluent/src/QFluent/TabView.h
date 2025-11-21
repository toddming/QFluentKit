#ifndef TAB_VIEW_H
#define TAB_VIEW_H

#include <QPushButton>

#include "Theme.h"
#include "FluentGlobal.h"
#include "ScrollArea.h"
#include "FluentIcon.h"
#include "ToolButton.h"

// Tab关闭按钮显示模式
enum class TabCloseButtonDisplayMode {
    ALWAYS = 0,
    ON_HOVER = 1,
    NEVER = 2
};

// Tab工具按钮
class TabToolButton : public TransparentToolButton {
    Q_OBJECT

public:
    explicit TabToolButton(const FluentIconBase& icon, QWidget* parent = nullptr);
    explicit TabToolButton(QWidget* parent = nullptr);

protected:
    void drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
};

class QPropertyAnimation;
class QGraphicsDropShadowEffect;
class QFLUENT_EXPORT TabItem : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QPoint pos READ pos WRITE move)

public:
    explicit TabItem(const QString& text, QWidget* parent = nullptr, const QIcon& icon = QIcon());

    void slideTo(int x, int duration = 250);
    void setShadowEnabled(bool isEnabled);
    void setRouteKey(const QString& key);
    QString routeKey() const;
    void setBorderRadius(int radius);
    void setSelected(bool isSelected);
    void setCloseButtonDisplayMode(TabCloseButtonDisplayMode mode);
    void setTextColor(const QColor& color);
    void setSelectedBackgroundColor(const QColor& light, const QColor& dark);
    QPropertyAnimation* getSlideAni();

    bool isHover = false;
    bool isPressed = false;
    bool isSelected = false;

signals:
    void closed();

protected:
    void postInit();
    void resizeEvent(QResizeEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

private:
    void initWidget();
    bool canShowShadow() const;
    void forwardMouseEvent(QMouseEvent* event);
    void drawSelectedBackground(QPainter* painter);
    void drawNotSelectedBackground(QPainter* painter);
    void drawText(QPainter* painter);

    int borderRadius;
    bool isShadowEnabled;
    TabCloseButtonDisplayMode closeButtonDisplayMode;
    QString m_routeKey;
    QColor textColor;
    QColor lightSelectedBackgroundColor;
    QColor darkSelectedBackgroundColor;
    TabToolButton* closeButton;
    QGraphicsDropShadowEffect* shadowEffect;
    QPropertyAnimation* slideAni;
    QIcon m_icon;
};

class QHBoxLayout;
class QFLUENT_EXPORT TabBar : public SingleDirectionScrollArea {
    Q_OBJECT
    Q_PROPERTY(bool movable READ isMovable WRITE setMovable)
    Q_PROPERTY(bool scrollable READ isScrollable WRITE setScrollable)
    Q_PROPERTY(int tabMaxWidth READ tabMaximumWidth WRITE setTabMaximumWidth)
    Q_PROPERTY(int tabMinWidth READ tabMinimumWidth WRITE setTabMinimumWidth)
    Q_PROPERTY(bool tabShadowEnabled READ isTabShadowEnabled WRITE setTabShadowEnabled)

public:
    explicit TabBar(QWidget* parent = nullptr);

    void setAddButtonVisible(bool isVisible);
    TabItem* addTab(const QString& routeKey, const QString& text, const QIcon& icon = QIcon(),
                    std::function<void()> onClick = nullptr);
    TabItem* insertTab(int index, const QString& routeKey, const QString& text,
                       const QIcon& icon = QIcon(), std::function<void()> onClick = nullptr);
    void removeTab(int index);
    void removeTabByKey(const QString& routeKey);
    void setCurrentIndex(int index);
    void setCurrentTab(const QString& routeKey);
    int currentIndex() const;
    TabItem* currentTab();
    void setCloseButtonDisplayMode(TabCloseButtonDisplayMode mode);
    TabItem* tabItem(int index);
    TabItem* tab(const QString& routeKey);
    QRect tabRegion() const;
    QRect tabRect(int index);
    QString tabText(int index);
    QIcon tabIcon(int index);
    QString tabToolTip(int index);
    void setTabsClosable(bool isClosable);
    bool tabsClosable() const;
    void setTabIcon(int index, const QIcon& icon);
    void setTabText(int index, const QString& text);
    void setTabVisible(int index, bool isVisible);
    void setTabTextColor(int index, const QColor& color);
    void setTabToolTip(int index, const QString& toolTip);
    void setTabSelectedBackgroundColor(const QColor& light, const QColor& dark);
    void setTabShadowEnabled(bool isEnabled);
    bool isTabShadowEnabled() const;
    void setMovable(bool movable);
    bool isMovable() const;
    void setScrollable(bool scrollable);
    void setTabMaximumWidth(int width);
    void setTabMinimumWidth(int width);
    int tabMaximumWidth() const;
    int tabMinimumWidth() const;
    bool isScrollable() const;
    int count() const;

signals:
    void currentChanged(int index);
    void tabBarClicked(int index);
    void tabCloseRequested(int index);
    void tabAddRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void onItemPressed();
    void adjustLayout();

private:
    void initWidget();
    void initLayout();
    void swapItem(int index);

    QList<TabItem*> items;
    QMap<QString, TabItem*> itemMap;
    int m_currentIndex;
    bool m_isMovable;
    bool m_isScrollable;
    bool m_isTabShadowEnabled;
    int m_tabMaxWidth;
    int m_tabMinWidth;
    QPoint dragPos;
    bool isDraging;
    QColor lightSelectedBackgroundColor;
    QColor darkSelectedBackgroundColor;
    TabCloseButtonDisplayMode closeButtonDisplayMode;
    QWidget* view;
    QHBoxLayout* hBoxLayout;
    QHBoxLayout* itemLayout;
    QHBoxLayout* widgetLayout;
    TabToolButton* addButton;
};

#endif // TAB_VIEW_H
