#ifndef NAVIGATION_WIDGET_H
#define NAVIGATION_WIDGET_H

#include <QWidget>
#include <QScrollArea>

#include "FluentGlobal.h"
#include "FluentIcon.h"

// NavigationWidget
class AvatarWidget;
class QVBoxLayout;
class QPropertyAnimation;
class NavigationWidget : public QWidget {
    Q_OBJECT
public:
    explicit NavigationWidget(bool isSelectable, QWidget* parent = nullptr);
    virtual void insertChild(int index, NavigationWidget* child);

    virtual void setCompacted(bool isCompacted);
    void setSelected(bool isSelected);
    void setLightTextColor(const QColor& color);
    void setDarkTextColor(const QColor& color);
    void setTextColor(const QColor& light, const QColor& dark);
    QColor textColor();

    void click();

    void setExpandWidth(int width);

signals:
    void clicked(bool triggeredByUser);
    void selectedChanged(bool selected);

protected:
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent* e) override;
#else
    void enterEvent(QEvent* e) override;
#endif
    void leaveEvent(QEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    NavigationWidget* treeParent();
    void setTreeParent(NavigationWidget* p);

    int expandWidth();

private:
    NavigationWidget* m_treeParent;
    QColor lightTextColor;
    QColor darkTextColor;
    int m_expandWidth;

};


// NavigationPushButton
class NavigationPushButton : public NavigationWidget {
    Q_OBJECT
public:
    NavigationPushButton(const QString &text, const FluentIconBase &icon, bool isSelectable, QWidget* parent = nullptr);

    QString text() const;
    void setText(const QString& text);

    void setFluentIcon(const FluentIconBase &icon);
    FluentIconBase* fluentIcon() const;

    void setIndicatorColor(const QColor& light, const QColor& dark);

protected:
    void paintEvent(QPaintEvent* e) override;
    virtual QMargins _margins();
    virtual bool _canDrawIndicator();

private:
    std::unique_ptr<FluentIconBase> m_fluentIcon;
    QString m_text;
    QColor lightIndicatorColor;
    QColor darkIndicatorColor;
};


// NavigationToolButton
class NavigationToolButton : public NavigationPushButton {
    Q_OBJECT
public:
    NavigationToolButton(const FluentIconBase &icon, QWidget* parent = nullptr);
    void setCompacted(bool isCompacted) override;
};

// NavigationSeparator
class NavigationSeparator : public NavigationWidget {
    Q_OBJECT
public:
    explicit NavigationSeparator(QWidget* parent = nullptr);
    void setCompacted(bool isCompacted) override;

protected:
    void paintEvent(QPaintEvent* e) override;
};

class NavigationTreeWidget; // 前向声明

// NavigationTreeItem
class NavigationTreeItem : public NavigationPushButton {
    Q_OBJECT
    Q_PROPERTY(float arrowAngle READ getArrowAngle WRITE setArrowAngle)
public:
    NavigationTreeItem(const QString &text, const FluentIconBase &icon, bool isSelectable, NavigationTreeWidget* parent = nullptr);

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
private:
    float _arrowAngle;
    QPropertyAnimation* rotateAni;
};

// NavigationTreeWidgetBase
class NavigationTreeWidgetBase : public NavigationWidget {
    Q_OBJECT
public:
    explicit NavigationTreeWidgetBase(bool isSelectable, QWidget* parent = nullptr)
        : NavigationWidget(isSelectable, parent) {}

    virtual void addChild(NavigationWidget* child) = 0;
    virtual void removeChild(NavigationWidget* child) = 0;
    virtual bool isRoot() = 0;
    virtual bool isLeaf() = 0;
    virtual void setExpanded(bool isExpanded) = 0;
    virtual std::vector<NavigationWidget*> childItems() = 0;
};

// NavigationTreeWidget
class NavigationTreeWidget : public NavigationTreeWidgetBase {
    Q_OBJECT
public:
    NavigationTreeWidget(const QString &text, const FluentIconBase &icon, bool isSelectable, QWidget* parent = nullptr);

    void addChild(NavigationWidget* child) override;
    void insertChild(int index, NavigationWidget* child) override;
    void removeChild(NavigationWidget* child) override;
    bool isRoot() override;
    bool isLeaf() override;
    void setExpanded(bool isExpanded) override;
    void setExpanded(bool isExpanded, bool ani);

    QString text() const;
    void setText(const QString& text);
    void setFluentIcon(const FluentIconBase &icon);
    FluentIconBase* fluentIcon() const;
    void setIndicatorColor(const QColor& light, const QColor& dark);
    void setFont(const QFont& font);
    NavigationTreeWidget* clone();
    int suitableWidth();
    void setSelected(bool isSelected);
    void setCompacted(bool isCompacted) override;
    std::vector<NavigationWidget*> childItems() override;

    std::vector<NavigationTreeWidget*> treeChildren();
    NavigationTreeItem* itemWidget();

    void setExpandWidth(int width);

signals:
    void expanded();

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;

private slots:
    void _onClicked(bool triggerByUser, bool clickArrow);

private:
    void __initWidget();

    bool isExpanded;
    std::unique_ptr<FluentIconBase> m_fluentIcon;

    QVBoxLayout* vBoxLayout;
    QPropertyAnimation* expandAni;

    std::vector<NavigationTreeWidget*> m_treeChildren;
    NavigationTreeItem* m_itemWidget;

};


// NavigationFlyoutMenu
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


// NavigationAvatarWidget
class QFLUENT_EXPORT NavigationAvatarWidget : public NavigationWidget
{
    Q_OBJECT

public:
    explicit NavigationAvatarWidget(const QString &name,
                                    const QVariant &avatar = {},
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
