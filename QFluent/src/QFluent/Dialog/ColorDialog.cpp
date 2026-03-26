#include "ColorDialog.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPen>
#include <QBrush>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIntValidator>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QRegularExpressionValidator>
    #include <QRegularExpression>
#else
    #include <QRegExpValidator>
    #include <QRegExp>
#endif
#include <QApplication>
#include <QScrollBar>
#include <QtMath>
#include <QWidget>
#include <QColor>
#include <QPixmap>
#include <QLabel>
#include <QFrame>

#include "QFluent/ScrollArea.h"
#include "QFluent/PushButton.h"
#include "StyleSheet.h"
#include "Theme.h"

// ============================================================================
// HuePanel Implementation
// ============================================================================

HuePanel::HuePanel(const QColor& color, QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(256, 256);
    m_huePixmap = QPixmap(":/qfluent/images/color_dialog/HuePanel.png");
    setColor(color);
}

void HuePanel::mousePressEvent(QMouseEvent* event)
{
    setPickerPosition(event->pos());
}

void HuePanel::mouseMoveEvent(QMouseEvent* event)
{
    setPickerPosition(event->pos());
}

void HuePanel::setPickerPosition(const QPoint& pos)
{
    m_pickerPos = pos;
    m_color.setHsv(
        static_cast<int>(qMax(0.0, qMin(1.0, pos.x() / static_cast<qreal>(width()))) * 359),
        static_cast<int>(qMax(0.0, qMin(1.0, (height() - pos.y()) / static_cast<qreal>(height()))) * 255),
        255
    );
    update();
    emit colorChanged(m_color);
}

void HuePanel::setColor(const QColor& color)
{
    m_color = color;
    m_color.setHsv(m_color.hue(), m_color.saturation(), 255);
    m_pickerPos = QPoint(
        static_cast<int>(hue() / 359.0 * width()),
        static_cast<int>((255 - saturation()) / 255.0 * height())
    );
    update();
}

void HuePanel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // 绘制色调面板
    painter.setBrush(QBrush(m_huePixmap));
    painter.setPen(QPen(QColor(0, 0, 0, 15), 2.4));
    painter.drawRoundedRect(rect(), 5.6, 5.6);

    // 绘制拾取器
    QColor pickerColor;
    if (saturation() > 153 || (40 < hue() && hue() < 180)) {
        pickerColor = Qt::black;
    } else {
        pickerColor = QColor(255, 253, 254);
    }

    painter.setPen(QPen(pickerColor, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(m_pickerPos.x() - 8, m_pickerPos.y() - 8, 16, 16);
}


// ============================================================================
// BrightnessSlider Implementation
// ============================================================================

BrightnessSlider::BrightnessSlider(const QColor& color, QWidget* parent)
    : ClickableSlider(Qt::Horizontal, parent)
{
    setRange(0, 255);
    setSingleStep(1);
    setColor(color);
    connect(this, &QSlider::valueChanged, this, &BrightnessSlider::onValueChanged);
}

void BrightnessSlider::setColor(const QColor& color)
{
    m_color = color;
    setValue(m_color.value());

    // 设置样式表
    QString qss = FluentStyleSheet(Fluent::ThemeStyle::COLOR_DIALOG).content();
    qss.replace("--slider-hue", QString::number(m_color.hue()));
    qss.replace("--slider-saturation", QString::number(m_color.saturation()));
    setStyleSheet(qss);
}

void BrightnessSlider::onValueChanged(int value)
{
    m_color.setHsv(m_color.hue(), m_color.saturation(), value, m_color.alpha());
    setColor(m_color);
    emit colorChanged(m_color);
}


// ============================================================================
// ColorCard Implementation
// ============================================================================

ColorCard::ColorCard(const QColor& color, QWidget* parent, bool enableAlpha)
    : QWidget(parent)
    , m_enableAlpha(enableAlpha)
{
    setFixedSize(44, 128);
    setColor(color);
    m_titledPixmap = createTitledBackground();
}

QPixmap ColorCard::createTitledBackground()
{
    QPixmap pixmap(8, 8);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);

    int c = Theme::instance()->isDarkTheme() ? 255 : 0;
    QColor color(c, c, c, 26);
    painter.fillRect(4, 0, 4, 4, color);
    painter.fillRect(0, 4, 4, 4, color);
    painter.end();
    return pixmap;
}

