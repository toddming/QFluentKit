#pragma once
#include <QWidget>

#include "FluentIcon.h"
#include "FluentGlobal.h"
#include "QFluent/ScrollArea.h"

class Flyout;
class AvatarWidget;
class QVBoxLayout;
class QPropertyAnimation;
class ScaleSlideAnimation;
class QParallelAnimationGroup;
class QFLUENT_EXPORT NavigationWidget : public QWidget {
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
class QFLUENT_EXPORT NavigationPushButton : public NavigationWidget {
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
class QFLUENT_EXPORT NavigationToolButton : public NavigationPushButton {
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
class QFLUENT_EXPORT NavigationTreeItem : public NavigationPushButton {
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
class QFLUENT_EXPORT NavigationTreeWidgetBase : public NavigationWidget {
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
class QFLUENT_EXPORT NavigationTreeWidget : public NavigationTreeWidgetBase {
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
class QFLUENT_EXPORT NavigationFlyoutMenu : public ScrollArea {
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

    QString name() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_name;
    AvatarWidget *m_avatar;
};


// NavigationUserCard 类
class QFLUENT_EXPORT NavigationUserCard : public NavigationAvatarWidget
{
    Q_OBJECT
    Q_PROPERTY(float textOpacity READ textOpacity WRITE setTextOpacity)
    Q_PROPERTY(QColor subtitleColor READ subtitleColor WRITE setSubtitleColor)

public:
    explicit NavigationUserCard(QWidget *parent = nullptr);

    // Avatar icon and color
    void setAvatarIcon(const QIcon &icon);
    void setAvatarBackgroundColor(const QColor &light, const QColor &dark);

    // Title and subtitle
    QString title() const;
    void setTitle(const QString &title);

    QString subtitle() const;
    void setSubtitle(const QString &subtitle);

    // Font sizes
    void setTitleFontSize(int size);
    void setSubtitleFontSize(int size);

    // Animation
    void setAnimationDuration(int duration);

    // Override setCompacted to add animation
    void setCompacted(bool isCompacted) override;

    // Properties
    float textOpacity() const;
    void setTextOpacity(float opacity);

    QColor subtitleColor() const;
    void setSubtitleColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void _drawText(QPainter &painter);
    void _updateAvatarPosition();

    // Text properties
    QString m_title;
    QString m_subtitle;
    int m_titleSize;
    int m_subtitleSize;
    QColor m_subtitleColor;

    // Animation properties
    float m_textOpacity;
    int m_animationDuration;
    QParallelAnimationGroup *m_animationGroup;
    QPropertyAnimation *m_radiusAni;
    QPropertyAnimation *m_opacityAni;
};


// NavigationIndicator 类
class QFLUENT_EXPORT NavigationIndicator : public QWidget
{
    Q_OBJECT

public:
    explicit NavigationIndicator(QWidget *parent = nullptr);

    // Start animation from startRect to endRect
    void startAnimation(const QRectF &startRect, const QRectF &endRect, bool useCrossFade = false);

    // Stop animation
    void stopAnimation();

    // Set indicator color
    void setIndicatorColor(const QColor &light, const QColor &dark);

signals:
    void aniFinished();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor m_lightColor;
    QColor m_darkColor;
    ScaleSlideAnimation *m_scaleSlideAni;
};


// NavigationItemHeader 类
class QFLUENT_EXPORT NavigationItemHeader : public NavigationWidget
{
    Q_OBJECT
    Q_PROPERTY(int maximumHeight READ maximumHeight WRITE setMaximumHeight)

public:
    explicit NavigationItemHeader(const QString &text, QWidget *parent = nullptr);

    // Text
    QString text() const;
    void setText(const QString &text);

    // Override setCompacted for animation
    void setCompacted(bool isCompacted) override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *e) override;
#else
    void enterEvent(QEvent *e) override;
#endif
    void leaveEvent(QEvent *e) override;

private slots:
    void _onCollapseFinished();
    void _onHeightChanged(const QVariant &value);

private:
    QString m_text;
    int m_targetHeight;
    QPropertyAnimation *m_heightAni;
};
