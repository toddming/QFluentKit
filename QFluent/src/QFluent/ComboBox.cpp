#include "ComboBox.h"
#include "Private/ComboBoxPrivate.h"

#include "StyleSheet.h"
#include "Animation.h"
#include "FluentIcon.h"

#include <QPainter>
#include <QMouseEvent>
#include <QAbstractItemModel>

ComboBox::ComboBox(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new ComboBoxPrivate(this))
{
    Q_D(ComboBox);

    setObjectName("ComboBox");
    setFixedHeight(33);
    setMenu(nullptr);

    d->m_arrowAni = new TranslateYAnimation(this);
    d->updateTextState();
    installEventFilter(this);

    StyleSheet::registerWidget(this, Fluent::ThemeStyle::COMBO_BOX);
}

ComboBox::~ComboBox() = default;

void ComboBox::setModel(QAbstractItemModel *model)
{
    Q_D(ComboBox);
    d->setModel(model);
}

QAbstractItemModel *ComboBox::model() const
{
    Q_D(const ComboBox);
    return d->m_model;
}

void ComboBox::addItem(const QString &text, const QVariant &userData)
{
    Q_D(ComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRow(row);
    d->m_model->setData(d->m_model->index(row, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(row, 0), userData, Qt::UserRole);
}

void ComboBox::addItem(const QIcon &icon, const QString &text, const QVariant &userData)
{
    Q_D(ComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRow(row);
    d->m_model->setData(d->m_model->index(row, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(row, 0), icon, Qt::DecorationRole);
    d->m_model->setData(d->m_model->index(row, 0), userData, Qt::UserRole);
}

void ComboBox::addItems(const QStringList &texts)
{
    Q_D(ComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRows(row, texts.size());
    for (int i = 0; i < texts.size(); ++i) {
        d->m_model->setData(d->m_model->index(row + i, 0), texts[i], Qt::DisplayRole);
    }
}

void ComboBox::insertItem(int index, const QString &text, const QVariant &userData)
{
    Q_D(ComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(index, 0), userData, Qt::UserRole);
}

void ComboBox::insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userData)
{
    Q_D(ComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(index, 0), icon, Qt::DecorationRole);
    d->m_model->setData(d->m_model->index(index, 0), userData, Qt::UserRole);
}

void ComboBox::insertItems(int index, const QStringList &texts)
{
    Q_D(ComboBox);
    d->m_model->insertRows(index, texts.size());
    for (int i = 0; i < texts.size(); ++i) {
        d->m_model->setData(d->m_model->index(index + i, 0), texts[i], Qt::DisplayRole);
    }
}

void ComboBox::insertSeparator(int index)
{
    Q_D(ComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), true, ComboItemModel::SeparatorRole);
}

void ComboBox::removeItem(int index)
{
    Q_D(ComboBox);
    d->m_model->removeRow(index);
}

void ComboBox::clear()
{
    Q_D(ComboBox);
    d->m_model->removeRows(0, d->m_model->rowCount());
    setCurrentIndex(-1);
}

int ComboBox::currentIndex() const
{
    Q_D(const ComboBox);
    return d->m_currentIndex;
}

QString ComboBox::currentText() const
{
    Q_D(const ComboBox);
    if (d->m_currentIndex >= 0 && d->m_currentIndex < d->m_model->rowCount()) {
        return d->m_model->data(d->m_model->index(d->m_currentIndex, 0), Qt::DisplayRole).toString();
    }
    return QString();
}

QVariant ComboBox::currentData(int role) const
{
    Q_D(const ComboBox);
    if (d->m_currentIndex >= 0 && d->m_currentIndex < d->m_model->rowCount()) {
        return d->m_model->data(d->m_model->index(d->m_currentIndex, 0), role);
    }
    return QVariant();
}

void ComboBox::setCurrentIndex(int index)
{
    Q_D(ComboBox);

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

void ComboBox::setCurrentText(const QString &text)
{
    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    } else {
        addItem(text);
        setCurrentIndex(count() - 1);
    }
}

int ComboBox::count() const
{
    Q_D(const ComboBox);
    return d->m_model->rowCount();
}

QString ComboBox::itemText(int index) const
{
    Q_D(const ComboBox);
    return d->m_model->data(d->m_model->index(index, 0), Qt::DisplayRole).toString();
}

QIcon ComboBox::itemIcon(int index) const
{
    Q_D(const ComboBox);
    return d->m_model->data(d->m_model->index(index, 0), Qt::DecorationRole).value<QIcon>();
}

QVariant ComboBox::itemData(int index, int role) const
{
    Q_D(const ComboBox);
    return d->m_model->data(d->m_model->index(index, 0), role);
}

void ComboBox::setItemText(int index, const QString &text)
{
    Q_D(ComboBox);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
    if (index == d->m_currentIndex) {
        d->updateTextState();
    }
}

void ComboBox::setItemIcon(int index, const QIcon &icon)
{
    Q_D(ComboBox);
    d->m_model->setData(d->m_model->index(index, 0), icon, Qt::DecorationRole);
}

void ComboBox::setItemData(int index, const QVariant &value, int role)
{
    Q_D(ComboBox);
    d->m_model->setData(d->m_model->index(index, 0), value, role);
}

int ComboBox::findText(const QString &text, Qt::MatchFlags flags) const
{
    Q_D(const ComboBox);
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

int ComboBox::findData(const QVariant &data, int role, Qt::MatchFlags flags) const
{
    Q_D(const ComboBox);
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

void ComboBox::setPlaceholderText(const QString &text)
{
    Q_D(ComboBox);
    d->m_placeholderText = text;
    if (d->m_currentIndex < 0) {
        d->updateTextState();
    }
}

QString ComboBox::placeholderText() const
{
    Q_D(const ComboBox);
    return d->m_placeholderText;
}

void ComboBox::setText(const QString &text)
{
    Q_D(ComboBox);
    d->m_settingCurrentIndex = true;
    QPushButton::setText(text);
    d->m_settingCurrentIndex = false;
}

void ComboBox::setMaxVisibleItems(int count)
{
    Q_D(ComboBox);
    d->m_maxVisibleItems = count;
}

int ComboBox::maxVisibleItems() const
{
    Q_D(const ComboBox);
    return d->m_maxVisibleItems;
}

void ComboBox::paintEvent(QPaintEvent *event)
{
    Q_D(ComboBox);
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (d->m_isHover) {
        painter.setOpacity(0.8);
    } else if (d->m_isPressed) {
        painter.setOpacity(0.7);
    }

    QRectF rect = QRectF(width() - 22, height() / 2 - 5 + d->m_arrowAni->y(), 10, 10);
    FluentIconUtils::drawThemeIcon(Fluent::IconType::ARROW_DOWN, &painter, rect);
}

void ComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(ComboBox);

    if (d->m_isPressed) {
        d->m_isPressed = false;
        update();
        d->toggleComboMenu();
    }

    QPushButton::mouseReleaseEvent(event);
}

bool ComboBox::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(ComboBox);

    if (watched == this) {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            d->m_isPressed = true;
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
    return QPushButton::eventFilter(watched, event);
}