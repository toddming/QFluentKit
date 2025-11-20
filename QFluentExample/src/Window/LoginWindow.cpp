#include "LoginWindow.h"

#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "Theme.h"
#include "StyleSheet.h"
#include "QFluent/InfoBar.h"

LoginWidget::LoginWidget(bool autoLogin)
{
    Fluent::WindowButtonHints hints = Fluent::WindowButtonHint::Minimize |
            Fluent::WindowButtonHint::Close;
    setWindowButtonHints(hints);
    // setWindowEffect(ApplicationType::Mica);

    setContentsMargins(0, 0, 0, 0);
    setFixedSize(380, 300);

    initUI();

    if (autoLogin)
        QTimer::singleShot(500, this, [=](){ userLogin(); });
}

void LoginWidget::initUI()
{
    auto w = new QWidget(this);
    setCentralWidget(w);

    auto c = new QWidget(w);
    c->setFixedWidth(220);
    auto hLayout = new QHBoxLayout(w);
    hLayout->addStretch();
    hLayout->addWidget(c);
    hLayout->addStretch();
    hLayout->setContentsMargins(0, 0, 0, 0);
    auto vLayout = new QVBoxLayout(c);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(8);
    auto imageLabel = new QLabel(this);
    imageLabel->setFixedSize(67, 67);
    imageLabel->setScaledContents(true);
    imageLabel->setPixmap(QPixmap(":res/example.png"));

    m_editUser = new LineEdit(this);
    m_editUser->setClearButtonEnabled(true);
    m_editUser->setPlaceholderText("请输入用户名");
    m_editUser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_editPass = new LineEdit(this);
    m_editPass->setClearButtonEnabled(true);
    m_editPass->setPlaceholderText("请输入密码");
    m_editPass->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_editPass->setEchoMode(QLineEdit::Password);

    m_autoLogin = new CheckBox("自动登录", this);

    m_btnLogin = new PrimaryPushButton("登  录", this);
    m_btnLogin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    vLayout->addWidget(imageLabel, 0, Qt::AlignCenter);
    vLayout->addSpacing(5);
    vLayout->addWidget(m_editUser);
    vLayout->addWidget(m_editPass);
    vLayout->addWidget(m_autoLogin, 0, Qt::AlignLeft);
    vLayout->addWidget(m_btnLogin);

    vLayout->addStretch();

    connect(m_editPass, &LineEdit::returnPressed, this, &LoginWidget::onBtnLoginClicked);
    connect(m_btnLogin, &PrimaryPushButton::clicked, this, &LoginWidget::onBtnLoginClicked);
}

void LoginWidget::loading(bool load)
{
    static auto _loading = new Loading(QString(), this);
    if (load)
        _loading->show();
    else
        _loading->reject();
}

void LoginWidget::onBtnLoginClicked()
{
    userLogin();
}

void LoginWidget::userLogin()
{
    static const QRegularExpression s_whitespaceRegex(QStringLiteral("\\s"));
    QString user = m_editUser->text().remove(s_whitespaceRegex);
    QString pass = m_editPass->text().remove(s_whitespaceRegex);

    if (user.isEmpty()) {
        InfoBar::warning("警告", "用户名不能为空", Qt::Horizontal, false, 2000, Fluent::MessagePosition::TOP, this);
        return;
    }
    if (pass.isEmpty()) {
        InfoBar::warning("警告", "密码不能为空", Qt::Horizontal, false, 2000, Fluent::MessagePosition::TOP, this);
        return;
    }

    loading(true);

    QTimer::singleShot(3000, this, [=](){
        loading(false);
        InfoBar::success("提示", "登录成功", Qt::Horizontal, false, 2000, Fluent::MessagePosition::TOP, this);
    });
}