void ColorCard::setColor(const QColor& color)
{
    m_color = color;
    update();
}

void ColorCard::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // 绘制平铺背景
    if (m_enableAlpha) {
        painter.setBrush(QBrush(m_titledPixmap));
        painter.setPen(QColor(0, 0, 0, 13));
        painter.drawRoundedRect(rect(), 4, 4);
    }

    // 绘制颜色
    painter.setBrush(m_color);
    painter.setPen(QColor(0, 0, 0, 13));
    painter.drawRoundedRect(rect(), 4, 4);
}


// ============================================================================
// ColorLineEdit Implementation
// ============================================================================

ColorLineEdit::ColorLineEdit(const QString& value, QWidget* parent)
    : LineEdit(parent)
{
    setText(value);
    setFixedSize(136, 33);
    setClearButtonEnabled(true);
    setValidator(new QIntValidator(0, 255, this));

    connect(this, &QLineEdit::textEdited, this, &ColorLineEdit::onTextEdited);
}

void ColorLineEdit::onTextEdited(const QString& text)
{
    int pos = 0;
    QString temp = text;
    QValidator::State state = validator()->validate(temp, pos);
    if (state == QValidator::Acceptable) {
        emit valueChanged(text);
    }
}


// ============================================================================
// HexColorLineEdit Implementation
// ============================================================================

HexColorLineEdit::HexColorLineEdit(const QColor& color, QWidget* parent, bool enableAlpha)
    : ColorLineEdit(color.name(enableAlpha ? QColor::HexArgb : QColor::HexRgb).mid(1), parent)
    , m_colorFormat(enableAlpha ? QColor::HexArgb : QColor::HexRgb)
{
    QString pattern = enableAlpha ? "^[A-Fa-f0-9]{8}$" : "^[A-Fa-f0-9]{6}$";
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setValidator(new QRegularExpressionValidator(QRegularExpression(pattern), this));
#else
    setValidator(new QRegExpValidator(QRegExp(pattern), this));
#endif

    setTextMargins(4, 0, 33, 0);
    m_prefixLabel = new QLabel("#", this);
    m_prefixLabel->move(7, 2);
    m_prefixLabel->setObjectName("prefixLabel");
}

void HexColorLineEdit::setColor(const QColor& color)
{
    setText(color.name(m_colorFormat).mid(1));
}


// ============================================================================
// OpacityLineEdit Implementation
// ============================================================================

OpacityLineEdit::OpacityLineEdit(int value, QWidget* parent)
    : ColorLineEdit(QString::number(static_cast<int>(value / 255.0 * 100)), parent)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression regExp(R"(^(?:[0-9]{1,2}|100)$)");
    setValidator(new QRegularExpressionValidator(regExp, parent));
#else
    QRegExp regExp("[0-9]{1,2}|100");
    setValidator(new QRegExpValidator(regExp, parent));
#endif
    setTextMargins(4, 0, 33, 0);
    m_suffixLabel = new QLabel("%", this);
    m_suffixLabel->setObjectName("suffixLabel");
    connect(this, &QLineEdit::textChanged, this, &OpacityLineEdit::adjustSuffixPos);
}

void OpacityLineEdit::showEvent(QShowEvent* event)
{
    LineEdit::showEvent(event);
    adjustSuffixPos();
}

void OpacityLineEdit::adjustSuffixPos()
{
    int x = fontMetrics().boundingRect(text()).width() + 18;
    m_suffixLabel->move(x, 2);
}


// ============================================================================
// ColorDialog Implementation
// ============================================================================

