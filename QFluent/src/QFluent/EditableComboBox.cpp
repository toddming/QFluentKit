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

    d->m_currentIndex = -1;
    d->m_maxVisibleItems = -1;
    d->m_arrowAni = new TranslateYAnimation(this);

    d->m_dropButton = new LineEditButton(FluentIcon(Fluent::IconType::ARROW_DOWN).qicon(), this);
    d->m_dropButton->setFixedSize(30, 25);
    hBoxLayout()->addWidget(d->m_dropButton, 0, Qt::AlignRight);
    setClearButtonEnabled(false);
    setTextMargins(0, 0, 29, 0);

    connect(d->m_dropButton, &LineEditButton::clicked, d, &EditableComboBoxPrivate::toggleComboMenu);
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

    d->m_items.insert(index, EditableComboBoxDetail::ComboItem(text, icon, userData));

    if (index <= d->m_currentIndex) {
        setCurrentIndex(d->m_currentIndex + 1);
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

    d->m_items.removeAt(index);

    if (index < d->m_currentIndex) {
        setCurrentIndex(d->m_currentIndex - 1);
    } else if (index == d->m_currentIndex) {
        if (index > 0) {
            setCurrentIndex(d->m_currentIndex - 1);
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

    if (d->m_currentIndex >= 0) {
        setText(QString());
        d->m_currentIndex = -1;
    }
    d->m_items.clear();
}

int EditableComboBox::currentIndex() const
{
    Q_D(const EditableComboBox);

    return d->m_currentIndex;
}

QString EditableComboBox::currentText() const
{
    return text();
}

QVariant EditableComboBox::currentData() const
{
    Q_D(const EditableComboBox);

    if (d->m_currentIndex >= 0 && d->m_currentIndex < count()) {
        return d->m_items[d->m_currentIndex].userData;
    }
    return QVariant();
}

void EditableComboBox::setCurrentIndex(int index)
{
    Q_D(EditableComboBox);

    if (index >= count() || index == d->m_currentIndex) {
        return;
    }

    if (index < 0) {
        d->m_currentIndex = -1;
        setText(QString());
        setPlaceholderText(d->m_placeholderText);
    } else {
        d->m_currentIndex = index;
        setText(d->m_items.at(index).text);
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

    return d->m_items.size();
}

QString EditableComboBox::itemText(int index) const
{
    Q_D(const EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->m_items[index].text;
    }
    return QString();
}

QIcon EditableComboBox::itemIcon(int index) const
{
    Q_D(const EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->m_items[index].icon;
    }
    return QIcon();
}

QVariant EditableComboBox::itemData(int index) const
{
    Q_D(const EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->m_items[index].userData;
    }
    return QVariant();
}

int EditableComboBox::findText(const QString &text) const
{
    Q_D(const EditableComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->m_items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int EditableComboBox::findData(const QVariant &data) const
{
    Q_D(const EditableComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->m_items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void EditableComboBox::setPlaceholderText(const QString &text)
{
    Q_D(EditableComboBox);
    d->m_placeholderText = text;
    if (d->m_currentIndex == -1) {
        setText(text);
        d->updateTextState(true);
    }
}

QString EditableComboBox::placeholderText() const
{
    Q_D(const EditableComboBox);

    return d->m_placeholderText;
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
            d->m_isPressed = true;
            update();
            break;
        case QEvent::MouseButtonRelease:
            d->m_isPressed = false;
            update();
            break;
        case QEvent::Enter:
            d->m_isHover = true;
            update();
            break;
        case QEvent::Leave:
            d->m_isHover = false;
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
    d->m_maxVisibleItems = count;
}

int EditableComboBox::maxVisibleItems() const
{
    Q_D(const EditableComboBox);
    return d->m_maxVisibleItems;
}
