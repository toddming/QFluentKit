#include "EditableComboBox.h"
#include "Private/EditableComboBoxPrivate.h"

#include "FluentIcon.h"
#include "Animation.h"

#include <QAbstractItemModel>
#include <QEvent>

EditableComboBox::EditableComboBox(QWidget *parent)
    : LineEdit(parent)
    , d_ptr(new EditableComboBoxPrivate(this))
{
    Q_D(EditableComboBox);

    setObjectName("EditableComboBox");
    setFixedHeight(33);

    d->m_dropButton = new LineEditButton(Fluent::icon(Fluent::IconType::ARROW_DOWN), this);
    d->m_dropButton->setFixedSize(30, 25);
    hBoxLayout()->addWidget(d->m_dropButton, 0, Qt::AlignRight);
    setClearButtonEnabled(false);
    setTextMargins(0, 0, 29, 0);

    setPlaceholderText(tr("Please select"));

    connect(d->m_dropButton, &LineEditButton::clicked, d, &EditableComboBoxPrivate::toggleComboMenu);
    connect(this, &LineEdit::textChanged, d, &EditableComboBoxPrivate::onComboTextChanged);
    disconnect(clearButton(), &LineEditButton::clicked, nullptr, nullptr);
    connect(clearButton(), &LineEditButton::clicked, d, &EditableComboBoxPrivate::onClearButtonClicked);
    connect(this, &EditableComboBox::returnPressed, d, &EditableComboBoxPrivate::onReturnPressed);

    installEventFilter(this);
}

EditableComboBox::~EditableComboBox() = default;

void EditableComboBox::setModel(QAbstractItemModel *model)
{
    Q_D(EditableComboBox);
    d->setModel(model);
}

QAbstractItemModel *EditableComboBox::model() const
{
    Q_D(const EditableComboBox);
    return d->m_model;
}

