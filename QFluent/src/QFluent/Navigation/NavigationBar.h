#pragma once

#include <QWidget>
#include <QPropertyAnimation>

#include "FluentGlobal.h"
#include "FluentIcon.h"
#include "NavigationWidget.h"
#include "NavigationPanel.h"


// IconSlideAnimation 类
class IconSlideAnimation : public QPropertyAnimation {
    Q_OBJECT
    Q_PROPERTY(float offset READ offset WRITE setOffset)
public:
    explicit IconSlideAnimation(QWidget* parent = nullptr);
    float offset() const;
    void setOffset(float value);
    void slideDown();
    void slideUp();

protected:
    QVariant animateValue(const QVariant& startValue, const QVariant& endValue, float progress);

private:
    float m_offset;
    float m_maxOffset;
};

// NavigationBarPushButton 类
class NavigationBarPushButton : public NavigationPushButton {
    Q_OBJECT
public:
    explicit NavigationBarPushButton(const QString& text, const QIcon& icon,
                                     Fluent::IconType iconType = Fluent::IconType::NONE,
                                     bool isSelectable = true, QWidget* parent = nullptr);

    void setSelectedColor(const QColor& light, const QColor& dark);
    void setSelectedTextVisible(bool isVisible);
    void setSelected(bool isSelected);
    IconSlideAnimation* iconAni() { return m_iconAni; }

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    void drawBackground(QPainter& painter);
    void drawIcon(QPainter& painter);
    void drawText(QPainter& painter);

private:
    IconSlideAnimation *m_iconAni;
    Fluent::IconType m_iconType{Fluent::IconType::NONE};

    bool m_isSelectedTextVisible;
    QColor m_lightSelectedColor;
    QColor m_darkSelectedColor;
};

// NavigationBar 类
class QFLUENT_EXPORT NavigationBar : public QWidget {
    Q_OBJECT

public:
    explicit NavigationBar(QWidget* parent = nullptr);
    ~NavigationBar() override;

    // 公共方法
    NavigationWidget* widget(const QString& routeKey);
    void addItem(const QString& routeKey, const QIcon& icon, const QString& text,
                 const std::function<void()>& onClick = nullptr, bool selectable = true,
                 NavigationPanel::ItemPosition position = NavigationPanel::ItemPosition::TOP);

    void addWidget(const QString& routeKey, NavigationWidget* widget,
                   const std::function<void()>& onClick = nullptr,
                   NavigationPanel::ItemPosition position = NavigationPanel::ItemPosition::TOP);

    void insertItem(int index, const QString& routeKey, const QIcon& icon, const QString& text,
                   const std::function<void()>& onClick = nullptr, bool selectable = true,
                   NavigationPanel::ItemPosition position = NavigationPanel::ItemPosition::TOP);

    void insertWidget(int index, const QString& routeKey, NavigationWidget* widget,
                     const std::function<void()>& onClick = nullptr,
                     NavigationPanel::ItemPosition position = NavigationPanel::ItemPosition::TOP);

    void addSeparator(NavigationPanel::ItemPosition position = NavigationPanel::ItemPosition::TOP);

    void insertSeparator(int index, NavigationPanel::ItemPosition position = NavigationPanel::ItemPosition::TOP);

    void removeWidget(const QString& routeKey);
    void setCurrentItem(const QString& routeKey);
    void setFont(const QFont& font);
    void setSelectedTextVisible(bool isVisible);
    void setSelectedColor(const QColor& light, const QColor& dark);
    QList<NavigationBarPushButton*> buttons() const;

signals:
    void displayModeChanged(NavigationPanel::DisplayMode mode);

protected:
    void paintEvent(QPaintEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    void initWidget();
    void initLayout();
    void registerWidget(const QString& routeKey, NavigationWidget* widget, const std::function<void()>& onClick);
    void insertWidgetToLayout(int index, NavigationWidget* widget, NavigationPanel::ItemPosition position);
    void onWidgetClicked();
    void onExpandAniFinished();
    void setWidgetCompacted(bool isCompacted);

private:
    ScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;
    QVBoxLayout* m_vBoxLayout;
    QVBoxLayout* m_topLayout;
    QVBoxLayout* m_bottomLayout;
    QVBoxLayout* m_scrollLayout;
    QMap<QString, NavigationItem> m_items;
    QPropertyAnimation* m_expandAni;
    int m_expandWidth;
    bool m_isMinimalEnabled;
    NavigationPanel::DisplayMode m_displayMode;
    QColor m_lightSelectedColor;
    QColor m_darkSelectedColor;
    bool m_isMenuButtonVisible;
    bool m_isReturnButtonVisible;
    bool m_isCollapsible;
    bool m_isAcrylicEnabled;
};

