#pragma once
#include <QWidget>
#include <memory>

#include "FluentIcon.h"
#include "FluentGlobal.h"
#include "QFluent/ScrollArea.h"

class Flyout;
class AvatarWidget;
class QVBoxLayout;
class QVariantAnimation;
class QPropertyAnimation;
class ScaleSlideAnimation;
class QParallelAnimationGroup;

class QFLUENT_EXPORT NavigationWidget : public QWidget {
    Q_OBJECT
public:
    explicit NavigationWidget(bool isSelectable, QWidget* parent = nullptr);
    ~NavigationWidget() override = default;

    virtual void insertChild(int index, NavigationWidget* child);

    virtual void setCompacted(bool isCompacted);
    void setSelected(bool isSelected);
    void setLightTextColor(const QColor& color);
    void setDarkTextColor(const QColor& color);
    void setTextColor(const QColor& light, const QColor& dark);
    QColor textColor() const;

    void click();

    void setExpandWidth(int width);
    int expandWidth() const;

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

    NavigationWidget* treeParent() const;
    void setTreeParent(NavigationWidget* p);

private:
    NavigationWidget* m_treeParent{nullptr};
    QColor m_lightTextColor;
    QColor m_darkTextColor;
    int m_expandWidth{160};
};


// NavigationPushButton
class QFLUENT_EXPORT NavigationPushButton : public NavigationWidget {
    Q_OBJECT
public:
    NavigationPushButton(const QString &text, const FluentIconBase &icon,
                        bool isSelectable, QWidget* parent = nullptr);
    ~NavigationPushButton() override = default;

    QString text() const;
    void setText(const QString& text);

    void setFluentIcon(const FluentIconBase &icon);
    FluentIconBase* fluentIcon() const;

    void setIndicatorColor(const QColor& light, const QColor& dark);

protected:
    void paintEvent(QPaintEvent* e) override;
    virtual QMargins margins() const;
    virtual bool canDrawIndicator() const;

private:
    std::unique_ptr<FluentIconBase> m_fluentIcon;
    QString m_text;
    QColor m_lightIndicatorColor;
    QColor m_darkIndicatorColor;
};


// NavigationToolButton
class QFLUENT_EXPORT NavigationToolButton : public NavigationPushButton {
    Q_OBJECT
public:
    NavigationToolButton(const FluentIconBase &icon, QWidget* parent = nullptr);
    void setCompacted(bool isCompacted) override;
};


// NavigationSeparator
class QFLUENT_EXPORT NavigationSeparator : public NavigationWidget {
    Q_OBJECT
public:
    explicit NavigationSeparator(QWidget* parent = nullptr);
    void setCompacted(bool isCompacted) override;

protected:
    void paintEvent(QPaintEvent* e) override;
};


class NavigationTreeWidget;

// NavigationTreeItem
class QFLUENT_EXPORT NavigationTreeItem : public NavigationPushButton {
    Q_OBJECT
    Q_PROPERTY(float arrowAngle READ arrowAngle WRITE setArrowAngle)
public:
    NavigationTreeItem(const QString &text, const FluentIconBase &icon,
                      bool isSelectable, NavigationTreeWidget* parent = nullptr);
    ~NavigationTreeItem() override = default;

    void setExpanded(bool isExpanded);
    float arrowAngle() const;
    void setArrowAngle(float angle);

    bool canDrawIndicator() const override;
    QMargins margins() const override;

signals:
    void itemClicked(bool triggeredByUser, bool clickArrow);

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    float m_arrowAngle{0.0f};
    QPropertyAnimation* m_rotateAnimation{nullptr};
};


// NavigationTreeWidgetBase
class QFLUENT_EXPORT NavigationTreeWidgetBase : public NavigationWidget {
    Q_OBJECT
public:
    explicit NavigationTreeWidgetBase(bool isSelectable, QWidget* parent = nullptr)
        : NavigationWidget(isSelectable, parent) {}
    ~NavigationTreeWidgetBase() override = default;

    virtual void addChild(NavigationWidget* child) = 0;
    virtual void removeChild(NavigationWidget* child) = 0;
    virtual bool isRoot() const = 0;
    virtual bool isLeaf() const = 0;
    virtual void setExpanded(bool isExpanded) = 0;
    virtual std::vector<NavigationWidget*> childItems() const = 0;
};


// NavigationTreeWidget
class QFLUENT_EXPORT NavigationTreeWidget : public NavigationTreeWidgetBase {
    Q_OBJECT
public:
    NavigationTreeWidget(const QString &text, const FluentIconBase &icon,
                        bool isSelectable, QWidget* parent = nullptr);
    ~NavigationTreeWidget() override = default;

    void addChild(NavigationWidget* child) override;
    void insertChild(int index, NavigationWidget* child) override;
    void removeChild(NavigationWidget* child) override;

    bool isRoot() const override;
    bool isLeaf() const override;
    void setExpanded(bool isExpanded) override;
    void setExpanded(bool isExpanded, bool animated);

