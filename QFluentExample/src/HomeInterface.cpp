#include "HomeInterface.h"
#include "Card/LinkCard.h"
#include "Card/SampleCard.h"
#include "FluentIcon.h"
#include "Theme.h"
#include "StyleSheet.h"

BannerWidget::BannerWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupLinks();
}

void BannerWidget::setupUI()
{
    setFixedHeight(336);

    m_vBoxLayout = new QVBoxLayout(this);
    m_galleryLabel = new QLabel("QFluent Gallery", this);
    m_banner = QPixmap(":/res/header.png");
    m_linkCardView = new LinkCardView(this);
    m_linkCardView->setMinimumHeight(300);
    m_galleryLabel->setObjectName("galleryLabel");

    m_vBoxLayout->setSpacing(0);
    m_vBoxLayout->setContentsMargins(0, 20, 0, 0);
    m_vBoxLayout->addWidget(m_galleryLabel);
    m_vBoxLayout->addWidget(m_linkCardView, 1, Qt::AlignBottom);
    m_vBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void BannerWidget::setupLinks()
{
    m_linkCardView->addCard(
                FluentIcon(":/res/example.png").icon(),
                tr("Getting started"),
                tr("Get started with QFluent and explore detailed documentation."),
                ""
                );

    m_linkCardView->addCard(
                FluentIcon(Fluent::IconType::GITHUB).qicon(),
                tr("QFluent on GitHub"),
                tr("Explore the QFluent source code zand repository."),
                ""
                );

    m_linkCardView->addCard(
                FluentIcon(Fluent::IconType::CODE).qicon(),
                tr("Code samples"),
                tr("Find samples that demonstrate specific tasks, features and APIs."),
                ""
                );

    m_linkCardView->addCard(
                FluentIcon(Fluent::IconType::UPDATE).qicon(),
                tr("Partner Center"),
                tr("Upload your app to the Store."),
                ""
                );
}

void BannerWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    int w = width();
    int h = height() + 2;
    path.addRoundedRect(QRectF(0, 0, w, h), 10, 10);
    path.addRect(QRectF(0, h-50, 50, 50));
    path.addRect(QRectF(w-50, 0, 50, 50));
    path.addRect(QRectF(w-50, h-50, 50, 50));
    path = path.simplified();

    // 初始化线性渐变效果
    QLinearGradient gradient(0, 0, 0, h);

    // 绘制背景颜色
    if (!Theme::instance()->isDarkTheme()) {
        gradient.setColorAt(0, QColor(243, 243, 243, 0));
        gradient.setColorAt(1, QColor(247, 249, 252, 255));
    } else {
        gradient.setColorAt(0, QColor(0, 0, 0, 0));
        gradient.setColorAt(1, QColor(39, 39, 39, 255));
    }

    // 绘制横幅图片
    QPixmap scaled = m_banner.scaledToWidth(size().width(), Qt::SmoothTransformation);
    int y = (scaled.height() - size().height()) / 3;
    QPixmap pixmap = scaled.copy(0, y, size().width(), size().height());

    painter.drawPixmap(QRect(0, 0, size().width(), size().height()), pixmap);
    painter.fillPath(path, QBrush(gradient));
}

HomeInterface::HomeInterface(QWidget *parent)
    : ScrollArea(parent)
{
    m_view = new QWidget(this);
    m_vBoxLayout = new QVBoxLayout(m_view);
    m_banner = new BannerWidget(this);

    initWidget();
    loadSamples();
}

void HomeInterface::initWidget()
{
    m_view->setObjectName("view");
    setObjectName("homeInterface");

    auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/home_interface.qss");
    StyleSheetManager::instance()->registerWidget(styleSource, this);

    setWidget(m_view);
    setWidgetResizable(true);
    setViewportMargins(0, 0, 0, 0);
    m_vBoxLayout->setContentsMargins(0, 0, 0, 36);
    m_vBoxLayout->setSpacing(40);

    m_vBoxLayout->addWidget(m_banner);
    m_vBoxLayout->setAlignment(Qt::AlignTop);
}

