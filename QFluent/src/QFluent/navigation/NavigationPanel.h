#pragma once

#include <QFrame>
#include <QMap>
#include <QString>
#include <QVariant>
#include <functional>
#include <exception>
#include <QVBoxLayout>

#include "FluentGlobal.h"

// 前置声明
class QVBoxLayout;
class QScrollArea;
class QPropertyAnimation;
class NavigationWidget;
class NavigationToolButton;
class NavigationTreeWidget;
class NavigationFlyoutMenu;
class NavigationUserCard;
class Flyout;
class FluentIconBase;

namespace Fluent {
    enum class NavigationItemPosition;
    enum class NavigationDisplayMode;
}

// 自定义异常类
class RouteKeyError : public std::exception {
public:
    explicit RouteKeyError(const QString& message)
        : m_message(message.toUtf8()) {}

    const char* what() const noexcept override {
        return m_message.constData();
    }

private:
    QByteArray m_message;
};

// 导航项结构
struct NavigationItem {
    QString routeKey;
    QString parentRouteKey;
    NavigationWidget* widget;

    NavigationItem() : widget(nullptr) {}

    NavigationItem(const QString& key, const QString& parentKey, NavigationWidget* w)
        : routeKey(key), parentRouteKey(parentKey), widget(w) {}
};

// 导航面板类
class QFLUENT_EXPORT NavigationPanel : public QFrame {
    Q_OBJECT
    Q_PROPERTY(int minimumWidth READ minimumWidth WRITE setMinimumWidth)

public:
    explicit NavigationPanel(QWidget* parent = nullptr, bool isMinimalEnabled = false);
    ~NavigationPanel() override;

    // 禁用拷贝和移动
    NavigationPanel(const NavigationPanel&) = delete;
    NavigationPanel& operator=(const NavigationPanel&) = delete;
    NavigationPanel(NavigationPanel&&) = delete;
    NavigationPanel& operator=(NavigationPanel&&) = delete;

    // 获取导航项
    NavigationWidget* widget(const QString& routeKey);

    // 添加用户卡片
    NavigationUserCard* addUserCard(
        const QString& routeKey,
        const QVariant& avatar,
        const QString& title,
        const QString& subtitle,
        std::function<void()> onClick = nullptr,
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP,
        bool aboveMenuButton = false);

    // 添加导航项
    void addItem(
        const QString& routeKey,
        const FluentIconBase& icon,
        const QString& text,
        const std::function<void()>& onClick = nullptr,
        bool selectable = true,
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP,
        const QString& tooltip = QString(),
        const QString& parentRouteKey = QString());

    void addWidget(
        const QString& routeKey,
        NavigationWidget* widget,
        const std::function<void()>& onClick = nullptr,
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP,
        const QString& tooltip = QString(),
        const QString& parentRouteKey = QString());

    // 插入导航项
    void insertItem(
        int index,
        const QString& routeKey,
        const FluentIconBase& icon,
        const QString& text,
        const std::function<void()>& onClick = nullptr,
        bool selectable = true,
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP,
        const QString& tooltip = QString(),
        const QString& parentRouteKey = QString());

    void insertWidget(
        int index,
        const QString& routeKey,
        NavigationWidget* widget,
        const std::function<void()>& onClick = nullptr,
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP,
        const QString& tooltip = QString(),
        const QString& parentRouteKey = QString());

    // 添加分隔符和标题
    void addSeparator(
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void addItemHeader(
        const QString& text,
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void insertSeparator(
        int index,
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void insertItemHeader(
        int index,
        const QString& text,
        Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    // 移除和设置导航项
    void removeWidget(const QString& routeKey);
    void setCurrentItem(const QString& routeKey);

    // 设置属性
    void setMenuButtonVisible(bool isVisible);
    void setReturnButtonVisible(bool isVisible);
    void setCollapsible(bool on);
    void setExpandWidth(int width);
    void setAcrylicEnabled(bool isEnabled);
    bool isAcrylicEnabled() const;

    // 展开/折叠
    void expand(bool useAnimation = true);
    void collapse();
    void toggle();
    bool isCollapsed() const;

signals:
    void displayModeChanged(Fluent::NavigationDisplayMode mode);

protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onWidgetClicked();
    void onExpandAnimationFinished();

private:
    void initWidget();
    void initLayout();
    void updateAcrylicColor();

    void registerWidget(
        const QString& routeKey,
        const QString& parentRouteKey,
        NavigationWidget* widget,
        const std::function<void()>& onClick,
        const QString& tooltip);

    void insertWidgetToLayout(
        int index,
        NavigationWidget* widget,
        Fluent::NavigationItemPosition position);

    void setWidgetCompacted(bool isCompacted);
    int calculateLayoutMinHeight() const;
    bool canDrawAcrylic() const;

    void showFlyoutNavigationMenu(NavigationTreeWidget* widget);
    void adjustFlyoutMenuSize(
        Flyout* flyout,
        NavigationTreeWidget* widget,
        NavigationFlyoutMenu* menu);

private:
    // 父窗口指针(不拥有)
    QWidget* m_parentWidget;

    // 配置标志
    bool m_isMenuButtonVisible;
    bool m_isReturnButtonVisible;
    bool m_isCollapsible;
    bool m_isAcrylicEnabled;
    bool m_isMinimalEnabled;

    // 尺寸参数
    int m_minimumExpandWidth;
    int m_expandWidth;

    // UI 组件(由 Qt 父子关系管理)
    QScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;
    NavigationToolButton* m_menuButton;

    // 布局(由 Qt 父子关系管理)
    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_topLayout;
    QVBoxLayout* m_bottomLayout;
    QVBoxLayout* m_scrollLayout;

    // 导航项映射
    QMap<QString, NavigationItem> m_items;

    // 动画(由 Qt 父子关系管理)
    QPropertyAnimation* m_expandAnimation;

    // 显示模式
    Fluent::NavigationDisplayMode m_displayMode;
};

// 导航项布局类
class NavigationItemLayout : public QVBoxLayout {
    Q_OBJECT

public:
    explicit NavigationItemLayout(QWidget* parent = nullptr);

    void setGeometry(const QRect& rect) override;
};