ColorDialog::ColorDialog(const QColor& color, const QString& title,
                         QWidget* parent, bool enableAlpha)
    : MaskDialogBase(parent)
    , m_enableAlpha(enableAlpha)
{
    QColor initColor = color;
    if (!enableAlpha) {
        initColor.setAlpha(255);
    }

    m_oldColor = initColor;
    m_color = initColor;

    // 创建滚动区域
    m_scrollArea = new SingleDirectionScrollArea(centerWidget());
    m_scrollWidget = new QWidget(m_scrollArea);

    // 创建按钮组
    m_buttonGroup = new QFrame(centerWidget());
    m_yesButton = new PrimaryPushButton(tr("OK"), m_buttonGroup);
    m_cancelButton = new QPushButton(tr("Cancel"), m_buttonGroup);

    // 创建主要组件
    m_titleLabel = new QLabel(title, m_scrollWidget);
    m_huePanel = new HuePanel(initColor, m_scrollWidget);
    m_newColorCard = new ColorCard(initColor, m_scrollWidget, enableAlpha);
    m_oldColorCard = new ColorCard(initColor, m_scrollWidget, enableAlpha);
    m_brightSlider = new BrightnessSlider(initColor, m_scrollWidget);

    // 创建标签
    m_editLabel = new QLabel("编辑颜色", m_scrollWidget);
    m_redLabel = new QLabel(tr("红色"), m_scrollWidget);
    m_blueLabel = new QLabel(tr("绿色"), m_scrollWidget);
    m_greenLabel = new QLabel(tr("蓝色"), m_scrollWidget);
    m_opacityLabel = new QLabel(tr("透明"), m_scrollWidget);

    // 创建输入框
    m_hexLineEdit = new HexColorLineEdit(initColor, m_scrollWidget, enableAlpha);
    m_redLineEdit = new ColorLineEdit(QString::number(m_color.red()), m_scrollWidget);
    m_greenLineEdit = new ColorLineEdit(QString::number(m_color.green()), m_scrollWidget);
    m_blueLineEdit = new ColorLineEdit(QString::number(m_color.blue()), m_scrollWidget);
    m_opacityLineEdit = new OpacityLineEdit(m_color.alpha(), m_scrollWidget);

    // 创建布局
    m_vBoxLayout = new QVBoxLayout(centerWidget());

    initWidget();
}

void ColorDialog::initWidget()
{
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setViewportMargins(48, 24, 0, 24);
    m_scrollArea->setWidget(m_scrollWidget);

    int heightOffset = m_enableAlpha ? 40 : 0;
    centerWidget()->setFixedSize(488, 696 + heightOffset);
    m_scrollWidget->resize(440, 560 + heightOffset);
    m_buttonGroup->setFixedSize(486, 81);
    m_yesButton->setFixedWidth(216);
    m_cancelButton->setFixedWidth(216);

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 80));
    setMaskColor(QColor(0, 0, 0, 76));

    setQss();
    initLayout();
    connectSignalToSlot();
}

void ColorDialog::initLayout()
{
    m_huePanel->move(0, 46);
    m_newColorCard->move(288, 46);
    m_oldColorCard->move(288, m_newColorCard->geometry().bottom() + 1);
    m_brightSlider->move(0, 324);

    m_editLabel->move(0, 385);
    m_redLineEdit->move(0, 426);
    m_greenLineEdit->move(0, 470);
    m_blueLineEdit->move(0, 515);
    m_redLabel->move(144, 434);
    m_greenLabel->move(144, 478);
    m_blueLabel->move(144, 524);
    m_hexLineEdit->move(196, 381);

    if (m_enableAlpha) {
        m_opacityLineEdit->move(0, 560);
        m_opacityLabel->move(144, 567);
    } else {
        m_opacityLineEdit->hide();
        m_opacityLabel->hide();
    }

    m_vBoxLayout->setSpacing(0);
    m_vBoxLayout->setAlignment(Qt::AlignTop);
    m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_vBoxLayout->addWidget(m_scrollArea, 1);
    m_vBoxLayout->addWidget(m_buttonGroup, 0, Qt::AlignBottom);

    m_yesButton->move(24, 25);
    m_cancelButton->move(250, 25);
}

