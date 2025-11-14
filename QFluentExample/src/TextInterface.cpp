#include "TextInterface.h"

#include <QCompleter>
#include "QFluent/LineEdit.h"
#include "QFluent/SpinBox.h"
#include "QFluent/TextEdit.h"

TextInterface::TextInterface(QWidget *parent)
    : GalleryInterface("文本", "", parent)
{
    setObjectName("TextInterface");

    auto lineEdit = new LineEdit(this);
    lineEdit->setFixedWidth(230);
    lineEdit->setClearButtonEnabled(true);
    lineEdit->setText("带清空按钮的输入框");
    addExampleCard("带清空按钮的输入框", lineEdit);

    auto passwordLineEdit = new PasswordLineEdit(this);
    passwordLineEdit->setFixedWidth(230);
    passwordLineEdit->setViewPasswordButtonVisible(true);
    passwordLineEdit->setPlaceholderText("请输入密码");
    addExampleCard("密码输入框", passwordLineEdit);

    QStringList items = {
        "Settings", "Profile",
        "Dashboard", "Notifications",
        "Messages", "Calendar",
        "Documents", "Downloads",
        "Search", "Help",
        "Logout", "Preferences",
        "History", "Bookmarks",
        "Themes", "Shortcuts",
        "Backup", "Sync Now",
        "New Tab", "Open File",
        "Save As", "Print Preview",
        "Export PDF", "Import Data",
        "Refresh", "Reload Page",
        "Clear Cache", "Developer Tools",
        "About App", "Check Updates",
        "Send Feedback", "Report Issue"
    };
    auto searchLineEdit = new SearchLineEdit(this);
    searchLineEdit->setFixedWidth(230);
    searchLineEdit->setClearButtonEnabled(true);
    searchLineEdit->setPlaceholderText("请输入内容");
    QCompleter *completer = new QCompleter(items, searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(5);
    searchLineEdit->setCompleter(completer);

    addExampleCard("带补全功能的输入框", searchLineEdit);
    addExampleCard("微调框", new SpinBox(this));
    addExampleCard("浮点数微调框", new DoubleSpinBox(this));
    addExampleCard("时间编辑框", new TimeEdit(this));
    addExampleCard("日期编辑框", new DateEdit(this));
    addExampleCard("日期时间编辑框", new DateTimeEdit(this));

    auto textEdit = new TextEdit(this);
    textEdit->setMarkdown("## 演示小队\n"
                          "* 布局大师 🧱\n"
                          "* 样式达人 🎨\n"
                          "* 逻辑鬼才 💡\n"
                          "* 摸鱼专家 🍵\n"
                          "* 编译守护 🛡️");
    textEdit->setFixedHeight(150);
    addExampleCard("富文本框", textEdit, "", 1);
}


