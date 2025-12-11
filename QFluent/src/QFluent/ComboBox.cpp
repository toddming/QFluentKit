#include "Private/ComboBoxPrivate.h"

#include "ComboBox.h"
#include <QApplication>
#include <QScreen>
#include <QActionGroup>
#include <QPainter>

#include "Animation.h"
#include "FluentIcon.h"
#include "StyleSheet.h"

ComboBox::ComboBox(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new ComboBoxPrivate(this))
{
    Q_D(ComboBox);

    setCurrentIndex(-1);

    setMaxVisibleItems(-1);

    installEventFilter(this);

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::COMBO_BOX);

    d->arrowAni = new TranslateYAnimation(this);
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
    if (index < 0 || index > count()) {
        index = count();
    }

    d->items.insert(index, ComboBoxDetail::ComboItem(text, icon, userData));

    if (index <= d->currentIndex) {
        setCurrentIndex(d->currentIndex + 1);
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

    if (index < 0 || index >= count()) {
        return;
    }

    d->items.removeAt(index);

    if (index < d->currentIndex) {
        setCurrentIndex(d->currentIndex - 1);
    } else if (index == d->currentIndex) {
        if (index > 0) {
            setCurrentIndex(d->currentIndex - 1);
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

    if (d->currentIndex >= 0) {
        setText("");
        d->currentIndex = -1;
    }
    d->items.clear();
}

int ComboBox::currentIndex() const
{
    Q_D(const ComboBox);

    return d->currentIndex;
}

QString ComboBox::currentText() const
{
    Q_D(const ComboBox);

    if (d->currentIndex >= 0 && d->currentIndex < count()) {
        return d->items[d->currentIndex].text;
    }
    return "";
}

QVariant ComboBox::currentData() const
{
    Q_D(const ComboBox);

    if (d->currentIndex >= 0 && d->currentIndex < count()) {
        return d->items[d->currentIndex].userData;
    }
    return {};
}

void ComboBox::setCurrentIndex(int index)
{
    Q_D(ComboBox);
    if (index < -1 || index >= count() || index == d->currentIndex) {
        return;
    }

    QString oldText = currentText();

    if (index == -1) {
        d->currentIndex = -1;
        setPlaceholderText(d->placeholderText);
    } else {
        d->currentIndex = index;
        setText(d->items[index].text);
        d->updateTextState(false);
    }

    if (oldText != currentText()) {
        emit currentTextChanged(currentText());
    }

    emit currentIndexChanged(index);
}

void ComboBox::setCurrentText(const QString &text)
{
    if (text == currentText()) {
        return;
    }

    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

int ComboBox::count() const
{
    Q_D(const ComboBox);
    return d->items.size();
}

QString ComboBox::itemText(int index) const
{
    Q_D(const ComboBox);
    if (index >= 0 && index < count()) {
        return d->items[index].text;
    }
    return "";
}

QIcon ComboBox::itemIcon(int index) const
{
    Q_D(const ComboBox);
    if (index >= 0 && index < count()) {
        return d->items[index].icon;
    }
    return QIcon();
}

QVariant ComboBox::itemData(int index) const
{
    Q_D(const ComboBox);

    if (index >= 0 && index < count()) {
        return d->items[index].userData;
    }
    return {};
}

int ComboBox::findText(const QString &text) const
{
    Q_D(const ComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int ComboBox::findData(const QVariant &data) const
{
    Q_D(const ComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void ComboBox::setPlaceholderText(const QString &text)
{
    Q_D(ComboBox);
    d->placeholderText = text;
    if (d->currentIndex == -1) {
        setText(text);
        d->updateTextState(true);
    }
}

QString ComboBox::placeholderText() const
{
    Q_D(const ComboBox);
    return d->placeholderText;
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

    if (d->isHover) {
        painter.setOpacity(0.8);
    } else if (d->isPressed) {
        painter.setOpacity(0.7);
    }

    QRectF rect = QRectF(width() - 22, height() / 2 - 5 + d->arrowAni->y(), 10, 10);
    FluentIcon(Fluent::IconType::ARROW_DOWN).render(&painter, rect);
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
            d->isPressed = true;
            update();
            break;
        case QEvent::MouseButtonRelease:
            d->isPressed = false;
            update();
            break;
        case QEvent::Enter:
            d->isHover = true;
            update();
            break;
        case QEvent::Leave:
            d->isHover = false;
            update();
            break;
        default:
            break;
        }
    }
    return QPushButton::eventFilter(watched, event);
}

void ComboBox::setMaxVisibleItems(int count)
{
    Q_D(ComboBox);
    d->maxVisibleItems = count;
}

int ComboBox::maxVisibleItems() const
{
    Q_D(const ComboBox);
    return d->maxVisibleItems;
}