void EditableComboBox::addItem(const QString &text, const QVariant &userData)
{
    Q_D(EditableComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRow(row);
    d->m_model->setData(d->m_model->index(row, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(row, 0), userData, Qt::UserRole);
}

void EditableComboBox::addItem(const QIcon &icon, const QString &text, const QVariant &userData)
{
    Q_D(EditableComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRow(row);
    d->m_model->setData(d->m_model->index(row, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(row, 0), icon, Qt::DecorationRole);
    d->m_model->setData(d->m_model->index(row, 0), userData, Qt::UserRole);
}

void EditableComboBox::addItems(const QStringList &texts)
{
    Q_D(EditableComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRows(row, texts.size());
    for (int i = 0; i < texts.size(); ++i) {
        d->m_model->setData(d->m_model->index(row + i, 0), texts[i], Qt::DisplayRole);
    }
}

void EditableComboBox::insertItem(int index, const QString &text, const QVariant &userData)
{
    Q_D(EditableComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(index, 0), userData, Qt::UserRole);
}

void EditableComboBox::insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userData)
{
    Q_D(EditableComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(index, 0), icon, Qt::DecorationRole);
    d->m_model->setData(d->m_model->index(index, 0), userData, Qt::UserRole);
}

void EditableComboBox::insertItems(int index, const QStringList &texts)
{
    Q_D(EditableComboBox);
    d->m_model->insertRows(index, texts.size());
    for (int i = 0; i < texts.size(); ++i) {
        d->m_model->setData(d->m_model->index(index + i, 0), texts[i], Qt::DisplayRole);
    }
}

void EditableComboBox::insertSeparator(int index)
{
    Q_D(EditableComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), true, ComboItemModel::SeparatorRole);
}

void EditableComboBox::removeItem(int index)
{
    Q_D(EditableComboBox);
    d->m_model->removeRow(index);
}

void EditableComboBox::clear()
{
    Q_D(EditableComboBox);
    d->m_model->removeRows(0, d->m_model->rowCount());
    setCurrentIndex(-1);
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

QVariant EditableComboBox::currentData(int role) const
{
    Q_D(const EditableComboBox);
    if (d->m_currentIndex >= 0 && d->m_currentIndex < d->m_model->rowCount()) {
        return d->m_model->data(d->m_model->index(d->m_currentIndex, 0), role);
    }
    return QVariant();
}

void EditableComboBox::setCurrentIndex(int index)
{
    Q_D(EditableComboBox);

    if (index < 0 || index >= d->m_model->rowCount()) {
        if (d->m_currentIndex == -1 && index == -1) {
            return;
        }
        d->m_currentIndex = -1;
        d->updateTextState();
        emit currentIndexChanged(-1);
        emit currentTextChanged(QString());
        return;
    }

    if (d->m_currentIndex == index) {
        return;
    }

    d->m_currentIndex = index;
    d->updateTextState();
    emit currentIndexChanged(index);
    emit currentTextChanged(d->m_model->data(d->m_model->index(index, 0), Qt::DisplayRole).toString());
}

void EditableComboBox::setCurrentText(const QString &text)
{
    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    } else {
        addItem(text);
        setCurrentIndex(count() - 1);
    }
}

int EditableComboBox::count() const
{
    Q_D(const EditableComboBox);
    return d->m_model->rowCount();
}

QString EditableComboBox::itemText(int index) const
{
    Q_D(const EditableComboBox);
    return d->m_model->data(d->m_model->index(index, 0), Qt::DisplayRole).toString();
}

QIcon EditableComboBox::itemIcon(int index) const
{
    Q_D(const EditableComboBox);
    return d->m_model->data(d->m_model->index(index, 0), Qt::DecorationRole).value<QIcon>();
}

QVariant EditableComboBox::itemData(int index, int role) const
{
    Q_D(const EditableComboBox);
    return d->m_model->data(d->m_model->index(index, 0), role);
}

void EditableComboBox::setItemText(int index, const QString &text)
{
    Q_D(EditableComboBox);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
    if (index == d->m_currentIndex) {
        d->updateTextState();
    }
}

void EditableComboBox::setItemIcon(int index, const QIcon &icon)
{
    Q_D(EditableComboBox);
    d->m_model->setData(d->m_model->index(index, 0), icon, Qt::DecorationRole);
}

void EditableComboBox::setItemData(int index, const QVariant &value, int role)
{
    Q_D(EditableComboBox);
    d->m_model->setData(d->m_model->index(index, 0), value, role);
}

int EditableComboBox::findText(const QString &text, Qt::MatchFlags flags) const
{
    Q_D(const EditableComboBox);
    for (int i = 0; i < d->m_model->rowCount(); ++i) {
        QString itemText = d->m_model->data(d->m_model->index(i, 0), Qt::DisplayRole).toString();
        if (flags & Qt::MatchExactly) {
            if (itemText == text)
                return i;
        } else {
            Qt::CaseSensitivity cs = (flags & Qt::MatchCaseSensitive) ? Qt::CaseSensitive
                                                                      : Qt::CaseInsensitive;
            if (itemText.compare(text, cs) == 0)
                return i;
        }
    }
    return -1;
}

int EditableComboBox::findData(const QVariant &data, int role, Qt::MatchFlags flags) const
{
    Q_D(const EditableComboBox);
    for (int i = 0; i < d->m_model->rowCount(); ++i) {
        QVariant v = d->m_model->data(d->m_model->index(i, 0), role);
        if (flags & Qt::MatchExactly) {
            if (v == data)
                return i;
        } else {
            if (v.toString().contains(data.toString()))
                return i;
        }
    }
    return -1;
}

void EditableComboBox::setPlaceholderText(const QString &text)
{
    Q_D(EditableComboBox);
    d->m_placeholderText = text;
    LineEdit::setPlaceholderText(text);
    if (d->m_currentIndex < 0) {
        d->updateTextState();
    }
}

QString EditableComboBox::placeholderText() const
{
    Q_D(const EditableComboBox);
    return d->m_placeholderText;
}

void EditableComboBox::setText(const QString &text)
{
    Q_D(EditableComboBox);
    d->m_settingCurrentIndex = true;
    LineEdit::setText(text);
    d->m_settingCurrentIndex = false;
}

void EditableComboBox::setCompleterMenu(CompleterMenu *menu)
{
    Q_D(EditableComboBox);

    LineEdit::setCompleterMenu(menu);
    connect(menu, &CompleterMenu::activated, d, &EditableComboBoxPrivate::onActivated);
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
