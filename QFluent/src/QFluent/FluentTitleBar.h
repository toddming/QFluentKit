#ifndef FLUENTTITLEBAR_H
#define FLUENTTITLEBAR_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QPointer>

#include "Property.h"

class QFLUENT_EXPORT FluentTitleBar : public QFrame {
    Q_OBJECT
public:
    explicit FluentTitleBar(QWidget *parent = nullptr);
    ~FluentTitleBar();

public:
    QLabel *titleLabel() const;
    QAbstractButton *iconButton() const;
    QAbstractButton *themeButton() const;
    QAbstractButton *minButton() const;
    QAbstractButton *maxButton() const;
    QAbstractButton *closeButton() const;
    QAbstractButton *backButton() const;

    QWidget *hostWidget() const;
    void setHostWidget(QWidget *w);

Q_SIGNALS:
    void backRequested();
    void themeRequested(bool dark = true);
    void minimizeRequested();
    void maximizeRequested(bool max = false);
    void closeRequested();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    virtual void titleChanged(const QString &text);
    virtual void iconChanged(const QIcon &icon);

private:
    QPushButton *m_minButton;
    QPushButton *m_maxButton;
    QPushButton *m_closeButton;
    QPushButton *m_themeButton;
    QPushButton *m_backButton;
    QPushButton *m_iconButton;
    QLabel *m_titleLabel;
    QPointer<QWidget> m_hostWidget;

    void initWidgets();
};



#endif // FLUENTTITLEBAR_H
