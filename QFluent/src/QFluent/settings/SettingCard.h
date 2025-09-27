#ifndef SETTINGCARD_H
#define SETTINGCARD_H

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QPainter>

#include "../IconWidget.h"
#include "../SwitchButton.h"
#include "../PushButton.h"
#include "../ComboBox.h"

class SettingIconWidget : public IconWidget
{
    Q_OBJECT
public:
    using IconWidget::IconWidget;

protected:
    void paintEvent(QPaintEvent *e) override {
        IconWidget::paintEvent(e);
        QPainter painter(this);

        if (!this->isEnabled()) {
            painter.setOpacity(0.36);
        }

    }
};

class SettingCard : public QFrame
{
    Q_OBJECT

public:
    explicit SettingCard(const QIcon &icon, const QString &title,
                         const QString &content = QString(), QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setContent(const QString &content);
    virtual void setValue(const QVariant &value); // 可被子类 override

    void setIconSize(int width, int height);

    QHBoxLayout *hBoxLayout() { return m_hBoxLayout; }
    QVBoxLayout *vBoxLayout() { return m_vBoxLayout; }
    QLabel* titleLabel() {return m_titleLabel; };

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    SettingIconWidget *iconLabel;
    QLabel *m_titleLabel;
    QLabel *contentLabel;
    QHBoxLayout *m_hBoxLayout;
    QVBoxLayout *m_vBoxLayout;
};



class PushSettingCard : public SettingCard
{
    Q_OBJECT

public:
    explicit PushSettingCard(const QString &buttonText,
                             const QIcon &icon,
                             const QString &title,
                             const QString &content = QString(),
                             QWidget *parent = nullptr);

    QPushButton *button() const { return m_button; }

signals:
    void clicked();

private:
    QPushButton *m_button;
};



class PrimaryPushSettingCard : public PushSettingCard
{
    Q_OBJECT

public:
    explicit PrimaryPushSettingCard(const QString &buttonText,
                                    const QIcon &icon,
                                    const QString &title,
                                    const QString &content = QString(),
                                    QWidget *parent = nullptr);
};



class HyperlinkCard :  public SettingCard
{
    Q_OBJECT

public:
    explicit HyperlinkCard(const QString &url,
                           const QString &text,
                             const QIcon &icon,
                             const QString &title,
                             const QString &content = QString(),
                             QWidget *parent = nullptr);
};



class SwitchSettingCard : public SettingCard
{
    Q_OBJECT

public:
    explicit SwitchSettingCard(const QIcon &icon, const QString &title,
                         const QString &content = QString(), QWidget *parent = nullptr);

    void setValue(bool isChecked);

    void setChecked(bool isChecked);

    bool isChecked();

signals:
    void checkedChanged(bool isChecked);

private slots:
    void onCheckedChanged(bool isChecked);

private:
    SwitchButton *m_switchButton;
};




class ComboBoxSettingCard : public SettingCard
{
    Q_OBJECT
public:

    explicit ComboBoxSettingCard(const QStringList &items, const QIcon &icon, const QString &title,
                               const QString &content = QString(), QWidget *parent = nullptr);

    void setValue(int value);

private slots:
    void onCurrentIndexChanged(int);

private:
    ComboBox *m_comboBox;
};




#endif // SETTINGCARD_H
