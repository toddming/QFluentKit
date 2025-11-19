#ifndef EXPAND_SETTING_CARD_H
#define EXPAND_SETTING_CARD_H

#include <QScrollArea>
#include <QAbstractButton>

#include "FluentGlobal.h"
#include "SettingCard.h"

class QPropertyAnimation;
class ExpandButton : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(float angle READ getAngle WRITE setAngle)

public:
    explicit ExpandButton(QWidget *parent = nullptr);

    void setExpand(bool isExpand);
    float getAngle() const;
    void setAngle(float angle);

    void setHover(bool isHover);
    void setPressed(bool isPressed);


protected:
    void paintEvent(QPaintEvent *e) override;
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *e) override;
#else
    void enterEvent(QEvent *e) override;
#endif
    void leaveEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    void onClicked();

    float m_angle;
    bool m_isHover;
    bool m_isPressed;
    QPropertyAnimation *m_rotateAni;
};

class SpaceWidget : public QWidget {
    Q_OBJECT

public:
    explicit SpaceWidget(QWidget *parent = nullptr);
};

class HeaderSettingCard : public SettingCard {
    Q_OBJECT

public:
    explicit HeaderSettingCard(const QIcon &icon, const QString &title, const QString &content = QString(), QWidget *parent = nullptr);

    void addWidget(QWidget *widget);
    ExpandButton *expandButton() const { return m_expandButton; }


protected:
    void paintEvent(QPaintEvent *e) override;
    bool eventFilter(QObject *obj, QEvent *e) override;

private:
    ExpandButton *m_expandButton;

};

class ExpandBorderWidget : public QWidget {
    Q_OBJECT

public:
    explicit ExpandBorderWidget(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
};

class QFLUENT_EXPORT ExpandSettingCard : public QScrollArea {
    Q_OBJECT

public:
    explicit ExpandSettingCard(const QIcon &icon, const QString &title, const QString &content = QString(), QWidget *parent = nullptr);

    void addWidget(QWidget *widget);
    void setExpand(bool isExpand);
    void toggleExpand();
    void setValue(const QVariant &value); // Assuming QVariant for generality

    bool isExpand() const { return m_isExpand; }

    HeaderSettingCard *card() const { return m_card; }

    QVBoxLayout *viewLayout() const { return m_viewLayout; }
    SpaceWidget *spaceWidget() const { return m_spaceWidget; }
    QFrame *view() const { return m_view; }


protected:
    void wheelEvent(QWheelEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    void initWidget();
    void onExpandValueChanged(const QVariant &value);
    void adjustViewSize();

    QFrame *scrollWidget;
    QVBoxLayout *scrollLayout;
    ExpandBorderWidget *borderWidget;
    QPropertyAnimation *expandAni;
    bool m_isExpand;
    HeaderSettingCard *m_card;
    QVBoxLayout *m_viewLayout;
    SpaceWidget *m_spaceWidget;
    QFrame *m_view;
};

class GroupSeparator : public QWidget {
    Q_OBJECT

public:
    explicit GroupSeparator(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;
};

class ExpandGroupSettingCard : public ExpandSettingCard {
    Q_OBJECT

public:
    explicit ExpandGroupSettingCard(const QIcon &icon, const QString &title, const QString &content = QString(), QWidget *parent = nullptr);

    void addGroupWidget(QWidget *widget);
    void removeGroupWidget(QWidget *widget);

private:
    void adjustViewSize();

    QList<QWidget *> widgets;
};

class QFLUENT_EXPORT SimpleExpandGroupSettingCard : public ExpandGroupSettingCard {
    Q_OBJECT

public:
    using ExpandGroupSettingCard::ExpandGroupSettingCard;

private:
    void adjustViewSize();
};

#endif // EXPAND_SETTING_CARD_H
