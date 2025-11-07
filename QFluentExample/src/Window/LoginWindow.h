#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QObject>

#include "QFluent/Loading.h"
#include "QFluent/LineEdit.h"
#include "QFluent/CheckBox.h"
#include "QFluent/PushButton.h"
#include "QFluent/FluentWidget.h"

class LoginWidget : public FluentWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(bool autoLogin = false);

signals:
    void loginComplete(int code);

private slots:
    void onBtnLoginClicked();

private:
    void initUI();

    LineEdit *m_editUser;
    LineEdit *m_editPass;
    CheckBox *m_autoLogin;
    PrimaryPushButton *m_btnLogin;

    void loading(bool load);
    void userLogin();
};

#endif // LOGINWIDGET_H
