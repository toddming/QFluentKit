#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>
#include <QIcon>
#include <QPainter>
#include <QStyleOption>
#include <QVariant>
#include <QSize>
#include <QPointer>

#include "../common/Theme.h"
#include "RoundMenu.h"
#include "../common/Animation.h"
#include "../Property.h"

class QFLUENT_EXPORT PushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)

public:
    explicit PushButton(QWidget *parent = nullptr);
    explicit PushButton(const QString &text, QWidget *parent = nullptr,
                        const Icon::IconType iconType = Icon::IconType::NONE);
    explicit PushButton(const QString &fontFamily, QChar iconChar, QWidget *parent = nullptr);

    void setButtonIcon(const Icon::IconType iconType);
    Icon::IconType buttonIcon() const;

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    void initialize();

    bool m_isPressed;
    bool m_isHover;
    Icon::IconType m_iconType;
};




class QFLUENT_EXPORT PrimaryPushButton : public PushButton
{
    Q_OBJECT

public:
    explicit PrimaryPushButton(QWidget *parent = nullptr);
    explicit PrimaryPushButton(const QString &text, QWidget *parent = nullptr,
                               const Icon::IconType iconType = Icon::IconType::NONE);
};


class QFLUENT_EXPORT TransparentPushButton : public PushButton
{
    Q_OBJECT

public:
    explicit TransparentPushButton(QWidget *parent = nullptr);
    explicit TransparentPushButton(const QString &text, QWidget *parent = nullptr,
                               const Icon::IconType iconType = Icon::IconType::NONE);
    explicit TransparentPushButton(const QString &fontFamily, QChar iconChar, QWidget *parent = nullptr);

};




class QFLUENT_EXPORT HyperlinkButton : public PushButton
{
    Q_OBJECT

public:
    explicit HyperlinkButton(QWidget *parent = nullptr);
    explicit HyperlinkButton(const QString &text, QWidget *parent = nullptr,
                               const Icon::IconType iconType = Icon::IconType::NONE);
};


class QFLUENT_EXPORT ToggleButton : public PushButton
{
    Q_OBJECT

public:
    explicit ToggleButton(QWidget *parent = nullptr);
    explicit ToggleButton(const QString &text, QWidget *parent = nullptr,
                             const Icon::IconType iconType = Icon::IconType::NONE);
    explicit ToggleButton(const QString &fontFamily, QChar iconChar, QWidget *parent = nullptr);


private:
    void postInit();

};




class QFLUENT_EXPORT TransparentTogglePushButton : public ToggleButton
{
    Q_OBJECT

public:
    explicit TransparentTogglePushButton(QWidget *parent = nullptr);
    explicit TransparentTogglePushButton(const QString &text, QWidget *parent = nullptr,
                          const Icon::IconType iconType = Icon::IconType::NONE);
    explicit TransparentTogglePushButton(const QString &fontFamily, QChar iconChar, QWidget *parent = nullptr);


};



class QFLUENT_EXPORT DropDownButtonBase : public PushButton
{
    Q_OBJECT
public:
    explicit DropDownButtonBase(QWidget* parent = nullptr);
    explicit DropDownButtonBase(const QString &text, QWidget *parent = nullptr,
                             const Icon::IconType iconType = Icon::IconType::NONE);


    ~DropDownButtonBase();

    void setMenu(RoundMenu* menu);

    RoundMenu* menu() const;

    void showMenu();

    void hideMenu();

protected:
    virtual void drawDropDownIcon(QPainter* painter, const QRectF& rect);


    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPointer<RoundMenu> m_menu;
    TranslateYAnimation* m_arrowAni;

    bool m_isHover = false;
    bool m_isPressed = false;
};





class QFLUENT_EXPORT DropDownPushButton : public DropDownButtonBase
{
    Q_OBJECT
public:
    explicit DropDownPushButton(QWidget *parent = nullptr);
    explicit DropDownPushButton(const QString &text, QWidget *parent = nullptr,
                                const Icon::IconType iconType = Icon::IconType::NONE);


protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

};





class QFLUENT_EXPORT TransparentDropDownPushButton : public DropDownPushButton
{
    Q_OBJECT
public:
    explicit TransparentDropDownPushButton(QWidget *parent = nullptr);
    explicit TransparentDropDownPushButton(const QString &text, QWidget *parent = nullptr,
                                const Icon::IconType iconType = Icon::IconType::NONE);
};

#endif // PUSHBUTTON_H
