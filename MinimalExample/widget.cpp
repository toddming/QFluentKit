#include "widget.h"
#include "./ui_widget.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QFluent/Theme.h>
#include <QFluent/CheckBox.h>
#include <QFluent/PushButton.h>
#include <QFluent/RadioButton.h>
#include <QFluent/CardWidget.h>

Widget::Widget(QWidget *parent)
    : AcrylicWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setDarkTheme(false);
    Theme::instance()->setTheme(Fluent::ThemeMode::LIGHT);
    setWindowEffect(AcrylicWidget::WindowEffect::Normal);


    static const AcrylicWidget::WindowEffect effects[] = {
        AcrylicWidget::WindowEffect::Normal,
        AcrylicWidget::WindowEffect::DWMBlur,
        AcrylicWidget::WindowEffect::Acrylic,
        AcrylicWidget::WindowEffect::Mica,
        AcrylicWidget::WindowEffect::MicaAlt
    };

    auto cardWidget = new CardWidget(this);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(cardWidget);


    auto vLayout = new QVBoxLayout(cardWidget);
    auto hLayout = new QHBoxLayout();

    auto buttonGroup = new QButtonGroup(this);
    connect(buttonGroup, &QButtonGroup::idClicked, this, [this](int id) {
        setWindowEffect(effects[id]);
    });

    const QString names[] = {
        tr("无效果"), tr("DWM模糊"), tr("亚克力"), tr("Mica"), tr("Mica Alt")
    };

    for (int i = 0; i < std::size(effects); ++i) {
        auto btn = new RadioButton(names[i], this);
        buttonGroup->addButton(btn, i);
        hLayout->addWidget(btn);
    }
    buttonGroup->button(0)->setChecked(true);

    auto darkCheckBox = new CheckBox(tr("深色"), this);

    vLayout->addWidget(new PrimaryPushButton("欢迎加入QQ群交流: 1084320682", this), 0, Qt::AlignHCenter);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(darkCheckBox, 0, Qt::AlignHCenter);

    connect(darkCheckBox, &CheckBox::clicked, this, [this](bool checked) {
        setDarkTheme(checked);
        Theme::instance()->toggleTheme();
    });
}

Widget::~Widget()
{
    delete ui;
}

