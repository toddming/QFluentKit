#include "widget.h"
#include "./ui_widget.h"

#include <QButtonGroup>
#include <QClipboard>
#include <QApplication>
#include <QStringListModel>

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

    // ==================== 不使用 Model（便利 API）====================

    // ComboBox - addItem / addItems
    ui->combobox->addItems({"C++", "Python", "Java", "Rust", "Go"});
    ui->combobox->setCurrentIndex(0);
    connect(ui->combobox, &ComboBox::currentIndexChanged, this, [](int index) {
        qDebug("ComboBox selected: %d", index);
    });

    // EditableComboBox - addItem / insertSeparator
    ui->editcombobox->addItems({"Apple", "Banana", "Cherry"});
    ui->editcombobox->insertSeparator(1);
    ui->editcombobox->setCurrentIndex(2);
    connect(ui->editcombobox, &EditableComboBox::currentIndexChanged, this, [](int index) {
        qDebug("EditableComboBox selected: %d", index);
    });

    // MultiViewComboBox - addItem / setItemSelected
    ui->multiviewcombobox->addItems({"Red", "Green", "Blue", "Yellow"});
    ui->multiviewcombobox->setItemSelected(0, true);
    ui->multiviewcombobox->setItemSelected(2, true);
    connect(ui->multiviewcombobox, &MultiViewComboBox::selectionChanged, this, [this]() {
        qDebug() << "MultiViewComboBox selected:" << ui->multiviewcombobox->selectedTexts();
    });

    // ==================== 使用 Model ====================

    // ComboBox + QStringListModel
    auto stringModel = new QStringListModel({"C++", "Python", "Java", "Rust", "Go"}, this);
    ui->combobox_model->setModel(stringModel);
    ui->combobox_model->setCurrentIndex(0);
    connect(ui->combobox_model, &ComboBox::currentIndexChanged, this, [](int index) {
        qDebug("ComboBox(model) selected: %d", index);
    });

    // EditableComboBox + QStringListModel
    auto editModel = new QStringListModel({"Apple", "Banana", "Cherry"}, this);
    ui->editcombobox_model->setModel(editModel);
    ui->editcombobox_model->setCurrentIndex(0);
    connect(ui->editcombobox_model, &EditableComboBox::currentIndexChanged, this, [](int index) {
        qDebug("EditableComboBox(model) selected: %d", index);
    });

    // MultiViewComboBox + QStringListModel
    auto multiModel = new QStringListModel({"Red", "Green", "Blue", "Yellow"}, this);
    ui->multiviewcombobox_model->setModel(multiModel);
    ui->multiviewcombobox_model->setItemSelected(1, true);
    connect(ui->multiviewcombobox_model, &MultiViewComboBox::selectionChanged, this, [this]() {
        qDebug() << "MultiViewComboBox(model) selected:" << ui->multiviewcombobox_model->selectedTexts();
    });
}

Widget::~Widget()
{
    delete ui;
}
