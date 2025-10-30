#include "SettingInterface.h"

#include <QLabel>
#include "StyleSheet.h"

#include "FluentIcon.h"
#include "QFluent/settings/SettingCard.h"
#include "QFluent/settings/SettingCardGroup.h"
#include "QFluent/settings/OptionsSettingCard.h"

#include "MainWindow.h"

SettingInterface::SettingInterface(QWidget *parent)
    : ScrollArea(parent)
{
    m_scrollWidget = new QWidget();
    m_expandLayout = new ExpandLayout(m_scrollWidget);
    setObjectName("settingInterface");
    m_scrollWidget->setObjectName("scrollWidget");

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ScrollBar* floatVScrollBar = new ScrollBar(this->verticalScrollBar(), this);
    floatVScrollBar->setIsAnimation(true);


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




    SettingCardGroup *aboutGroup = new SettingCardGroup("关于", m_scrollWidget);

    PrimaryPushSettingCard *feedbackCard = new PrimaryPushSettingCard("Provide feedback",
                                                                      FluentIcon(FluentIconType::FEEDBACK).qicon(),
                                                                      "Provide feedback",
                                                                      "Help us improve PyQt-Fluent-Widgets by providing feedback",
                                                                      aboutGroup);
    SwitchSettingCard *updateOnStartUpCard = new SwitchSettingCard(FluentIcon(FluentIconType::UPDATE).qicon(),
                                                                   "Check for updates when the application starts",
                                                                   "The new version will be more stable and have more features",
                                                                   aboutGroup);

    HyperlinkCard *helpCard = new HyperlinkCard("", "Open help page", FluentIcon(FluentIconType::HELP).qicon(),
                                                "Help",
                                                "Discover new features and learn useful tips about PyQt-Fluent-Widgets");

    ComboBoxSettingCard *languageCard = new ComboBoxSettingCard({"简体中文", "繁體中文", "English", "系统设置"},
                                                                FluentIcon(FluentIconType::LANGUAGE).qicon(),
                                                                "Language",
                                                                "Set your preferred language for UI",
                                                                aboutGroup);
    OptionsSettingCard *effectCard = new OptionsSettingCard(FluentIcon(FluentIconType::ZOOM).qicon(),
                                                          "窗口效果",
                                                          "改变窗口的显示效果",
                                                          QVector<QString>() << "none" << "dwm-blur" << "acrylic-material" << "mica" << "miac-alt",
                                                          aboutGroup);

    aboutGroup->addSettingCard(feedbackCard);
    aboutGroup->addSettingCard(updateOnStartUpCard);
    aboutGroup->addSettingCard(helpCard);
    aboutGroup->addSettingCard(languageCard);
    aboutGroup->addSettingCard(effectCard);

    m_expandLayout->addWidget(aboutGroup);

    connect(effectCard, &OptionsSettingCard::optionChanged, this, [=]
            (int index, const QString& text)
    {
        Q_UNUSED(text);
        auto main = qobject_cast<MainWindow*>(this->parent()->parent()->parent()->parent());
        main->setWindowDisplayMode(static_cast<ApplicationType::WindowDisplayMode>(index));
    });
}
