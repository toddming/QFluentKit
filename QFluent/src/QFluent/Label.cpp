#include "Label.h"

#include <QApplication>
#include <QPalette>
#include <QMetaObject>
#include <QDesktopServices>

#include "Theme.h"
#include "StyleSheet.h"

FluentLabelBase::FluentLabelBase(int fontSize, QFont::Weight weight, QWidget* parent)
    : QLabel(parent) {
    setFont(Theme::instance()->font(fontSize, weight));

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::LABEL);
}

FluentLabelBase::FluentLabelBase(const QString& text, int fontSize, QFont::Weight weight, QWidget* parent)
    : QLabel(text, parent) {
    setFont(Theme::instance()->font(fontSize, weight));

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::LABEL);
}


void FluentLabelBase::setTextColor(const QColor& lightColor, const QColor& darkColor) {
    QString lightColorStr = lightColor.name(QColor::HexArgb);
    QString darkColorStr  = darkColor.name(QColor::HexArgb);

    auto customStyle = std::make_shared<CustomStyleSheet>(this);
    customStyle->setCustomStyleSheet(
        QString("FluentLabelBase{color:%1}").arg(lightColorStr),
        QString("FluentLabelBase{color:%1}").arg(darkColorStr)
        );

    StyleSheetManager::instance()->registerWidget(customStyle, this);

}


int FluentLabelBase::pixelFontSize() const {
    return font().pixelSize();
}

void FluentLabelBase::setPixelFontSize(int size) {
    QFont f = font();
    f.setPixelSize(size);
    setFont(f);
}

bool FluentLabelBase::strikeOut() const {
    return font().strikeOut();
}

void FluentLabelBase::setStrikeOut(bool isStrikeOut) {
    QFont f = font();
    f.setStrikeOut(isStrikeOut);
    setFont(f);
}

bool FluentLabelBase::underline() const {
    return font().underline();
}

void FluentLabelBase::setUnderline(bool isUnderline) {
    QFont f = font();
    f.setUnderline(isUnderline);
    setFont(f);
}

// ----------------- 具体标签类的实现 -----------------

CaptionLabel::CaptionLabel(QWidget* parent) : FluentLabelBase(12, QFont::Normal, parent) {}
CaptionLabel::CaptionLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 12, QFont::Normal, parent) {}


BodyLabel::BodyLabel(QWidget* parent) : FluentLabelBase(14, QFont::Normal, parent) {}
BodyLabel::BodyLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 14, QFont::Normal, parent) {}


StrongBodyLabel::StrongBodyLabel(QWidget* parent) : FluentLabelBase(14, QFont::DemiBold, parent) {}
StrongBodyLabel::StrongBodyLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 14, QFont::DemiBold, parent) {}


SubtitleLabel::SubtitleLabel(QWidget* parent) : FluentLabelBase(20, QFont::DemiBold, parent) {}
SubtitleLabel::SubtitleLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 20, QFont::DemiBold, parent) {}


TitleLabel::TitleLabel(QWidget* parent) : FluentLabelBase(28, QFont::DemiBold, parent) {}
TitleLabel::TitleLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 28, QFont::DemiBold, parent) {}


LargeTitleLabel::LargeTitleLabel(QWidget* parent) : FluentLabelBase(40, QFont::DemiBold, parent) {}
LargeTitleLabel::LargeTitleLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 40, QFont::DemiBold, parent) {}


DisplayLabel::DisplayLabel(QWidget* parent) : FluentLabelBase(68, QFont::DemiBold, parent) {}
DisplayLabel::DisplayLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 68, QFont::DemiBold, parent) {}


HyperlinkLabel::HyperlinkLabel(QWidget *parent)
    : QPushButton(parent), m_url(QUrl())
{
    init();
}

HyperlinkLabel::HyperlinkLabel(const QString &text, QWidget *parent)
    : QPushButton(parent), m_url(QUrl())
{
    init();
    setText(text);
}

HyperlinkLabel::HyperlinkLabel(const QUrl &url, const QString &text, QWidget *parent)
    : QPushButton(parent), m_url(url)
{
    init();
    setText(text);
}

void HyperlinkLabel::init()
{
    Theme::instance()->setFont(this, 14);
    setUnderlineVisible(false);
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::LABEL);

    setCursor(Qt::PointingHandCursor);
    connect(this, &HyperlinkLabel::clicked, this, &HyperlinkLabel::onClicked);
}

QUrl HyperlinkLabel::url() const
{
    return m_url;
}

void HyperlinkLabel::setUrl(const QUrl &url)
{
    m_url = QUrl(url);
}

bool HyperlinkLabel::isUnderlineVisible() const
{
    return m_isUnderlineVisible;
}

void HyperlinkLabel::setUnderlineVisible(bool isVisible)
{
    m_isUnderlineVisible = isVisible;
    setProperty("underline", isVisible);
    setStyle(QApplication::style());
}

void HyperlinkLabel::onClicked()
{
    if (url().isValid()) {
        QDesktopServices::openUrl(url());
    }
}
