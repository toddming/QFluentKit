#include "SettingInterface.h"

#include <QLabel>
#include "Theme.h"
#include "StyleSheet.h"
#include "FluentIcon.h"
#include "QFluent/settings/SettingCard.h"
#include "QFluent/settings/SettingCardGroup.h"
#include "QFluent/settings/OptionsSettingCard.h"

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

    auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/setting_interface.qss");
    StyleSheetManager::instance()->registerWidget(styleSource, this);

    SettingCardGroup *personalGroup = new SettingCardGroup("个性化", m_scrollWidget);
    SettingCardGroup *aboutGroup = new SettingCardGroup("关于", m_scrollWidget);


    SwitchSettingCard *updateOnStartUpCard = new SwitchSettingCard(FluentIcon(Fluent::IconType::UPDATE).qicon(),
                                                                   "Check for updates when the application starts",
                                                                   "The new version will be more stable and have more features",
                                                                   aboutGroup);



    ComboBoxSettingCard *languageCard = new ComboBoxSettingCard({"简体中文", "繁體中文", "English", "系统设置"},
                                                                FluentIcon(Fluent::IconType::LANGUAGE).qicon(),
                                                                "Language",
                                                                "Set your preferred language for UI",
                                                                aboutGroup);

    OptionsSettingCard *effectCard = new OptionsSettingCard(FluentIcon(Fluent::IconType::ZOOM).qicon(),
                                                            "窗口效果",
                                                            "改变窗口的显示效果",
                                                            QVector<QString>() << "none" << "dwm-blur" << "acrylic-material" << "mica" << "miac-alt",
                                                            aboutGroup);

    OptionsSettingCard *themeCard = new OptionsSettingCard(FluentIcon(Fluent::IconType::BRUSH).qicon(),
                                                           "应用主题",
                                                           "调整你的应用的外观",
                                                           QVector<QString>() << "深色" << "浅色",
                                                           aboutGroup);

    PrimaryPushSettingCard *feedbackCard = new PrimaryPushSettingCard("联系作者",
                                                                      FluentIcon(Fluent::IconType::FEEDBACK).qicon(),
                                                                      "联系作者",
                                                                      "QQ:1912229135",
                                                                      aboutGroup);

    HyperlinkCard *helpCard = new HyperlinkCard("", "打开帮助页面", FluentIcon(Fluent::IconType::HELP).qicon(),
                                                "帮助",
                                                "https://github.com/toddming/QFluentExample");

    personalGroup->addSettingCard(updateOnStartUpCard);
    personalGroup->addSettingCard(languageCard);
    personalGroup->addSettingCard(themeCard);
    personalGroup->addSettingCard(effectCard);

    aboutGroup->addSettingCard(feedbackCard);
    aboutGroup->addSettingCard(helpCard);


    m_expandLayout->addWidget(personalGroup);
    m_expandLayout->addWidget(aboutGroup);

    connect(themeCard, &OptionsSettingCard::optionChanged, this, [=]
            (int index, const QString& text) {
        Q_UNUSED(text);
        Theme::instance()->setTheme(index == 0 ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);
        ConfigManager::instance().setValue("Window/theme", index);
    });

    connect(effectCard, &OptionsSettingCard::optionChanged, this, [=]
            (int index, const QString& text) {
        auto main = qobject_cast<MainWindow*>(this->window());
        main->setWindowEffect(static_cast<Fluent::WindowEffect>(index));
        ConfigManager::instance().setValue("Window/effect", text);
    });

    const QString theme  = ConfigManager::instance().getValue("Window/theme").toInt() == 0 ? "深色" : "浅色";
    const QString value  = ConfigManager::instance().isWin11() ? "mica" : "none";
    const QString effect = ConfigManager::instance().getValue("Window/effect", value).toString();
    themeCard->setValue(theme);
    effectCard->setValue(effect);

    QStringList modes; modes << "none" << "dwm-blur" << "acrylic-material" << "mica" << "miac-alt";
    int var = modes.indexOf(effect);
    if (var >= 0) {
        auto main = qobject_cast<MainWindow*>(this->window());
        main->setWindowEffect(static_cast<Fluent::WindowEffect>(var));
    }
}
