#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPointer>
#include <QPushButton>
#include "Define.h"

class RoundMenu;
class TranslateYAnimation;
class QFLUENT_EXPORT PushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit PushButton(QWidget *parent = nullptr);
    explicit PushButton(const QString &text, QWidget *parent = nullptr,
                        const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);
    explicit PushButton(const QString &fontFamily, QChar iconChar, QWidget *parent = nullptr);

    void setButtonIcon(const FluentIconType::IconType iconType);
    FluentIconType::IconType buttonIcon() const;

    bool isPressed() { return m_isPressed; }
    bool isHover() { return m_isHover; }

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    virtual void drawIcon(QPainter* painter, const QRectF& rect);

private:
    void initialize();

    bool m_isPressed;
    bool m_isHover;
    FluentIconType::IconType m_iconType;
};




class QFLUENT_EXPORT PrimaryPushButton : public PushButton
{
    Q_OBJECT

public:
    explicit PrimaryPushButton(QWidget *parent = nullptr);
    explicit PrimaryPushButton(const QString &text, QWidget *parent = nullptr,
                               const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);
};


class QFLUENT_EXPORT TransparentPushButton : public PushButton
{
    Q_OBJECT

public:
    explicit TransparentPushButton(QWidget *parent = nullptr);
    explicit TransparentPushButton(const QString &text, QWidget *parent = nullptr,
                                   const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);
    explicit TransparentPushButton(const QString &fontFamily, QChar iconChar, QWidget *parent = nullptr);

};




class QFLUENT_EXPORT HyperlinkButton : public PushButton
{
    Q_OBJECT

public:
    explicit HyperlinkButton(QWidget *parent = nullptr);
    explicit HyperlinkButton(const QString &text, QWidget *parent = nullptr,
                             const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);

protected:
    void drawIcon(QPainter* painter, const QRectF& rect) override;

};


class QFLUENT_EXPORT ToggleButton : public PushButton
{
    Q_OBJECT

public:
    explicit ToggleButton(QWidget *parent = nullptr);
    explicit ToggleButton(const QString &text, QWidget *parent = nullptr,
                          const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);
    explicit ToggleButton(const QString &fontFamily, QChar iconChar, QWidget *parent = nullptr);

protected:
    void drawIcon(QPainter* painter, const QRectF& rect) override;

private:
    void postInit();

};




class QFLUENT_EXPORT TransparentTogglePushButton : public ToggleButton
{
    Q_OBJECT

public:
    explicit TransparentTogglePushButton(QWidget *parent = nullptr);
    explicit TransparentTogglePushButton(const QString &text, QWidget *parent = nullptr,
                                         const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);
    explicit TransparentTogglePushButton(const QString &fontFamily, QChar iconChar, QWidget *parent = nullptr);


};



class QFLUENT_EXPORT DropDownButtonBase : public PushButton
{
    Q_OBJECT
public:
    explicit DropDownButtonBase(QWidget* parent = nullptr);
    explicit DropDownButtonBase(const QString &text, QWidget *parent = nullptr,
                                const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);


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
                                const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);


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
                                           const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);
};


class QFLUENT_EXPORT PillPushButton : public ToggleButton
{
    Q_OBJECT
public:
    explicit PillPushButton(QWidget *parent = nullptr);
    explicit PillPushButton(const QString &text, QWidget *parent = nullptr,
                            const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);

protected:
    void paintEvent(QPaintEvent* event) override;


};



class QFLUENT_EXPORT PrimaryDropDownPushButton : public DropDownButtonBase
{
  Q_OBJECT
public:
    explicit PrimaryDropDownPushButton(QWidget *parent = nullptr);
    explicit PrimaryDropDownPushButton(const QString &text, QWidget *parent = nullptr,
                                const FluentIconType::IconType iconType = FluentIconType::IconType::NONE);

protected:
    void paintEvent(QPaintEvent* event) override;
    void drawDropDownIcon(QPainter* painter, const QRectF& rect) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void drawIcon(QPainter* painter, const QRectF& rect) override;
};




#endif // PUSHBUTTON_H