void ColorDialog::setQss()
{
    m_editLabel->setObjectName("editLabel");
    m_titleLabel->setObjectName("titleLabel");
    m_yesButton->setObjectName("yesButton");
    m_cancelButton->setObjectName("cancelButton");
    m_buttonGroup->setObjectName("buttonGroup");

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::COLOR_DIALOG);

    m_titleLabel->adjustSize();
    m_editLabel->adjustSize();
}

void ColorDialog::setColor(const QColor& color, bool movePicker)
{
    m_color = color;
    m_brightSlider->setColor(color);
    m_newColorCard->setColor(color);
    m_hexLineEdit->setColor(color);
    m_redLineEdit->setText(QString::number(color.red()));
    m_blueLineEdit->setText(QString::number(color.blue()));
    m_greenLineEdit->setText(QString::number(color.green()));

    if (movePicker) {
        m_huePanel->setColor(color);
    }
}

void ColorDialog::onHueChanged(const QColor& color)
{
    m_color.setHsv(color.hue(), color.saturation(), m_color.value(), m_color.alpha());
    setColor(m_color);
}

void ColorDialog::onBrightnessChanged(const QColor& color)
{
    m_color.setHsv(m_color.hue(), m_color.saturation(), color.value(), color.alpha());
    setColor(m_color, false);
}

void ColorDialog::onRedChanged(const QString& red)
{
    m_color.setRed(red.toInt());
    setColor(m_color);
}

void ColorDialog::onBlueChanged(const QString& blue)
{
    m_color.setBlue(blue.toInt());
    setColor(m_color);
}

void ColorDialog::onGreenChanged(const QString& green)
{
    m_color.setGreen(green.toInt());
    setColor(m_color);
}

void ColorDialog::onOpacityChanged(const QString& opacity)
{
    m_color.setAlpha(static_cast<int>(opacity.toInt() / 100.0 * 255));
    setColor(m_color);
}

void ColorDialog::onHexColorChanged(const QString& color)
{
    m_color.setNamedColor("#" + color);
    setColor(m_color);
}

void ColorDialog::onYesButtonClicked()
{
    accept();
    if (m_color != m_oldColor) {
        emit colorChanged(m_color);
    }
}

void ColorDialog::updateStyle()
{
    setStyle(QApplication::style());
    m_titleLabel->adjustSize();
    m_editLabel->adjustSize();
    m_redLabel->adjustSize();
    m_greenLabel->adjustSize();
    m_blueLabel->adjustSize();
    m_opacityLabel->adjustSize();
}

void ColorDialog::showEvent(QShowEvent* event)
{
    updateStyle();
    MaskDialogBase::showEvent(event);
}

void ColorDialog::connectSignalToSlot()
{
    connect(m_cancelButton, &QPushButton::clicked, this, &ColorDialog::reject);
    connect(m_yesButton, &QPushButton::clicked, this, &ColorDialog::onYesButtonClicked);

    connect(m_huePanel, &HuePanel::colorChanged, this, &ColorDialog::onHueChanged);
    connect(m_brightSlider, &BrightnessSlider::colorChanged, this, &ColorDialog::onBrightnessChanged);

    connect(m_redLineEdit, &ColorLineEdit::valueChanged, this, &ColorDialog::onRedChanged);
    connect(m_blueLineEdit, &ColorLineEdit::valueChanged, this, &ColorDialog::onBlueChanged);
    connect(m_greenLineEdit, &ColorLineEdit::valueChanged, this, &ColorDialog::onGreenChanged);
    connect(m_hexLineEdit, &ColorLineEdit::valueChanged, this, &ColorDialog::onHexColorChanged);
    connect(m_opacityLineEdit, &ColorLineEdit::valueChanged, this, &ColorDialog::onOpacityChanged);
}
