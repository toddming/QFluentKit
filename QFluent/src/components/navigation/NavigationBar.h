#ifndef NAVIGATION_BAR_H
#define NAVIGATION_BAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMap>
#include <QPropertyAnimation>
#include <QFont>
#include <QEvent>
#include <QMouseEvent>
#include <QIcon>
#include <QColor>

#include "NavigationWidget.h"
#include "NavigationPanel.h"


// IconSlideAnimation 类
class IconSlideAnimation : public QPropertyAnimation {
    Q_OBJECT
    Q_PROPERTY(float offset READ getOffset WRITE setOffset)
public:
    explicit IconSlideAnimation(QWidget* parent = nullptr);
    float getOffset() const;
    void setOffset(float value);
    void slideDown();
    void slideUp();

protected:
    QVariant animateValue(const QVariant& startValue, const QVariant& endValue, float progress);

private:
    float _offset;
    float maxOffset;
};

// NavigationBarPushButton 类
class NavigationBarPushButton : public NavigationPushButton {
    Q_OBJECT
public:
    explicit NavigationBarPushButton(Icon::IconType icon, const QString& text, bool isSelectable = true,
                                   Icon::IconType selectedIcon = Icon::IconType::NONE, QWidget* parent = nullptr);
    void setSelectedColor(const QColor& light, const QColor& dark);
    void setSelectedIcon(Icon::IconType icon);
    void setSelectedTextVisible(bool isVisible);
    void setSelected(bool isSelected);
    IconSlideAnimation* iconAni() { return m_iconAni; }

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    void _drawBackground(QPainter& painter);
    void _drawIcon(QPainter& painter);
    void _drawText(QPainter& painter);

private:
    Icon::IconType _selectedIcon;
    bool _isSelectedTextVisible;
    QColor lightSelectedColor;
    QColor darkSelectedColor;
    IconSlideAnimation *m_iconAni;
};

// NavigationBar 类
class NavigationBar : public QWidget {
    Q_OBJECT

public:
    explicit NavigationBar(QWidget* parent = nullptr);
    ~NavigationBar() override;

    // 公共方法
    NavigationWidget* widget(const QString& routeKey);
    void addItem(const QString& routeKey, Icon::IconType icon, const QString& text,
                 const std::function<void()>& onClick = nullptr, bool selectable = true,
                 Icon::IconType selectedIcon = Icon::IconType::NONE,
                 NavigationItemPosition position = NavigationItemPosition::TOP);
    void addWidget(const QString& routeKey, NavigationWidget* widget,
                   const std::function<void()>& onClick = nullptr,
                   NavigationItemPosition position = NavigationItemPosition::TOP);
    void insertItem(int index, const QString& routeKey, Icon::IconType icon, const QString& text,
                   const std::function<void()>& onClick = nullptr, bool selectable = true,
                   Icon::IconType selectedIcon = Icon::IconType::NONE,
                   NavigationItemPosition position = NavigationItemPosition::TOP);
    void insertWidget(int index, const QString& routeKey, NavigationWidget* widget,
                     const std::function<void()>& onClick = nullptr,
                     NavigationItemPosition position = NavigationItemPosition::TOP);
    void addSeparator(NavigationItemPosition position = NavigationItemPosition::TOP);
    void insertSeparator(int index, NavigationItemPosition position = NavigationItemPosition::TOP);
    void removeWidget(const QString& routeKey);
    void setCurrentItem(const QString& routeKey);
    void setFont(const QFont& font);
    void setSelectedTextVisible(bool isVisible);
    void setSelectedColor(const QColor& light, const QColor& dark);
    QList<NavigationBarPushButton*> buttons() const;

signals:
    void displayModeChanged(NavigationDisplayMode mode);

protected:
    void paintEvent(QPaintEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    void __initWidget();
    void __initLayout();
    void _registerWidget(const QString& routeKey, NavigationWidget* widget, const std::function<void()>& onClick);
    void _insertWidgetToLayout(int index, NavigationWidget* widget, NavigationItemPosition position);
    void _onWidgetClicked();
    void _onExpandAniFinished();
    void setWidgetCompacted(bool isCompacted);

private:
    QWidget* m_parent;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;
    QVBoxLayout* m_vBoxLayout;
    QVBoxLayout* m_topLayout;
    QVBoxLayout* m_bottomLayout;
    QVBoxLayout* m_scrollLayout;
    QMap<QString, NavigationItem> m_items;
    QPropertyAnimation* m_expandAni;
    int m_expandWidth;
    bool m_isMinimalEnabled;
    NavigationDisplayMode m_displayMode;
    QColor m_lightSelectedColor;
    QColor m_darkSelectedColor;
    bool m_isMenuButtonVisible;
    bool m_isReturnButtonVisible;
    bool m_isCollapsible;
    bool m_isAcrylicEnabled;
};

#endif // NAVIGATION_BAR_H
