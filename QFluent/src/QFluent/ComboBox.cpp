#include "ComboBox.h"
#include <QApplication>
#include <QScreen>
#include <QActionGroup>
#include <QDebug>
#include <QPainter>

#include "Private/ComboBoxPrivate.h"
#include "Animation.h"
#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"


Q_PROPERTY_CREATE_Q_CPP(ComboBox, int, MaxVisibleItems)
ComboBox::ComboBox(QWidget *parent)
    : QPushButton(parent), d_ptr(new ComboBoxPrivate())
{
    Q_D(ComboBox);
    d->q_ptr = this;

    setCurrentIndex(-1);

    setMaxVisibleItems(-1);

    installEventFilter(this);

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::COMBO_BOX);

    d->_arrowAni = new TranslateYAnimation(this);
}

ComboBox::~ComboBox()
{

}

void ComboBox::addItem(const QString &text, const QIcon &icon, const QVariant &userData)
{
    insertItem(count(), text, icon, userData);
}

void ComboBox::addItems(const QStringList &texts)
{
    for (const QString &text : texts) {
        addItem(text);
    }
}

void ComboBox::insertItem(int index, const QString &text, const QIcon &icon, const QVariant &userData)
{
    Q_D(ComboBox);
    if (index < 0 || index > count()) index = count();

    d->_items.insert(index, ComboBoxDetail::ComboItem(text, icon, userData));

    if (index <= d->_pCurrentIndex) {
        setCurrentIndex(d->_pCurrentIndex + 1);
    }
}

void ComboBox::insertItems(int index, const QStringList &texts)
{
    for (const QString &text : texts) {
        insertItem(index++, text);
    }
}

void ComboBox::removeItem(int index)
{
    Q_D(ComboBox);

    if (index < 0 || index >= count()) return;

    d->_items.removeAt(index);

    if (index < d->_pCurrentIndex) {
        setCurrentIndex(d->_pCurrentIndex - 1);
    } else if (index == d->_pCurrentIndex) {
        if (index > 0) {
            setCurrentIndex(d->_pCurrentIndex - 1);
        } else if (count() > 0) {
            setCurrentIndex(0);
        } else {
            setCurrentIndex(-1);
        }
    }
}

void ComboBox::clear()
{
    Q_D(ComboBox);

    if (d->_pCurrentIndex >= 0) {
        setText("");
        d->_pCurrentIndex = -1;
    }
    d->_items.clear();
}

int ComboBox::currentIndex() const
{
    Q_D_CONST(ComboBox);

    return d->_pCurrentIndex;
}

QString ComboBox::currentText() const
{
    Q_D_CONST(ComboBox);

    if (d->_pCurrentIndex >= 0 && d->_pCurrentIndex < count()) {
        return d->_items[d->_pCurrentIndex].text;
    }
    return "";
}

QVariant ComboBox::currentData() const
{
    Q_D_CONST(ComboBox);

    if (d->_pCurrentIndex >= 0 && d->_pCurrentIndex < count()) {
        return d->_items[d->_pCurrentIndex].userData;
    }
    return {};
}

void ComboBox::setCurrentIndex(int index)
{
    Q_D(ComboBox);
    if (index < -1 || index >= count() || index == d->_pCurrentIndex)
        return;

    QString oldText = currentText();

    if (index == -1) {
        d->_pCurrentIndex = -1;
        setPlaceholderText(d->_pPlaceholderText);
    } else {
        d->_pCurrentIndex = index;
        setText(d->_items[index].text);
        d->updateTextState(false);
    }

    if (oldText != currentText()) {
        emit currentTextChanged(currentText());
    }

    emit currentIndexChanged(index);
}

void ComboBox::setCurrentText(const QString &text)
{
    if (text == currentText()) return;

    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

int ComboBox::count() const
{
    Q_D_CONST(ComboBox);
    return d->_items.size();
}

QString ComboBox::itemText(int index) const
{
    Q_D_CONST(ComboBox);
    if (index >= 0 && index < count()) {
        return d->_items[index].text;
    }
    return "";
}

QIcon ComboBox::itemIcon(int index) const
{
    Q_D_CONST(ComboBox);
    if (index >= 0 && index < count()) {
        return d->_items[index].icon;
    }
    return QIcon();
}

QVariant ComboBox::itemData(int index) const
{
    Q_D_CONST(ComboBox);

    if (index >= 0 && index < count()) {
        return d->_items[index].userData;
    }
    return {};
}

int ComboBox::findText(const QString &text) const
{
    Q_D_CONST(ComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->_items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int ComboBox::findData(const QVariant &data) const
{
    Q_D_CONST(ComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->_items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void ComboBox::setPlaceholderText(const QString &text)
{
    Q_D(ComboBox);
    d->_pPlaceholderText = text;
    if (d->_pCurrentIndex == -1) {
        setText(text);
        d->updateTextState(true);
    }
}

QString ComboBox::placeholderText() const
{
    Q_D_CONST(ComboBox);
    return d->_pPlaceholderText;
}

void ComboBox::setText(const QString &text)
{
    QPushButton::setText(text);
    adjustSize();
}

void ComboBox::paintEvent(QPaintEvent *event)
{
    Q_D(ComboBox);
    QPushButton::paintEvent(event);

    // 绘制下拉箭头
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (d->_pIsHover) {
        painter.setOpacity(0.8);
    } else if (d->_pIsPressed) {
        painter.setOpacity(0.7);
    }

    QRectF rect = QRectF(width()-22, height()/2-5+d->_arrowAni->y(), 10, 10);
    FluentIcon(FluentIconType::ARROW_DOWN).render(&painter, rect);
}

void ComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(ComboBox);
    QPushButton::mouseReleaseEvent(event);
    d->toggleComboMenu();
}


bool ComboBox::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(ComboBox);
    if (watched == this) {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            d->_pIsPressed = true;
            update();
            break;
        case QEvent::MouseButtonRelease:
            d->_pIsPressed = false;
            update();
            break;
        case QEvent::Enter:
            d->_pIsHover = true;
            update();
            break;
        case QEvent::Leave:
            d->_pIsHover = false;
            update();
            break;
        default:
            break;
        }
    }
    return QPushButton::eventFilter(watched, event);
}


