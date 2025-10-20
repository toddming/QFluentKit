#include "EditableComboBox.h"
#include <QApplication>
#include <QScreen>
#include <QActionGroup>

#include "Theme.h"
#include "Icon.h"
#include "Animation.h"
#include "Private/EditableComboBoxPrivate.h"


Q_PROPERTY_CREATE_Q_CPP(EditableComboBox, int, MaxVisibleItems)
EditableComboBox::EditableComboBox(QWidget *parent)
    : LineEdit(parent), d_ptr(new EditableComboBoxPrivate())
{
    Q_D(EditableComboBox);;
    d->q_ptr = this;

    installEventFilter(this);

    // Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::COMBO_BOX);

    d->_pCurrentIndex = -1;
    d->_pMaxVisibleItems = -1;
    d->_arrowAni = new TranslateYAnimation(this);

    d->_dropButton = new LineEditButton(Icon::FluentIcon(IconType::FLuentIcon::ARROW_DOWN), this);
    d->_dropButton->setFixedSize(30, 25);
    hBoxLayout()->addWidget(d->_dropButton, 0, Qt::AlignRight);
    setTextMargins(0, 0, 29, 0);
    setClearButtonEnabled(false);

    connect(d->_dropButton, &LineEditButton::clicked, d, &EditableComboBoxPrivate::toggleComboMenu);
    connect(this, &LineEdit::textChanged, d, &EditableComboBoxPrivate::onComboTextChanged);
    getClearButton()->disconnect();
    connect(getClearButton(), &LineEditButton::clicked, d, &EditableComboBoxPrivate::onClearButtonClicked);
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
    if (index < 0 || index > count()) index = count();

    d->_items.insert(index, EditableComboBoxDetail::ComboItem(text, icon, userData));

    if (index <= d->_pCurrentIndex) {
        setCurrentIndex(d->_pCurrentIndex + 1);
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

void EditableComboBox::clear()
{
    Q_D(EditableComboBox);

    if (d->_pCurrentIndex >= 0) {
        setText("");
        d->_pCurrentIndex = -1;
    }
    d->_items.clear();
}

int EditableComboBox::currentIndex() const
{
    Q_D_CONST(EditableComboBox);

    return d->_pCurrentIndex;
}

QString EditableComboBox::currentText() const
{
    return text();
}

QVariant EditableComboBox::currentData() const
{
    Q_D_CONST(EditableComboBox);

    if (d->_pCurrentIndex >= 0 && d->_pCurrentIndex < count()) {
        return d->_items[d->_pCurrentIndex].userData;
    }
    return QVariant(NULL);
}

void EditableComboBox::setCurrentIndex(int index)
{
    Q_D(EditableComboBox);

    if (index >= count() || index == d->_pCurrentIndex)
        return;

    if (index < 0) {
        d->_pCurrentIndex = -1;
        setText("");
        setPlaceholderText(d->_pPlaceholderText);
    } else {
        d->_pCurrentIndex = index;
        setText(d->_items.at(index).text);
        d->updateTextState(false);
    }
}

void EditableComboBox::setCurrentText(const QString &text)
{
    if (text == currentText()) return;

    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

int EditableComboBox::count() const
{
    Q_D_CONST(EditableComboBox);

    return d->_items.size();
}

QString EditableComboBox::itemText(int index) const
{
    Q_D_CONST(EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->_items[index].text;
    }
    return "";
}

QIcon EditableComboBox::itemIcon(int index) const
{
    Q_D_CONST(EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->_items[index].icon;
    }
    return QIcon();
}

QVariant EditableComboBox::itemData(int index) const
{
    Q_D_CONST(EditableComboBox);

    if (index >= 0 && index < count()) {
        return d->_items[index].userData;
    }
    return QVariant(NULL);
}

int EditableComboBox::findText(const QString &text) const
{
    Q_D_CONST(EditableComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->_items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int EditableComboBox::findData(const QVariant &data) const
{
    Q_D_CONST(EditableComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->_items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void EditableComboBox::setPlaceholderText(const QString &text)
{
    Q_D(EditableComboBox);
    d->_pPlaceholderText = text;
    if (d->_pCurrentIndex == -1) {
        setText(text);
        d->updateTextState(true);
    }
}

QString EditableComboBox::placeholderText() const
{
    Q_D_CONST(EditableComboBox);

    return d->_pPlaceholderText;
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
    return LineEdit::eventFilter(watched, event);
}



