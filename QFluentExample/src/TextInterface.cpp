#include "TextInterface.h"

#include <QCompleter>
#include "QFluent/LineEdit.h"
#include "QFluent/SpinBox.h"
#include "QFluent/TextEdit.h"

TextInterface::TextInterface(QWidget *parent)
    : GalleryInterface("文本", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("TextInterface");

    auto lineEdit = new LineEdit(this);
    lineEdit->setFixedWidth(230);
    lineEdit->setClearButtonEnabled(true);
    lineEdit->setText("ko no dio da!");
    addExampleCard("带清空按钮的输入框", lineEdit);

    auto passwordLineEdit = new PasswordLineEdit(this);
    passwordLineEdit->setFixedWidth(230);
    passwordLineEdit->setViewPasswordButtonVisible(true);
    passwordLineEdit->setPlaceholderText("请输入密码");
    addExampleCard("密码输入框", passwordLineEdit);

    QStringList items = {
            "Star Platinum", "Hierophant Green",
            "Made in Haven", "King Crimson",
            "Silver Chariot", "Crazy diamond",
            "Metallica", "Another One Bites The Dust",
            "Heaven's Door", "Killer Queen",
            "The Grateful Dead", "Stone Free",
            "The World", "Sticky Fingers",
            "Ozone Baby", "Love Love Deluxe",
            "Hermit Purple", "Gold Experience",
            "King Nothing", "Paper Moon King",
            "Scary Monster", "Mandom",
            "20th Century Boy", "Tusk Act 4",
            "Ball Breaker", "Sex Pistols",
            "D4C • Love Train", "Born This Way",
            "SOFT & WET", "Paisley Park",
            "Wonder of U", "Walking Heart",
            "Cream Starter", "November Rain",
            "Smooth Operators", "The Matte Kudasai"
};
    auto searchLineEdit = new SearchLineEdit(this);
    searchLineEdit->setFixedWidth(230);
    searchLineEdit->setClearButtonEnabled(true);
    searchLineEdit->setPlaceholderText("Type a stand name");
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
    textEdit->setMarkdown("## Steel Ball Run \n * Johnny Joestar 🦄 \n * Gyro Zeppeli 🐴 ");
    textEdit->setFixedHeight(150);
    addExampleCard("富文本框", textEdit, "", 1);
}


