#ifndef NAVIGATION_BAR_H
#define NAVIGATION_BAR_H

#include <QWidget>
#include <QPropertyAnimation>

#include "FluentGlobal.h"
#include "FluentIcon.h"
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
    explicit NavigationBarPushButton(const FluentIconBase& icon, const QString& text,
                                     bool isSelectable = true, QWidget* parent = nullptr);

    void setSelectedColor(const QColor& light, const QColor& dark);
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
    IconSlideAnimation *m_iconAni;
    std::unique_ptr<FluentIconBase> m_fluentIcon;

    bool _isSelectedTextVisible;
    QColor lightSelectedColor;
    QColor darkSelectedColor;
};

// NavigationBar 类
class QFLUENT_EXPORT NavigationBar : public QWidget {
    Q_OBJECT

public:
    explicit NavigationBar(QWidget* parent = nullptr);
    ~NavigationBar() override;

    // 公共方法
    NavigationWidget* widget(const QString& routeKey);
    void addItem(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                 const std::function<void()>& onClick = nullptr, bool selectable = true,
                 Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void addWidget(const QString& routeKey, NavigationWidget* widget,
                   const std::function<void()>& onClick = nullptr,
                   Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void insertItem(int index, const QString& routeKey, const FluentIconBase& icon, const QString& text,
                   const std::function<void()>& onClick = nullptr, bool selectable = true,
                   Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void insertWidget(int index, const QString& routeKey, NavigationWidget* widget,
                     const std::function<void()>& onClick = nullptr,
                     Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void addSeparator(Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void insertSeparator(int index, Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP);

    void removeWidget(const QString& routeKey);
    void setCurrentItem(const QString& routeKey);
    void setFont(const QFont& font);
    void setSelectedTextVisible(bool isVisible);
    void setSelectedColor(const QColor& light, const QColor& dark);
    QList<NavigationBarPushButton*> buttons() const;

signals:
    void displayModeChanged(Fluent::NavigationDisplayMode mode);

protected:
    void paintEvent(QPaintEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    void __initWidget();
    void __initLayout();
    void _registerWidget(const QString& routeKey, NavigationWidget* widget, const std::function<void()>& onClick);
    void _insertWidgetToLayout(int index, NavigationWidget* widget, Fluent::NavigationItemPosition position);
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
    Fluent::NavigationDisplayMode m_displayMode;
    QColor m_lightSelectedColor;
    QColor m_darkSelectedColor;
    bool m_isMenuButtonVisible;
    bool m_isReturnButtonVisible;
    bool m_isCollapsible;
    bool m_isAcrylicEnabled;
};

#endif // NAVIGATION_BAR_H
