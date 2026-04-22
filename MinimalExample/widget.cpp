#include "widget.h"
#include "./ui_widget.h"

#include <QButtonGroup>
#include <QClipboard>
#include <QApplication>

#include <QFluent/Theme.h>
#include <QFluent/InfoBar.h>

Widget::Widget(QWidget *parent)
    : AcrylicWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setDarkTheme(false);
    Theme::setThemeMode(Fluent::ThemeMode::LIGHT);
    setWindowEffect(AcrylicWidget::WindowEffect::Normal);

    // 设置 RadioButton 组
    auto buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->radioNormal, 0);
    buttonGroup->addButton(ui->radioDWMBlur, 1);
    buttonGroup->addButton(ui->radioAcrylic, 2);
    buttonGroup->addButton(ui->radioMica, 3);
    buttonGroup->addButton(ui->radioMicaAlt, 4);

    static const AcrylicWidget::WindowEffect effects[] = {
        AcrylicWidget::WindowEffect::Normal,
        AcrylicWidget::WindowEffect::DWMBlur,
        AcrylicWidget::WindowEffect::Acrylic,
        AcrylicWidget::WindowEffect::Mica,
        AcrylicWidget::WindowEffect::MicaAlt
    };

    connect(buttonGroup, &QButtonGroup::idClicked, this, [this](int id) {
        setWindowEffect(effects[id]);
    });

    // 复制按钮
    connect(ui->pushButton, &PrimaryPushButton::clicked, this, [this](){
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText("1084320682");
        InfoBar::success("", tr("已复制到剪贴板"),
                         Qt::Horizontal, true, 2000, InfoBar::Position::TOP, this);
    });

    // 深色主题切换
    connect(ui->darkCheckBox, &CheckBox::clicked, this, [this](bool checked) {
        setDarkTheme(checked);
        Theme::toggleTheme();
    });
}

Widget::~Widget()
{
    delete ui;
}