void HomeInterface::loadSamples()
{
    // 基础输入样例
    SampleCardView *basicInputView = new SampleCardView(tr("Basic input samples"), m_view);
    basicInputView->addSampleCard(
        ":/res/controls/Button.png",
        "Button",
        tr("A control that responds to user input and emit clicked signal."),
        "basicInputInterface",
        0
    );
    basicInputView->addSampleCard(
        ":/res/controls/Checkbox.png",
        "CheckBox",
        tr("A control that a user can select or clear."),
        "basicInputInterface",
        8
    );
    basicInputView->addSampleCard(
        ":/res/controls/ComboBox.png",
        "ComboBox",
        tr("A drop-down list of items a user can select from."),
        "basicInputInterface",
        10
    );
    basicInputView->addSampleCard(
        ":/res/controls/DropDownButton.png",
        "DropDownButton",
        tr("A button that displays a flyout of choices when clicked."),
        "basicInputInterface",
        12
    );
    basicInputView->addSampleCard(
        ":/res/controls/HyperlinkButton.png",
        "HyperlinkButton",
        tr("A button that appears as hyperlink text, and can navigate to a URI or handle a Click event."),
        "basicInputInterface",
        18
    );
    basicInputView->addSampleCard(
        ":/res/controls/RadioButton.png",
        "RadioButton",
        tr("A control that allows a user to select a single option from a group of options."),
        "basicInputInterface",
        19
    );
    basicInputView->addSampleCard(
        ":/res/controls/Slider.png",
        "Slider",
        tr("A control that lets the user select from a range of values by moving a Thumb control along a track."),
        "basicInputInterface",
        20
    );
    basicInputView->addSampleCard(
        ":/res/controls/SplitButton.png",
        "SplitButton",
        tr("A two-part button that displays a flyout when its secondary part is clicked."),
        "basicInputInterface",
        21
    );
    basicInputView->addSampleCard(
        ":/res/controls/ToggleSwitch.png",
        "SwitchButton",
        tr("A switch that can be toggled between 2 states."),
        "basicInputInterface",
        25
    );
    basicInputView->addSampleCard(
        ":/res/controls/ToggleButton.png",
        "ToggleButton",
        tr("A button that can be switched between two states like a CheckBox."),
        "basicInputInterface",
        26
    );
    m_vBoxLayout->addWidget(basicInputView);

    // 日期时间样例
    SampleCardView *dateTimeView = new SampleCardView(tr("Date & time samples"), m_view);
    dateTimeView->addSampleCard(
        ":/res/controls/CalendarDatePicker.png",
        "CalendarPicker",
        tr("A control that lets a user pick a date value using a calendar."),
        "dateTimeInterface",
        0
    );
    dateTimeView->addSampleCard(
        ":/res/controls/DatePicker.png",
        "DatePicker",
        tr("A control that lets a user pick a date value."),
        "dateTimeInterface",
        2
    );
    dateTimeView->addSampleCard(
        ":/res/controls/TimePicker.png",
        "TimePicker",
        tr("A configurable control that lets a user pick a time value."),
        "dateTimeInterface",
        4
    );
    m_vBoxLayout->addWidget(dateTimeView);

    // 对话框样例
    SampleCardView *dialogView = new SampleCardView(tr("Dialog samples"), m_view);
    dialogView->addSampleCard(
        ":/res/controls/Flyout.png",
        "Dialog",
        tr("A frameless message dialog."),
        "dialogInterface",
        0
    );
    dialogView->addSampleCard(
        ":/res/controls/ContentDialog.png",
        "MessageBox",
        tr("A message dialog with mask."),
        "dialogInterface",
        1
    );
    dialogView->addSampleCard(
        ":/res/controls/ColorPicker.png",
        "ColorDialog",
        tr("A dialog that allows user to select color."),
        "dialogInterface",
        2
    );
    dialogView->addSampleCard(
        ":/res/controls/Flyout.png",
        "Flyout",
        tr("Shows contextual information and enables user interaction."),
        "dialogInterface",
        3
    );
    dialogView->addSampleCard(
        ":/res/controls/TeachingTip.png",
        "TeachingTip",
        tr("A content-rich flyout for guiding users and enabling teaching moments."),
        "dialogInterface",
        5
    );
    m_vBoxLayout->addWidget(dialogView);

    // 布局样例
    SampleCardView *layoutView = new SampleCardView(tr("Layout samples"), m_view);
    layoutView->addSampleCard(
        ":/res/controls/Grid.png",
        "FlowLayout",
        tr("A layout arranges components in a left-to-right flow, wrapping to the next row when the current row is full."),
        "layoutInterface",
        0
    );
    m_vBoxLayout->addWidget(layoutView);

    // 材质样例
    SampleCardView *materialView = new SampleCardView(tr("Material samples"), m_view);
    materialView->addSampleCard(
        ":/res/controls/Acrylic.png",
        "AcrylicLabel",
        tr("A translucent material recommended for panel background."),
        "materialInterface",
        0
    );
    m_vBoxLayout->addWidget(materialView);

    // 菜单与工具栏样例
    SampleCardView *menuView = new SampleCardView(tr("Menu & toolbars samples"), m_view);
    menuView->addSampleCard(
        ":/res/controls/MenuFlyout.png",
        "RoundMenu",
        tr("Shows a contextual list of simple commands or options."),
        "menuInterface",
        0
    );
    menuView->addSampleCard(
        ":/res/controls/CommandBar.png",
        "CommandBar",
        tr("Shows a contextual list of simple commands or options."),
        "menuInterface",
        2
    );
    menuView->addSampleCard(
        ":/res/controls/CommandBarFlyout.png",
        "CommandBarFlyout",
        tr("A mini-toolbar displaying proactive commands, and an optional menu of commands."),
        "menuInterface",
        3
    );
    m_vBoxLayout->addWidget(menuView);

    // 导航样例
    SampleCardView *navigationView = new SampleCardView(tr("Navigation"), m_view);
    navigationView->addSampleCard(
        ":/res/controls/BreadcrumbBar.png",
        "BreadcrumbBar",
        tr("Shows the trail of navigation taken to the current location."),
        "navigationViewInterface",
        0
    );
    navigationView->addSampleCard(
        ":/res/controls/Pivot.png",
        "Pivot",
        tr("Presents information from different sources in a tabbed view."),
        "navigationViewInterface",
        1
    );
    navigationView->addSampleCard(
        ":/res/controls/TabView.png",
        "TabView",
        tr("Presents information from different sources in a tabbed view."),
        "navigationViewInterface",
        3
    );
    m_vBoxLayout->addWidget(navigationView);

    // 滚动样例
    SampleCardView *scrollView = new SampleCardView(tr("Scrolling samples"), m_view);
    scrollView->addSampleCard(
        ":/res/controls/ScrollViewer.png",
        "ScrollArea",
        tr("A container control that lets the user pan and zoom its content smoothly."),
        "scrollInterface",
        0
    );
    scrollView->addSampleCard(
        ":/res/controls/PipsPager.png",
        "PipsPager",
        tr("A control to let the user navigate through a paginated collection when the page numbers do not need to be visually known."),
        "scrollInterface",
        3
    );
    m_vBoxLayout->addWidget(scrollView);

    // 状态与信息样例
    SampleCardView *stateInfoView = new SampleCardView(tr("Status & info samples"), m_view);
    stateInfoView->addSampleCard(
        ":/res/controls/ProgressRing.png",
        "StateToolTip",
        tr("Shows the apps progress on a task, or that the app is performing ongoing work that does block user interaction."),
        "statusInfoInterface",
        0
    );
    stateInfoView->addSampleCard(
        ":/res/controls/InfoBadge.png",
        "InfoBadge",
        tr("An non-intrusive Ul to display notifications or bring focus to an area."),
        "statusInfoInterface",
        3
    );
    stateInfoView->addSampleCard(
        ":/res/controls/InfoBar.png",
        "InfoBar",
        tr("An inline message to display app-wide status change information."),
        "statusInfoInterface",
        4
    );
    stateInfoView->addSampleCard(
        ":/res/controls/ProgressBar.png",
        "ProgressBar",
        tr("Shows the apps progress on a task, or that the app is performing ongoing work that doesn't block user interaction."),
        "statusInfoInterface",
        8
    );
    stateInfoView->addSampleCard(
        ":/res/controls/ProgressRing.png",
        "ProgressRing",
        tr("Shows the apps progress on a task, or that the app is performing ongoing work that doesn't block user interaction."),
        "statusInfoInterface",
        10
    );
    stateInfoView->addSampleCard(
        ":/res/controls/ToolTip.png",
        "ToolTip",
        tr("Displays information for an element in a pop-up window."),
        "statusInfoInterface",
        1
    );
    m_vBoxLayout->addWidget(stateInfoView);

    // 文本样例
    SampleCardView *textView = new SampleCardView(tr("Text samples"), m_view);
    textView->addSampleCard(
        ":/res/controls/TextBox.png",
        "LineEdit",
        tr("A single-line plain text field."),
        "textInterface",
        0
    );
    textView->addSampleCard(
        ":/res/controls/PasswordBox.png",
        "PasswordLineEdit",
        tr("A control for entering passwords."),
        "textInterface",
        2
    );
    textView->addSampleCard(
        ":/res/controls/NumberBox.png",
        "SpinBox",
        tr("A text control used for numeric input and evaluation of algebraic equations."),
        "textInterface",
        3
    );
    textView->addSampleCard(
        ":/res/controls/RichEditBox.png",
        "TextEdit",
        tr("A rich text editing control that supports formatted text, hyperlinks, and other rich content."),
        "textInterface",
        8
    );
    m_vBoxLayout->addWidget(textView);

    // 视图样例
    SampleCardView *collectionView = new SampleCardView(tr("View samples"), m_view);
    collectionView->addSampleCard(
        ":/res/controls/ListView.png",
        "ListView",
        tr("A control that presents a collection of items in a vertical list."),
        "viewInterface",
        0
    );
    collectionView->addSampleCard(
        ":/res/controls/DataGrid.png",
        "TableView",
        tr("The DataGrid control provides a flexible way to display a collection of data in rows and columns."),
        "viewInterface",
        1
    );
    collectionView->addSampleCard(
        ":/res/controls/TreeView.png",
        "TreeView",
        tr("The TreeView control is a hierarchical list pattern with expanding and collapsing nodes that contain nested items."),
        "viewInterface",
        2
    );
    collectionView->addSampleCard(
        ":/res/controls/FlipView.png",
        "FlipView",
        tr("Presents a collection of items that the user can flip through, one item at a time."),
        "viewInterface",
        4
    );
    m_vBoxLayout->addWidget(collectionView);
}
