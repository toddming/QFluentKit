#ifndef NAVIGATION_PANEL_H
#define NAVIGATION_PANEL_H

#include <QFrame>
#include <QVBoxLayout>

#include "Define.h"
#include "FluentIcon.h"

class QScrollArea;
class ProfileCard;
class NavigationWidget;
class QPropertyAnimation;
class NavigationToolButton;
class NavigationTreeWidget;
class NavigationAvatarWidget;
class RouteKeyError : public std::exception {
public:
    RouteKeyError(const QString& message) : m_message(message.toUtf8()) {}
    const char* what() const noexcept override { return m_message.constData(); }

private:
    QByteArray m_message;
};

// 导航项结构
struct NavigationItem {
    QString routeKey;
    QString parentRouteKey;
    NavigationWidget* widget;
};

// 导航面板类
class QFLUENT_EXPORT NavigationPanel : public QFrame {
    Q_OBJECT

public:
    explicit NavigationPanel(QWidget* parent = nullptr, bool isMinimalEnabled = false);

    // 公共方法
    NavigationWidget* widget(const QString& routeKey);
    void addItem(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                 const std::function<void()>& onClick = nullptr, bool selectable = true,
                 NavigationType::NavigationItemPosition position = NavigationType::NavigationItemPosition::TOP,
                 const QString& tooltip = QString(), const QString& parentRouteKey = QString());

    void addWidget(const QString& routeKey, NavigationWidget* widget,
                   const std::function<void()>& onClick = nullptr,
                   NavigationType::NavigationItemPosition position = NavigationType::NavigationItemPosition::TOP,
                   const QString& tooltip = QString(), const QString& parentRouteKey = QString());

    void insertItem(int index, const QString& routeKey, const FluentIconBase& icon, const QString& text,
                   const std::function<void()>& onClick = nullptr, bool selectable = true,
                   NavigationType::NavigationItemPosition position = NavigationType::NavigationItemPosition::TOP,
                   const QString& tooltip = QString(), const QString& parentRouteKey = QString());

    void insertWidget(int index, const QString& routeKey, NavigationWidget* widget,
                     const std::function<void()>& onClick = nullptr,
                     NavigationType::NavigationItemPosition position = NavigationType::NavigationItemPosition::TOP,
                     const QString& tooltip = QString(), const QString& parentRouteKey = QString());

    void addSeparator(NavigationType::NavigationItemPosition position = NavigationType::NavigationItemPosition::TOP);
    void insertSeparator(int index, NavigationType::NavigationItemPosition position = NavigationType::NavigationItemPosition::TOP);
    void removeWidget(const QString& routeKey);
    void setCurrentItem(const QString& routeKey);

    void setMenuButtonVisible(bool isVisible);
    void setReturnButtonVisible(bool isVisible);
    void setCollapsible(bool on);
    void setExpandWidth(int width);
    void setAcrylicEnabled(bool isEnabled);
    bool isAcrylicEnabled() const;

    void expand(bool useAni = true);
    void collapse();
    void toggle();
    bool isCollapsed() const;

    // 信号
    signals:
    void displayModeChanged(NavigationType::NavigationDisplayMode mode);

protected:
    void paintEvent(QPaintEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    void initWidget();
    void initLayout();
    void updateAcrylicColor();
    void registerWidget(const QString& routeKey, const QString& parentRouteKey,
                       NavigationWidget* widget, const std::function<void()>& onClick,
                       const QString& tooltip);
    void insertWidgetToLayout(int index, NavigationWidget* widget, NavigationType::NavigationItemPosition position);
    void onWidgetClicked();
    void onExpandAniFinished();
    void setWidgetCompacted(bool isCompacted);
    int layoutMinHeight();
    bool canDrawAcrylic();
    void showFlyoutNavigationMenu(NavigationTreeWidget* widget);
    // void adjustFlyoutMenuSize(Flyout* flyout, NavigationTreeWidget* widget, NavigationFlyoutMenu* menu);

private:
    QWidget* m_parent;
    bool m_isMenuButtonVisible;
    bool m_isReturnButtonVisible;
    bool m_isCollapsible;
    bool m_isAcrylicEnabled;

    // AcrylicBrush* m_acrylicBrush;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;

    NavigationToolButton* m_menuButton;
    NavigationAvatarWidget * m_avatarWidget;

    QVBoxLayout* m_vBoxLayout;
    QVBoxLayout* m_topLayout;
    QVBoxLayout* m_bottomLayout;
    QVBoxLayout* m_scrollLayout;

    QMap<QString, NavigationItem> m_items;
    // Router* m_history;

    QPropertyAnimation* m_expandAni;
    int m_expandWidth;

    bool m_isMinimalEnabled;
    NavigationType::NavigationDisplayMode m_displayMode;
};

// 导航项布局类
class NavigationItemLayout : public QVBoxLayout {
public:
    explicit NavigationItemLayout(QWidget* parent = nullptr) : QVBoxLayout(parent) {}

    void setGeometry(const QRect& rect) override;
};

#endif // NAVIGATION_PANEL_H
