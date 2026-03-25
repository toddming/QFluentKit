#include "EditableComboBox.h"
#include <QApplication>
#include <QScreen>
#include <QActionGroup>

#include "FluentIcon.h"
#include "Animation.h"
#include "Private/EditableComboBoxPrivate.h"

EditableComboBox::EditableComboBox(QWidget *parent)
    : LineEdit(parent)
    , d_ptr(new EditableComboBoxPrivate(this))
{
    Q_D(EditableComboBox);

    installEventFilter(this);

    d->currentIndex = -1;
    d->maxVisibleItems = -1;
    d->arrowAni = new TranslateYAnimation(this);

    d->dropButton = new LineEditButton(FluentIcon(Fluent::IconType::ARROW_DOWN).qicon(), this);
    d->dropButton->setFixedSize(30, 25);
    hBoxLayout()->addWidget(d->dropButton, 0, Qt::AlignRight);
    setClearButtonEnabled(false);
    setTextMargins(0, 0, 29, 0);

    connect(d->dropButton, &LineEditButton::clicked, d, &EditableComboBoxPrivate::toggleComboMenu);
    connect(this, &LineEdit::textChanged, d, &EditableComboBoxPrivate::onComboTextChanged);
    disconnect(clearButton(), &LineEditButton::clicked, nullptr, nullptr);
    connect(clearButton(), &LineEditButton::clicked, d, &EditableComboBoxPrivate::onClearButtonClicked);
    connect(this, &EditableComboBox::returnPressed, d, &EditableComboBoxPrivate::onReturnPressed);
}

EditableComboBox::~EditableComboBox()
{
}

void EditableComboBox::addItem(const QString &text, const QIcon &icon, const QVariant &userData)
{
    insertItem(count(), text, icon, userData);
}

void EditableComboBox::addItems(const QStringList &texts)
{
    for (const QString &text : texts) {
        addItem(text);
    }
}

void EditableComboBox::insertItem(int index, const QString &text, const QIcon &icon, const QVariant &userData)
{
    Q_D(EditableComboBox);
    if (index < 0 || index > count()) {
        index = count();
    }

    d->items.insert(index, EditableComboBoxDetail::ComboItem(text, icon, userData));

    if (index <= d->currentIndex) {
        setCurrentIndex(d->currentIndex + 1);
    }
}

void EditableComboBox::insertItems(int index, const QStringList &texts)
{
    for (const QString &text : texts) {
        insertItem(index++, text);
    }
}

void EditableComboBox::removeItem(int index)
{
    Q_D(EditableComboBox);

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

void EditableComboBox::clear()
{
    Q_D(EditableComboBox);

    if (d->currentIndex >= 0) {
        setText("");
        d->currentIndex = -1;
    }
    d->items.clear();
}

int EditableComboBox::currentIndex() const
{
    Q_D(const EditableComboBox);

    return d->currentIndex;
}

QString EditableComboBox::currentText() const
{
    return text();
}

QVariant EditableComboBox::currentData() const
{
    Q_D(const EditableComboBox);

    if (d->currentIndex >= 0 && d->currentIndex < count()) {
        return d->items[d->currentIndex].userData;
    }
    return {};
}

void EditableComboBox::setCurrentIndex(int index)
{
    Q_D(EditableComboBox);

    if (index >= count() || index == d->currentIndex) {
        return;
    }

    if (index < 0) {
        d->currentIndex = -1;
        setText("");
        setPlaceholderText(d->placeholderText);
    } else {
        d->currentIndex = index;
        setText(d->items.at(index).text);
        d->updateTextState(false);
    }
}

void EditableComboBox::setCurrentText(const QString &text)
{
    if (text == currentText()) {
        return;
    }

    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

int EditableComboBox::count() const
{
    Q_D(const EditableComboBox);

    return d->items.size();
}

QString EditableComboBox::itemText(int index) const
{
    Q_D(const EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->items[index].text;
    }
    return "";
}

QIcon EditableComboBox::itemIcon(int index) const
{
    Q_D(const EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->items[index].icon;
    }
    return QIcon();
}

QVariant EditableComboBox::itemData(int index) const
{
    Q_D(const EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->items[index].userData;
    }
    return {};
}

int EditableComboBox::findText(const QString &text) const
{
    Q_D(const EditableComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int EditableComboBox::findData(const QVariant &data) const
{
    Q_D(const EditableComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void EditableComboBox::setPlaceholderText(const QString &text)
{
    Q_D(EditableComboBox);
    d->placeholderText = text;
    if (d->currentIndex == -1) {
        setText(text);
        d->updateTextState(true);
    }
}

QString EditableComboBox::placeholderText() const
{
    Q_D(const EditableComboBox);

    return d->placeholderText;
}

void EditableComboBox::setText(const QString &text)
{
    LineEdit::setText(text);
    adjustSize();
}

void EditableComboBox::setCompleterMenu(CompleterMenu *menu)
{
    Q_D(EditableComboBox);

    LineEdit::setCompleterMenu(menu);
    connect(menu, &CompleterMenu::activated, d, &EditableComboBoxPrivate::onActivated);
}

void EditableComboBox::paintEvent(QPaintEvent *event)
{
    LineEdit::paintEvent(event);
}

void EditableComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    LineEdit::mouseReleaseEvent(event);
}

bool EditableComboBox::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(EditableComboBox);

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
    return LineEdit::eventFilter(watched, event);
}

void EditableComboBox::setMaxVisibleItems(int count)
{
    Q_D(EditableComboBox);
    d->maxVisibleItems = count;
}

int EditableComboBox::maxVisibleItems() const
{
    Q_D(const EditableComboBox);
    return d->maxVisibleItems;
}
