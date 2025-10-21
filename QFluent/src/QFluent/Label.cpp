#include "Label.h"
#include <QApplication>
#include <QPalette>
#include <QMetaObject>
#include "Theme.h"
#include "StyleSheet.h"

FluentLabelBase::FluentLabelBase(int fontSize, QFont::Weight weight, QWidget* parent)
    : QLabel(parent) {
    setFont(Theme::instance()->getFont(fontSize, weight));

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::LABEL);
    setTextColor(Theme::instance()->isDarkTheme() ? Qt::white : Qt::black);
    connect(Theme::instance(), &Theme::themeModeChanged, this, [=](ThemeType::ThemeMode theme){
        setTextColor(theme == ThemeType::ThemeMode::DARK ? Qt::white : Qt::black);
    });
}

FluentLabelBase::FluentLabelBase(const QString& text, int fontSize, QFont::Weight weight, QWidget* parent)
    : QLabel(text, parent) {
    setFont(Theme::instance()->getFont(fontSize, weight));

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::LABEL);
    setTextColor(Theme::instance()->isDarkTheme() ? Qt::white : Qt::black);
    connect(Theme::instance(), &Theme::themeModeChanged, this, [=](ThemeType::ThemeMode theme){
        setTextColor(theme == ThemeType::ThemeMode::DARK ? Qt::white : Qt::black);
    });
}


void FluentLabelBase::setTextColor(const QColor& color) {
    QString colorStr = color.name(QColor::HexArgb);
    QString styleSheet = QString("FluentLabelBase{color:%1}").arg(colorStr);
    setStyleSheet(styleSheet);
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


TitleLabel::TitleLabel(QWidget* parent, QFont::Weight weight) : FluentLabelBase(28, weight, parent) {}
TitleLabel::TitleLabel(const QString& text, QWidget* parent, QFont::Weight weight) : FluentLabelBase(text, 28, weight, parent) {}


LargeTitleLabel::LargeTitleLabel(QWidget* parent) : FluentLabelBase(40, QFont::DemiBold, parent) {}
LargeTitleLabel::LargeTitleLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 40, QFont::DemiBold, parent) {}


DisplayLabel::DisplayLabel(QWidget* parent) : FluentLabelBase(68, QFont::DemiBold, parent) {}
DisplayLabel::DisplayLabel(const QString& text, QWidget* parent) : FluentLabelBase(text, 68, QFont::DemiBold, parent) {}

