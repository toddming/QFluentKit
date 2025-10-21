#include "FluentWindow.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QSettings>
#include <QFrame>
#include <QStandardItemModel>
#include <QActionGroup>
#include <QCompleter>
#include <QStyleOption>
#include <QPainter>


#include "QWKWidgets/widgetwindowagent.h"


FluentWindow::FluentWindow(QMainWindow *parent)
    : QMainWindow(parent)
{
    setObjectName("FluentWindow");

    installAgent();

    initUI();
}

FluentWindow::~FluentWindow()
{
}


bool FluentWindow::event(QEvent *event) {
    switch (event->type()) {
    case QEvent::WindowActivate: {
        auto menu = menuWidget();
        if (menu) {
            menu->setProperty("bar-active", true);
            style()->polish(menu);
        }
        break;
    }

    case QEvent::WindowDeactivate: {
        auto menu = menuWidget();
        if (menu) {
            menu->setProperty("bar-active", false);
            style()->polish(menu);
        }
        break;
    }

    default:
        break;
    }
    return QMainWindow::event(event);
}

void FluentWindow::installAgent() {
    if (windowAgent != nullptr)
        return;
    setAttribute(Qt::WA_DontCreateNativeAncestors);

    QWK::WidgetWindowAgent *agent = new QWK::WidgetWindowAgent(this);
    agent->setup(this);

    auto windowBar = new FluentTitleBar(this);
    windowBar->setHostWidget(this);

    agent->setTitleBar(windowBar);
    agent->setHitTestVisible(windowBar->themeButton(), true);
    agent->setHitTestVisible(windowBar->backButton(), true);
    agent->setSystemButton(QWK::WindowAgentBase::Minimize, windowBar->minButton());
    agent->setSystemButton(QWK::WindowAgentBase::Maximize, windowBar->maxButton());
    agent->setSystemButton(QWK::WindowAgentBase::Close, windowBar->closeButton());

    setMenuWidget(windowBar);

    connect(windowBar, &FluentTitleBar::themeRequested, this, [this, windowBar](bool dark){
        windowBar->themeButton()->setChecked(dark);
        loadStyleSheet(!dark);

    });
    connect(windowBar, &FluentTitleBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(windowBar, &FluentTitleBar::maximizeRequested, this, [this](bool max) {
        if (max) {
            showMaximized();
        } else {
            showNormal();
        }
    });
    connect(windowBar, &FluentTitleBar::closeRequested, this, &QWidget::close);
    windowAgent = agent;
}

void FluentWindow::loadStyleSheet(bool dark) {
    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(windowAgent);
    if (agent == nullptr) {
        return;
    }

    agent->setWindowAttribute("dark-mode", dark);
    QFile file(dark ? ":/res/style/dark-style.qss" : ":/res/style/light-style.qss");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Open Style File Error:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    setStyleSheet(content);
    file.close();

    Theme::instance()->setTheme(!dark ? ThemeType::ThemeMode::LIGHT : ThemeType::ThemeMode::DARK);
}

void FluentWindow::initUI()
{

    QWidget *w = new QWidget(this);
    w->setFocusPolicy(Qt::ClickFocus);

    QHBoxLayout *hlay = new QHBoxLayout(w);
    QVBoxLayout *vlay = new QVBoxLayout;

    QFrame *frame = new QFrame(this);
    vlay->addWidget(frame);
    frame->setProperty("card-frame", true);

    QGridLayout *lay = new QGridLayout(frame);

    QStringList names = {"none", "dwm-blur", "acrylic-material", "mica", "mica-alt"};

    QButtonGroup *group = new QButtonGroup(this);
    foreach (QString name, names) {
        RadioButton *radio = new RadioButton(name, this);

        group->addButton(radio);
        lay->addWidget(radio, 0, names.indexOf(name));
    }

    SwitchButton *button = new SwitchButton("Off", this, SwitchButton::IndicatorPosition::Left);
    lay->addWidget(button, 1, 0);


    LineEdit *edit = new LineEdit(this);
    edit->setPlaceholderText("Entry your name");
    edit->setFixedWidth(100);


    lay->addWidget(edit, 1, 1);

    ComboBox* combo2 = new ComboBox(this);
    combo2->addItem("木星上行", Icon::FluentIcon(IconType::FLuentIcon::BRUSH), 1);
    combo2->addItem("星际穿越", Icon::FluentIcon(IconType::FLuentIcon::GAME), 2);
    combo2->addItem("我是传奇", Icon::FluentIcon(IconType::FLuentIcon::LABEL), 3);

    lay->addWidget(combo2);

    PushButton *btn = new PushButton("PushButton", this, IconType::FLuentIcon::HOME);
    lay->addWidget(btn);

    connect(btn, &PushButton::clicked, this, [=](){
        InfoBar::warning("警告", "我的名字是吉良吉影，年龄33岁，家住杜王町东北部别墅区，未婚。我在“龟友百货连锁公司”上班，每天最晚也是八点前回家，不吸烟，酒也是浅尝辄止，晚上十一点上床，保证八个小时的充足睡眠，睡前喝一杯热牛奶，再做二十分钟伸展运动暖身，然后再睡觉，基本可以熟睡到天亮。像婴儿一样不留下疲劳与压力，迎来第二天的早晨，健康检査结果也显示我很健康。我的意思是我是一个随时都想追求平静生活的人，不拘泥于胜负与烦恼，不树立令我夜不能寐的敌人，这就是我对于这个社会的生活态度，我也清楚这就是我的幸福。", Qt::Horizontal, false, 2000, InfoBarType::BarPosition::TOP, this);
    });

    PrimaryPushButton *pbtn = new PrimaryPushButton("PushButton", this);
    connect(pbtn, &PrimaryPushButton::clicked, this, [=](){
        static Loading *load = new Loading(QString(), this);
        load->exec();
    });

    lay->addWidget(pbtn);

    TransparentPushButton *tbtn = new TransparentPushButton("PushButton", this);
    connect(tbtn, &TransparentPushButton::clicked, this, [=](){
        auto box = new MessageDialog("这是一个带有遮罩的对话框",
                                  "一生消えない傷でいいな，絆創膏の様にいつも包んでよ。貴方のそばでわがまま言いたいな，一分一秒刻み貴方を知り，あたしをあげる~", this);
        box->setIsClosableOnMaskClicked(true);
        box->exec();
    });
    lay->addWidget(tbtn);

    QActionGroup *actionGroup = new QActionGroup(this);
    Action *up = new Action(Icon::FluentIcon(IconType::FLuentIcon::UP), "升序");
    up->setCheckable(true);
    Action *down = new Action(Icon::FluentIcon(IconType::FLuentIcon::DOWN), "降序");
    down->setCheckable(true);
    up->setChecked(true);
    actionGroup->addAction(up);
    actionGroup->addAction(down);

    ToggleToolButton *tool = new ToggleToolButton(IconType::FLuentIcon::WIFI, this);
    connect(tool, &ToggleToolButton::clicked, this, [=](){
        CheckableMenu *cm = new CheckableMenu("menu", this, MenuIndicatorType::MenuIndicator::RADIO);
        cm->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::CALENDAR), "创建时间"));
        cm->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::CAMERA), "拍摄时间"));
        cm->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::EDIT), "修改时间"));
        cm->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::FONT), "名称"));
        cm->addSeparator();

        cm->addAction(up);
        cm->addAction(down);
        cm->exec(QCursor::pos());
    });
    lay->addWidget(tool);


    auto menuBtn = new ToolButton(IconType::FLuentIcon::WIFI, this);

    connect(menuBtn, &ToolButton::clicked, this, [=](){
        RoundMenu *am = new RoundMenu("menu", this);
        am->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::COPY), "复制"));
        am->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::CUT), "剪切"));

        RoundMenu *bm = new RoundMenu("添加到", this);
        bm->setIcon(Icon::FluentIcon(IconType::FLuentIcon::ADD));
        bm->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::VIDEO), "视频"));
        bm->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::MUSIC), "音乐"));
        am->addMenu(bm);


        am->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::PASTE), "粘贴"));
        am->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::CANCEL), "撤回"));
        am->addSeparator();
        am->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::SETTING), "设置"));
        am->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::HELP), "帮助"));
        am->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::FEEDBACK), "反馈"));

        auto ac = new Action(Icon::FluentIcon(IconType::FLuentIcon::NONE), "全选");
        // ac->setShortcut(QKeySequence("Ctrl+A"));
        am->addAction(ac);

        am->exec(QCursor::pos());
    });

    lay->addWidget(menuBtn);

    auto tlbtn = new ToggleButton("8", this);
    lay->addWidget(tlbtn);
    connect(tlbtn, &ToggleButton::clicked, this, [=](){
        // static auto mesBox = new MessageBoxBase(this);
        // mesBox->exec();
        m_progressBar->setValue(m_progressBar->value() + 1);
    });

    auto hlbtn = new HyperlinkButton("PushButton", this);
    connect(hlbtn, &HyperlinkButton::clicked, this, [=]() {

        auto first = m_flay->itemAt(0)->widget();
        auto last  = m_flay->itemAt(m_flay->count()-1)->widget();

        first->hide();
        last->show();

        m_flay->removeWidget(first);
        m_flay->addWidget(first);
    });

    lay->addWidget(hlbtn);
    lay->addWidget(new CheckBox("CheckBox", this));



    ComboBox* combo = new ComboBox(this);
    combo->setPlaceholderText("请选择...");
    combo->addItems({"shoko🥰", "西宫硝子😊", "一级棒卡哇伊的硝子酱😘"});
    connect(combo, &ComboBox::currentIndexChanged, [](int index){
        qDebug() << "当前索引：" << index;
    });


    lay->addWidget(combo);

    EditableComboBox *editCombo = new EditableComboBox(this);
    editCombo->addItems({"Star Platinum" , "Crazy Diamond", "Gold Experience", "Sticky Fingers"});
    editCombo->setPlaceholderText("Choose your stand");
    editCombo->setMinimumWidth(210);
    lay->addWidget(editCombo);

    RoundMenu *menu = new RoundMenu("menu", this);
    menu->setItemHeight(36);
    menu->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::SEND), "Send"));
    menu->addAction(new Action(Icon::FluentIcon(IconType::FLuentIcon::EDUCATION), "Exit"));

    DropDownPushButton *dropBtn = new DropDownPushButton("Menu", this, IconType::FLuentIcon::MENU);
    dropBtn->setText("Menu");
    dropBtn->setMenu(menu);

    lay->addWidget(dropBtn);


    RoundMenu *textMenu = new RoundMenu("menu", this);
    textMenu->setItemHeight(36);
    textMenu->addAction(new Action("Send"));
    textMenu->addAction(new Action("Exit"));
    TransparentDropDownPushButton *transBtn = new TransparentDropDownPushButton("Menu", this);
    transBtn->setMenu(textMenu);


    lay->addWidget(transBtn);


    SpinBox *spin = new SpinBox(this);
    lay->addWidget(spin);


    lay->addWidget(new DoubleSpinBox(this));
    lay->addWidget(new DateTimeEdit(this));

    StrongBodyLabel *label = new StrongBodyLabel("简单按钮", this);
    lay->addWidget(label);

    SearchLineEdit *searchEdit = new SearchLineEdit(this);
    lay->addWidget(searchEdit);

    CalendarPicker *picker = new CalendarPicker(this);
    lay->addWidget(picker);

    // 进度条
    m_progressBar = new ProgressBar(this);
    m_progressBar->setValue(50);
    m_progressBar->setCustomBarColor(Theme::instance()->themeColor(), Theme::instance()->themeColor());
    lay->addWidget(m_progressBar);


    auto indeterminateProgressBar = new IndeterminateProgressBar(this);
    indeterminateProgressBar->setValue(50);
    indeterminateProgressBar->setCustomBarColor(Theme::instance()->themeColor(), Theme::instance()->themeColor());
    lay->addWidget(indeterminateProgressBar);

    auto progressRing = new ProgressRing(this);
    progressRing->setValue(50);
    progressRing->setCustomBarColor(Theme::instance()->themeColor(), Theme::instance()->themeColor());
    lay->addWidget(progressRing);


    QCompleter *completer = new QCompleter({"qw", "qe", "qr", "hm", "hw"}, searchEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(5);
    searchEdit->setCompleter(completer);


    TableWidget *table = new TableWidget(this);
    table->setMinimumHeight(100);
    table->verticalHeader()->hide();
    table->setBorderVisible(true);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    vlay->addWidget(table);



    QVector<QStringList> songInfos = {
        {"かばん", "aiko", "かばん", "2004", "5:04"},
        {"爱你", "王心凌", "爱你", "2004", "3:39"},
        {"星のない世界", "aiko", "星のない世界/横顔", "2007", "5:30"},
        {"横顔", "aiko", "星のない世界/横顔", "2007", "5:06"},
        {"秘密", "aiko", "秘密", "2008", "6:27"},
        {"シアワセ", "aiko", "秘密", "2008", "5:25"},
        {"二人", "aiko", "二人", "2008", "5:00"},
        {"スパークル", "RADWIMPS", "君の名は。", "2016", "8:54"},
        {"なんでもないや", "RADWIMPS", "君の名は。", "2016", "3:16"},
        {"前前前世", "RADWIMPS", "人間開花", "2016", "4:35"},
        {"恋をしたのは", "aiko", "恋をしたのは", "2016", "6:02"},
        {"夏バテ", "aiko", "恋をしたのは", "2016", "4:41"},
        {"もっと", "aiko", "もっと", "2016", "4:50"},
        {"問題集", "aiko", "もっと", "2016", "4:18"},
        {"半袖", "aiko", "もっと", "2016", "5:50"},
        {"ひねくれ", "鎖那", "Hush a by little girl", "2017", "3:54"},
        {"シュテルン", "鎖那", "Hush a by little girl", "2017", "3:16"},
        {"愛は勝手", "aiko", "湿った夏の始まり", "2018", "5:31"},
        {"ドライブモード", "aiko", "湿った夏の始まり", "2018", "3:37"},
        {"うん。", "aiko", "湿った夏の始まり", "2018", "5:48"},
        {"キラキラ", "aikoの詩。", "2019", "5:08", "aiko"},
        {"恋のスーパーボール", "aiko", "aikoの詩。", "2019", "4:31"},
        {"磁石", "aiko", "どうしたって伝えられないから", "2021", "4:24"},
        {"食べた愛", "aiko", "食べた愛/あたしたち", "2021", "5:17"},
        {"列車", "aiko", "食べた愛/あたしたち", "2021", "4:18"},
        {"花の塔", "さユり", "花の塔", "2022", "4:35"},
        {"夏恋のライフ", "aiko", "夏恋のライフ", "2022", "5:03"},
        {"あかときリロード", "aiko", "あかときリロード", "2023", "4:04"},
        {"荒れた唇は恋を失くす", "aiko", "今の二人をお互いが見てる", "2023", "4:07"},
        {"ワンツースリー", "aiko", "今の二人をお互いが見てる", "2023", "4:47"}
    };

    // 模拟 Python 中的 songInfos += songInfos（数据翻倍）
    auto originalSize = songInfos.size();
    for (int i = 0; i < originalSize; ++i) {
        songInfos.append(songInfos[i]);
    }

    // 设置表格行数和列数
    int rowCount = songInfos.size();
    int columnCount = 5;
    table->setRowCount(rowCount);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"标题", "歌手", "专辑", "年份", "时长"});

    // 填充数据
    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < columnCount; ++j) {
            table->setItem(i, j, new QTableWidgetItem(songInfos[i][j]));
        }
    }

    TableView *tv = new TableView(this);
    vlay->addWidget(tv);

    QStringList heardList;//表头
    heardList << "接收" << "发送";
    QHeaderView *hearview = new QHeaderView(Qt::Horizontal);
    QStandardItemModel* model = new QStandardItemModel;
    model->setHorizontalHeaderLabels(heardList);
    hearview->setModel(model);
    hearview->setSectionResizeMode(QHeaderView::Stretch);    //先自适应宽度
    hearview->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    tv->setHorizontalHeader(hearview);
    tv->setBorderVisible(true);
    // tv->setBorderRadius(true);


    PagiNation *page = new PagiNation(this);
    page->setAlign(PagiNationType::Alignment::Align_Center);
    page->setPageSize(10);
    page->setTotal(500);
    vlay->addWidget(page);


    QWidget *client = new QWidget(this);
    client->setLayout(vlay);
    vlay->setContentsMargins(30,30,30,30);

    stacked = new StackedWidget(this);
    stacked->addWidget(client);
    StyleSheetManager::instance()->registerWidget(stacked, ThemeType::ThemeStyle::FLUENT_WINDOW);

    QList<QWidget *> wids;
    wids << client;
    for (int i=0; i < 9; i++) {
        // QWidget *w = new QWidget(this);

        auto w = new GalleryInterface(QString("基本输入%1").arg(i+1), "qfluentwidgets.components.widgets", this);

        // 创建样式表源
        auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/gallery_interface.qss");

        // 注册控件
        StyleSheetManager::instance()->registerWidget(styleSource, w);


        w->addExampleCard("按钮", new PushButton("按钮", w), "");


        stacked->addWidget(w);
        wids << w;
    }



    ScrollArea *setArea = new ScrollArea(Qt::Vertical, this);
    QWidget *scrollWidget = new QWidget();
    scrollWidget->setObjectName("scrollWidget");
    setArea->setWidget(scrollWidget);
    setArea->enableTransparentBackground();
    stacked->addWidget(setArea);
    wids.append(setArea);

    ExpandLayout *setLayout = new ExpandLayout(scrollWidget);
    setLayout->setSpacing(28);
    setLayout->setContentsMargins(60, 20, 60, 0);

    SettingCardGroup *aboutGroup = new SettingCardGroup("关于", scrollWidget);

    PrimaryPushSettingCard *feedbackCard = new PrimaryPushSettingCard("Provide feedback",
                                                                      Icon::FluentIcon(IconType::FLuentIcon::FEEDBACK),
                                                                      "Provide feedback",
                                                                      "Help us improve PyQt-Fluent-Widgets by providing feedback",
                                                                      aboutGroup);
    SwitchSettingCard *updateOnStartUpCard = new SwitchSettingCard(Icon::FluentIcon(IconType::FLuentIcon::UPDATE),
                                                                   "Check for updates when the application starts",
                                                                   "The new version will be more stable and have more features",
                                                                   aboutGroup);

    HyperlinkCard *helpCard = new HyperlinkCard("", "Open help page", Icon::FluentIcon(IconType::FLuentIcon::HELP),
                                                "Help",
                                                "Discover new features and learn useful tips about PyQt-Fluent-Widgets");

    ComboBoxSettingCard *languageCard = new ComboBoxSettingCard({"简体中文", "繁體中文", "English", "系统设置"},
                                                                Icon::FluentIcon(IconType::FLuentIcon::LANGUAGE),
                                                                "Language",
                                                                "Set your preferred language for UI",
                                                                aboutGroup);
    OptionsSettingCard *zoomCard = new OptionsSettingCard(Icon::FluentIcon(IconType::FLuentIcon::ZOOM),
                                                          "Interface zoom",
                                                          "Change the size of widgets and fonts",
                                                          QVector<QString>() << "100%" << "125%" << "150%",
                                                          aboutGroup);

    aboutGroup->addSettingCard(feedbackCard);
    aboutGroup->addSettingCard(updateOnStartUpCard);
    aboutGroup->addSettingCard(helpCard);

    aboutGroup->addSettingCard(languageCard);
    aboutGroup->addSettingCard(zoomCard);


    setLayout->addWidget(aboutGroup);
    // setLayout->addWidget(new QLabel(this));

    NavigationPanel *panel = new NavigationPanel(this);
    panel->setMenuButtonVisible(true);
    panel->addItem("1", IconType::FLuentIcon::HOME, "主页", [=](){switchTo(wids[0]);}, true, NavigationType::NavigationItemPosition::TOP);
    panel->addSeparator();
    panel->addItem("2", IconType::FLuentIcon::CHECKBOX, "基本输入", [=](){switchTo(wids[1]);}, true, NavigationType::NavigationItemPosition::SCROLL);
    panel->addItem("3", IconType::FLuentIcon::DATE_TIME, "日期和时间", [=](){}, true, NavigationType::NavigationItemPosition::SCROLL);
    panel->addItem("3-1", IconType::FLuentIcon::DATE_TIME, "日期", [=](){ }, true, NavigationType::NavigationItemPosition::SCROLL, "", "3");
    panel->addItem("3-1-1", IconType::FLuentIcon::DATE_TIME, "AAAA", [=](){ }, true, NavigationType::NavigationItemPosition::SCROLL, "", "3-1");
    panel->addItem("3-1-2", IconType::FLuentIcon::DATE_TIME, "BBBB", [=](){ }, true, NavigationType::NavigationItemPosition::SCROLL, "", "3-1");
    panel->addItem("3-2", IconType::FLuentIcon::DATE_TIME, "时间", [=](){ }, true, NavigationType::NavigationItemPosition::SCROLL, "", "3");

    panel->addItem("4", IconType::FLuentIcon::MESSAGE, "弹出窗口", [=](){switchTo(wids[3]);}, true, NavigationType::NavigationItemPosition::SCROLL);
    panel->addItem("5", IconType::FLuentIcon::LAYOUT, "布局", [=](){switchTo(wids[4]);}, true, NavigationType::NavigationItemPosition::SCROLL);
    panel->addItem("6", IconType::FLuentIcon::PALETTE, "材料", [=](){switchTo(wids[5]);}, true, NavigationType::NavigationItemPosition::SCROLL);
    panel->addItem("7", IconType::FLuentIcon::WIFI, "网络", [=](){switchTo(wids[6]);}, true, NavigationType::NavigationItemPosition::SCROLL);
    panel->addItem("8", IconType::FLuentIcon::SCROLL, "布局", [=](){switchTo(wids[7]);}, true, NavigationType::NavigationItemPosition::SCROLL);
    panel->addItem("9", IconType::FLuentIcon::CHAT, "信息", [=](){switchTo(wids[8]);}, true, NavigationType::NavigationItemPosition::SCROLL);
    panel->addItem("10", IconType::FLuentIcon::EMOJI_TAB_SYMBOLS, "图标", [=](){switchTo(wids[9]);}, true, NavigationType::NavigationItemPosition::SCROLL);

    panel->addSeparator(NavigationType::NavigationItemPosition::BOTTOM);
    panel->addItem("11", IconType::FLuentIcon::SETTING, "设置", [=](){switchTo(wids[10]);}, true, NavigationType::NavigationItemPosition::BOTTOM);



    auto *navBar = new NavigationBar(this);
    navBar->addItem("1", IconType::FLuentIcon::HOME, "主页", nullptr, true, IconType::FLuentIcon::HOME, NavigationType::NavigationItemPosition::TOP);
    navBar->addSeparator();
    navBar->addItem("2", IconType::FLuentIcon::CHECKBOX, "输入", nullptr, true, IconType::FLuentIcon::CHECKBOX, NavigationType::NavigationItemPosition::SCROLL);
    navBar->addItem("3", IconType::FLuentIcon::DATE_TIME, "日期", nullptr, true, IconType::FLuentIcon::DATE_TIME, NavigationType::NavigationItemPosition::SCROLL);
    navBar->addItem("4", IconType::FLuentIcon::MESSAGE, "信息框", nullptr, true, IconType::FLuentIcon::MESSAGE, NavigationType::NavigationItemPosition::SCROLL);
    navBar->addSeparator(NavigationType::NavigationItemPosition::BOTTOM);
    navBar->addItem("5", IconType::FLuentIcon::SETTING, "设置", nullptr, true, IconType::FLuentIcon::SETTING, NavigationType::NavigationItemPosition::BOTTOM);

    hlay->addWidget(navBar, 0);
    navBar->hide();

    hlay->addWidget(panel, 0);

    hlay->addWidget(stacked, 1);
    hlay->setContentsMargins(0, 0, 0, 0);

    connect(group, &QButtonGroup::buttonClicked, this, [=](QAbstractButton *btn) {
        QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(windowAgent);
        if (agent == nullptr)
            return;

        foreach (QString name, names) {
            agent->setWindowAttribute(name, false);
        }
        const QString data = btn->text();

        if (data == QStringLiteral("none")) {
            setProperty("custom-style", false);
        } else if (!data.isEmpty()) {
            agent->setWindowAttribute(data, true);
            setProperty("custom-style", true);
        }
        style()->polish(this);
    });


    auto pivot = new Pivot(this);
    pivot->addItem("1", " 主页", Icon::FluentIcon(IconType::FLuentIcon::HOME));
    pivot->addItem("2", " 订阅", Icon::FluentIcon(IconType::FLuentIcon::BOOK_SHELF));
    pivot->addItem("3", " 历史", Icon::FluentIcon(IconType::FLuentIcon::HISTORY));
    vlay->addWidget(pivot);


    QWidget *flow = new QWidget(this);
    m_flay = new FlowLayout(flow, true);
    m_flay->setContentsMargins(0, 0, 0, 0);
    m_flay->setVerticalSpacing(20);
    m_flay->setHorizontalSpacing(10);

    int nl = 7;
    for (int i=0; i < nl; i++) {
        auto b = new PushButton(QString::number(10000000+i), flow);
        m_flay->addWidget(b);
        b->setHidden(i == nl-1);
        m_btns.append(b);
    }
    vlay->addWidget(flow);

    setCentralWidget(w);

    loadStyleSheet(true);

    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));

    resize(800, 600);
}

void FluentWindow::switchTo(QWidget *w)
{
    stacked->setCurrentWidget(w);
}
