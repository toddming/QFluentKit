#ifndef NAVIGATION_WIDGET_H
#define NAVIGATION_WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QEvent>
#include <QIcon>
#include <QColor>
#include <QMargins>
#include <QFont>
#include <QPoint>
#include <QPixmap>
#include <QImage>
#include <vector>

#include "AvatarWidget.h"
#include "Icon.h"

// 自定义模块占位符，原Python代码中的实现需要额外提供
// #include "common/config.h"       // 包含 isDarkTheme() 函数
// #include "common/style_sheet.h"  // 包含 themeColor() 函数
// #include "common/icon.h"         // 包含 drawIcon() 和 toQIcon() 函数
// #include "common/color.h"        // 包含 autoFallbackThemeColor() 函数
// #include "common/font.h"         // 包含 setFont() 函数
// #include "widgets/scroll_area.h" // 自定义滚动区域
// #include "widgets/label.h"       // 包含 AvatarWidget
// #include "widgets/info_badge.h"  // 包含 InfoBadgeManager 和 InfoBadgePosition



class NavigationWidget : public QWidget {
    Q_OBJECT
public:
    explicit NavigationWidget(bool isSelectable, QWidget* parent = nullptr);

    virtual void setCompacted(bool isCompacted);
    void setSelected(bool isSelected);
    void setLightTextColor(const QColor& color);
    void setDarkTextColor(const QColor& color);
    void setTextColor(const QColor& light, const QColor& dark);
    QColor textColor();

    void click();

    void setExpandWidth(int width);

    int nodeDepth;
    bool isCompacted;
    bool isSelected;
    bool isPressed;
    bool isEnter;
    bool isSelectable;


signals:
    void clicked(bool triggeredByUser);
    void selectedChanged(bool selected);

protected:
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    NavigationWidget* treeParent;

    QColor lightTextColor;
    QColor darkTextColor;
    int m_expandWidth{160};

};

class NavigationPushButton : public NavigationWidget {
    Q_OBJECT
public:
    NavigationPushButton(IconType::FLuentIcon icon, const QString& text, bool isSelectable, QWidget* parent = nullptr);

    QString text() const;
    void setText(const QString& text);
    QIcon icon() const;
    void setIcon(IconType::FLuentIcon icon);
    void setIndicatorColor(const QColor& light, const QColor& dark);

protected:
    void paintEvent(QPaintEvent* e) override;
    virtual QMargins _margins();
    virtual bool _canDrawIndicator();

    IconType::FLuentIcon m_icon;
    QString m_text;
    QColor lightIndicatorColor;
    QColor darkIndicatorColor;
};

class NavigationToolButton : public NavigationPushButton {
    Q_OBJECT
public:
    NavigationToolButton(IconType::FLuentIcon icon, QWidget* parent = nullptr);
    void setCompacted(bool isCompacted) override;
};

class NavigationSeparator : public NavigationWidget {
    Q_OBJECT
public:
    explicit NavigationSeparator(QWidget* parent = nullptr);
    void setCompacted(bool isCompacted) override;

protected:
    void paintEvent(QPaintEvent* e) override;
};

class NavigationTreeWidget; // 前向声明

class NavigationTreeItem : public NavigationPushButton {
    Q_OBJECT
    Q_PROPERTY(float arrowAngle READ getArrowAngle WRITE setArrowAngle)
public:
    NavigationTreeItem(IconType::FLuentIcon icon, const QString& text, bool isSelectable, NavigationTreeWidget* parent = nullptr);

    void setExpanded(bool isExpanded);
    float getArrowAngle() const;
    void setArrowAngle(float angle);
    bool _canDrawIndicator() override;
    QMargins _margins() override;


signals:
    void itemClicked(bool triggeredByUser, bool clickArrow);

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

    float _arrowAngle;
    QPropertyAnimation* rotateAni;
};

class NavigationTreeWidgetBase : public NavigationWidget {
    Q_OBJECT
public:
    explicit NavigationTreeWidgetBase(bool isSelectable, QWidget* parent = nullptr)
        : NavigationWidget(isSelectable, parent) {}

    virtual void addChild(NavigationWidget* child) = 0;
    virtual void insertChild(int index, NavigationWidget* child) = 0;
    virtual void removeChild(NavigationWidget* child) = 0;
    virtual bool isRoot() = 0;
    virtual bool isLeaf() = 0;
    virtual void setExpanded(bool isExpanded) = 0;
    virtual std::vector<NavigationWidget*> childItems() = 0;
};

class NavigationTreeWidget : public NavigationTreeWidgetBase {
    Q_OBJECT
public:
    NavigationTreeWidget(IconType::FLuentIcon icon, const QString& text, bool isSelectable, QWidget* parent = nullptr);

    void addChild(NavigationWidget* child) override;
    void insertChild(int index, NavigationWidget* child) override;
    void removeChild(NavigationWidget* child) override;
    bool isRoot() override;
    bool isLeaf() override;
    void setExpanded(bool isExpanded) override;
    void setExpanded(bool isExpanded, bool ani);

    QString text() const;
    QIcon icon() const;
    void setText(const QString& text);
    void setIcon(IconType::FLuentIcon icon);
    void setIndicatorColor(const QColor& light, const QColor& dark);
    void setFont(const QFont& font);
    NavigationTreeWidget* clone();
    int suitableWidth();
    void setSelected(bool isSelected);
    void setCompacted(bool isCompacted) override;
    std::vector<NavigationWidget*> childItems() override;
    std::vector<NavigationTreeWidget*> treeChildren;
    NavigationTreeItem* itemWidget;


signals:
    void expanded();

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;

private slots:
    void _onClicked(bool triggerByUser, bool clickArrow);

private:
    void __initWidget();

    bool isExpanded;
    IconType::FLuentIcon m_icon;

    QVBoxLayout* vBoxLayout;
    QPropertyAnimation* expandAni;
};




class NavigationFlyoutMenu : public QScrollArea {
    Q_OBJECT
public:
    NavigationFlyoutMenu(NavigationTreeWidget* tree, QWidget* parent = nullptr);

signals:
    void expanded();

private:
    void _initNode(NavigationTreeWidget* root);
    void _adjustViewSize(bool emitSignal = true);
    int _suitableWidth();
    std::vector<NavigationTreeWidget*> visibleTreeNodes();

    QWidget* view;
    NavigationTreeWidget* treeWidget;
    std::vector<NavigationTreeWidget*> treeChildren;
    QVBoxLayout* vBoxLayout;
};





class NavigationAvatarWidget : public NavigationWidget
{
    Q_OBJECT

public:
    explicit NavigationAvatarWidget(const QString &name,
                                    const QVariant &avatar = QVariant(),
                                    QWidget *parent = nullptr);

    void setName(const QString &name);
    void setAvatar(const QVariant &avatar);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_name;
    AvatarWidget *m_avatar;
};





#endif // NAVIGATION_WIDGET_H