    QString text() const;
    void setText(const QString& text);
    void setFluentIcon(const FluentIconBase &icon);
    FluentIconBase* fluentIcon() const;
    void setIndicatorColor(const QColor& light, const QColor& dark);
    void setFont(const QFont& font);

    NavigationTreeWidget* clone() const;
    int suitableWidth() const;
    void setSelected(bool isSelected);
    void setCompacted(bool isCompacted) override;
    void setExpandWidth(int width);

    std::vector<NavigationWidget*> childItems() const override;
    std::vector<NavigationTreeWidget*> treeChildren() const;
    NavigationTreeItem* itemWidget() const;

signals:
    void expanded();

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;

private slots:
    void onItemClicked(bool triggerByUser, bool clickArrow);

private:
    void initWidget();

    bool m_isExpanded{false};
    std::unique_ptr<FluentIconBase> m_fluentIcon;

    QVBoxLayout* m_vBoxLayout{nullptr};
    QPropertyAnimation* m_expandAnimation{nullptr};

    std::vector<NavigationTreeWidget*> m_treeChildren;
    NavigationTreeItem* m_itemWidget{nullptr};
};


// NavigationFlyoutMenu
class QFLUENT_EXPORT NavigationFlyoutMenu : public ScrollArea {
    Q_OBJECT
public:
    NavigationFlyoutMenu(NavigationTreeWidget* tree, QWidget* parent = nullptr);
    ~NavigationFlyoutMenu() override = default;

signals:
    void expanded();

private:
    void initNode(NavigationTreeWidget* root);
    void adjustViewSize(bool emitSignal = true);
    int suitableWidth() const;
    std::vector<NavigationTreeWidget*> visibleTreeNodes() const;

    QWidget* m_view{nullptr};
    NavigationTreeWidget* m_treeWidget{nullptr};
    std::vector<NavigationTreeWidget*> m_treeChildren;
    QVBoxLayout* m_vBoxLayout{nullptr};
};


// NavigationAvatarWidget
class QFLUENT_EXPORT NavigationAvatarWidget : public NavigationWidget {
    Q_OBJECT
public:
    explicit NavigationAvatarWidget(const QString &name,
                                    const QVariant &avatar = {},
                                    QWidget *parent = nullptr);
    ~NavigationAvatarWidget() override = default;

    void setName(const QString &name);
    void setAvatar(const QVariant &avatar);
    QString name() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_name;
    AvatarWidget *m_avatar{nullptr};
};


// NavigationUserCard
class QFLUENT_EXPORT NavigationUserCard : public NavigationAvatarWidget {
    Q_OBJECT
    Q_PROPERTY(float textOpacity READ textOpacity WRITE setTextOpacity)
    Q_PROPERTY(QColor subtitleColor READ subtitleColor WRITE setSubtitleColor)

public:
    explicit NavigationUserCard(QWidget *parent = nullptr);
    ~NavigationUserCard() override = default;

    void setAvatarIcon(const QIcon &icon);
    void setAvatarBackgroundColor(const QColor &light, const QColor &dark);

    QString title() const;
    void setTitle(const QString &title);

    QString subtitle() const;
    void setSubtitle(const QString &subtitle);

    void setTitleFontSize(int size);
    void setSubtitleFontSize(int size);

    void setAnimationDuration(int duration);

    void setCompacted(bool isCompacted) override;

    float textOpacity() const;
    void setTextOpacity(float opacity);

    QColor subtitleColor() const;
    void setSubtitleColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawText(QPainter &painter);

    QString m_title;
    QString m_subtitle;
    int m_titleSize{14};
    int m_subtitleSize{12};
    QColor m_subtitleColor;

    float m_textOpacity{1.0f};
    int m_animationDuration{185};
    QParallelAnimationGroup *m_animationGroup{nullptr};
    QPropertyAnimation *m_radiusAnimation{nullptr};
    QPropertyAnimation *m_posAnimation{nullptr};
    QVariantAnimation *m_sizeAnimation{nullptr};
};


// NavigationIndicator
class QFLUENT_EXPORT NavigationIndicator : public QWidget {
    Q_OBJECT
public:
    explicit NavigationIndicator(QWidget *parent = nullptr);
    ~NavigationIndicator() override = default;

    void startAnimation(const QRectF &startRect, const QRectF &endRect,
                       bool useCrossFade = false);
    void stopAnimation();

    void setIndicatorColor(const QColor &light, const QColor &dark);

signals:
    void animationFinished();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor m_lightColor;
    QColor m_darkColor;
    ScaleSlideAnimation *m_scaleSlideAnimation{nullptr};
};


// NavigationItemHeader
class QFLUENT_EXPORT NavigationItemHeader : public NavigationWidget {
    Q_OBJECT
    Q_PROPERTY(int maximumHeight READ maximumHeight WRITE setMaximumHeight)

public:
    explicit NavigationItemHeader(const QString &text, QWidget *parent = nullptr);
    ~NavigationItemHeader() override = default;

    QString text() const;
    void setText(const QString &text);

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
    void onCollapseFinished();
    void onHeightChanged(const QVariant &value);

private:
    QString m_text;
    int m_targetHeight{30};
    QPropertyAnimation *m_heightAnimation{nullptr};
};
