#include "SettingInterface.h"

#include <QLabel>
#include "Theme.h"
#include "StyleSheet.h"
#include "FluentIcon.h"
#include "QFluent/Settings/SettingCard.h"
#include "QFluent/Settings/SettingCardGroup.h"
#include "QFluent/Settings/OptionsSettingCard.h"

#include "MainWindow.h"
#include "ConfigManager.h"

SettingInterface::SettingInterface(QWidget *parent)
    : ScrollArea(parent)
{
    m_scrollWidget = new QWidget();
    m_expandLayout = new ExpandLayout(m_scrollWidget);
    setObjectName("settingInterface");
    m_scrollWidget->setObjectName("scrollWidget");

    setViewportMargins(0, 80, 0, 0);
    setWidget(m_scrollWidget);
    setWidgetResizable(true);

    m_expandLayout->setSpacing(28);
    m_expandLayout->setContentsMargins(36, 10, 36, 0);

    auto settingLabel = new QLabel("设置", this);
    settingLabel->setObjectName("settingLabel");
    settingLabel->move(36, 30);

    StyleSheet::registerWidget(this, ":/res/style/{theme}/setting_interface.qss");

    SettingCardGroup *personalGroup = new SettingCardGroup("个性化", m_scrollWidget);
    SettingCardGroup *aboutGroup = new SettingCardGroup("关于", m_scrollWidget);


    ComboBoxSettingCard *themeCard = new ComboBoxSettingCard({"自动", "浅色", "深色"},
                                                                FluentIcon(Fluent::IconType::BRUSH).qicon(),
                                                                "应用主题",
                                                                "调整你的应用的外观",
                                                                aboutGroup);

    PrimaryPushSettingCard *colorCard = new PrimaryPushSettingCard("选择颜色",
                                                                   FluentIcon(Fluent::IconType::PALETTE).qicon(),
                                                                   "主题色",
                                                                   "调整你的应用的主题色",
                                                                   aboutGroup);

    OptionsSettingCard *effectCard = new OptionsSettingCard(FluentIcon(Fluent::IconType::ZOOM).qicon(),
                                                            "窗口效果",
                                                            "改变窗口的显示效果",
                                                            QVector<QString>() << "none" << "dwm-blur" << "acrylic-material" << "mica" << "miac-alt",
                                                            aboutGroup);

    HyperlinkCard *helpCard = new HyperlinkCard("https://github.com/toddming/QFluentKit",
                                                "项目地址", FluentIcon(Fluent::IconType::GITHUB).qicon(),
                                                "GitHub",
                                                "https://github.com/toddming/QFluentKit");

    personalGroup->addSettingCard(colorCard);
    personalGroup->addSettingCard(themeCard);
    personalGroup->addSettingCard(effectCard);

    aboutGroup->addSettingCard(helpCard);

    m_expandLayout->addWidget(personalGroup);
    m_expandLayout->addWidget(aboutGroup);

    connect(colorCard, &PrimaryPushSettingCard::clicked, this, [this](){
        if (m_colorDialog == nullptr) {
            m_colorDialog = new ColorDialog(Theme::themeColor(), "选择颜色", this->window());
            connect(m_colorDialog, &ColorDialog::colorChanged, this, [=](const QColor &color) {
                Theme::setThemeColor(color);
                ConfigManager::instance().setValue("Window/color", color.name());
            });
        }
        m_colorDialog->exec();
    });

    connect(effectCard, &OptionsSettingCard::optionChanged, this, [=]
            (int index, const QString& text) {
        auto main = qobject_cast<MainWindow*>(this->window());
        main->setWindowEffect(static_cast<WindowEffect>(index));
        ConfigManager::instance().setValue("Window/effect", text);
    });

    int themeMode  = ConfigManager::instance().getValue("Window/theme").toInt();
    const QString value  = ConfigManager::instance().isWin11() ? "mica" : "none";
    const QString effect = ConfigManager::instance().getValue("Window/effect", value).toString();
    themeCard->setValue(themeMode);
    effectCard->setValue(effect);

    QStringList modes; modes << "none" << "dwm-blur" << "acrylic-material" << "mica" << "miac-alt";
    int var = modes.indexOf(effect);
    if (var >= 0) {
        auto main = qobject_cast<MainWindow*>(this->window());
        main->setWindowEffect(static_cast<WindowEffect>(var));
    }

    connect(themeCard, &ComboBoxSettingCard::currentIndexChanged, this, [=]
            (int index) {
        Theme::setThemeMode(static_cast<Fluent::ThemeMode>(index));
        ConfigManager::instance().setValue("Window/theme", index);
    });
}
