#include "MultiViewComboBox.h"
#include "Private/MultiViewComboBoxPrivate.h"

#include "StyleSheet.h"
#include "Animation.h"
#include "FluentIcon.h"

#include <QPainter>
#include <QMouseEvent>
#include <QAbstractItemModel>

MultiViewComboBox::MultiViewComboBox(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new MultiViewComboBoxPrivate(this))
{
    Q_D(MultiViewComboBox);

    setObjectName("MultiViewComboBox");
    setFixedHeight(33);
    setMenu(nullptr);

    d->m_arrowAni = new TranslateYAnimation(this);
    d->updateTextState();
    installEventFilter(this);

    StyleSheet::registerWidget(this, Fluent::ThemeStyle::COMBO_BOX);
}

MultiViewComboBox::~MultiViewComboBox() = default;

void MultiViewComboBox::setModel(QAbstractItemModel *model)
{
    Q_D(MultiViewComboBox);
    d->setModel(model);
}

QAbstractItemModel *MultiViewComboBox::model() const
{
    Q_D(const MultiViewComboBox);
    return d->m_model;
}

void MultiViewComboBox::addItem(const QString &text, const QVariant &userData)
{
    Q_D(MultiViewComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRow(row);
    d->m_model->setData(d->m_model->index(row, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(row, 0), userData, Qt::UserRole);
}

void MultiViewComboBox::addItem(const QIcon &icon, const QString &text, const QVariant &userData)
{
    Q_D(MultiViewComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRow(row);
    d->m_model->setData(d->m_model->index(row, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(row, 0), icon, Qt::DecorationRole);
    d->m_model->setData(d->m_model->index(row, 0), userData, Qt::UserRole);
}

void MultiViewComboBox::addItems(const QStringList &texts)
{
    Q_D(MultiViewComboBox);
    int row = d->m_model->rowCount();
    d->m_model->insertRows(row, texts.size());
    for (int i = 0; i < texts.size(); ++i) {
        d->m_model->setData(d->m_model->index(row + i, 0), texts[i], Qt::DisplayRole);
    }
}

void MultiViewComboBox::insertItem(int index, const QString &text, const QVariant &userData)
{
    Q_D(MultiViewComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(index, 0), userData, Qt::UserRole);
}

void MultiViewComboBox::insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userData)
{
    Q_D(MultiViewComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
    d->m_model->setData(d->m_model->index(index, 0), icon, Qt::DecorationRole);
    d->m_model->setData(d->m_model->index(index, 0), userData, Qt::UserRole);
}

void MultiViewComboBox::insertItems(int index, const QStringList &texts)
{
    Q_D(MultiViewComboBox);
    d->m_model->insertRows(index, texts.size());
    for (int i = 0; i < texts.size(); ++i) {
        d->m_model->setData(d->m_model->index(index + i, 0), texts[i], Qt::DisplayRole);
    }
}

void MultiViewComboBox::insertSeparator(int index)
{
    Q_D(MultiViewComboBox);
    d->m_model->insertRow(index);
    d->m_model->setData(d->m_model->index(index, 0), true, ComboItemModel::SeparatorRole);
}

void MultiViewComboBox::removeItem(int index)
{
    Q_D(MultiViewComboBox);
    d->m_model->removeRow(index);
}

void MultiViewComboBox::clear()
{
    Q_D(MultiViewComboBox);
    d->m_model->removeRows(0, d->m_model->rowCount());
    d->m_selectedIndexes.clear();
    d->updateTextState();
}

int MultiViewComboBox::count() const
{
    Q_D(const MultiViewComboBox);
    return d->m_model->rowCount();
}

QString MultiViewComboBox::itemText(int index) const
{
    Q_D(const MultiViewComboBox);
    return d->m_model->data(d->m_model->index(index, 0), Qt::DisplayRole).toString();
}

QIcon MultiViewComboBox::itemIcon(int index) const
{
    Q_D(const MultiViewComboBox);
    return d->m_model->data(d->m_model->index(index, 0), Qt::DecorationRole).value<QIcon>();
}

QVariant MultiViewComboBox::itemData(int index, int role) const
{
    Q_D(const MultiViewComboBox);
    return d->m_model->data(d->m_model->index(index, 0), role);
}

void MultiViewComboBox::setItemText(int index, const QString &text)
{
    Q_D(MultiViewComboBox);
    d->m_model->setData(d->m_model->index(index, 0), text, Qt::DisplayRole);
}

void MultiViewComboBox::setItemIcon(int index, const QIcon &icon)
{
    Q_D(MultiViewComboBox);
    d->m_model->setData(d->m_model->index(index, 0), icon, Qt::DecorationRole);
}

void MultiViewComboBox::setItemData(int index, const QVariant &value, int role)
{
    Q_D(MultiViewComboBox);
    d->m_model->setData(d->m_model->index(index, 0), value, role);
}

void MultiViewComboBox::setItemSelected(int index, bool selected)
{
    Q_D(MultiViewComboBox);

    if (index < 0 || index >= d->m_model->rowCount())
        return;

    if (selected) {
        if (d->m_maxSelectedCount > 0 && d->m_selectedIndexes.size() >= d->m_maxSelectedCount)
            return;
        if (!d->m_selectedIndexes.contains(index)) {
            d->m_selectedIndexes.append(index);
            std::sort(d->m_selectedIndexes.begin(), d->m_selectedIndexes.end());
            d->updateTextState();
            emit itemSelected(index);
            emit selectionChanged();
        }
    } else {
        if (d->m_selectedIndexes.removeOne(index)) {
            d->updateTextState();
            emit itemDeselected(index);
            emit selectionChanged();
        }
    }
}

bool MultiViewComboBox::isItemSelected(int index) const
{
    Q_D(const MultiViewComboBox);
    return d->m_selectedIndexes.contains(index);
}

QList<int> MultiViewComboBox::selectedIndexes() const
{
    Q_D(const MultiViewComboBox);
    return d->m_selectedIndexes;
}

QStringList MultiViewComboBox::selectedTexts() const
{
    Q_D(const MultiViewComboBox);
    QStringList texts;
    for (int idx : d->m_selectedIndexes) {
        if (idx >= 0 && idx < d->m_model->rowCount()) {
            texts << d->m_model->data(d->m_model->index(idx, 0), Qt::DisplayRole).toString();
        }
    }
    return texts;
}

QList<QVariant> MultiViewComboBox::selectedDatas() const
{
    Q_D(const MultiViewComboBox);
    QList<QVariant> datas;
    for (int idx : d->m_selectedIndexes) {
        if (idx >= 0 && idx < d->m_model->rowCount()) {
            datas << d->m_model->data(d->m_model->index(idx, 0), Qt::UserRole);
        }
    }
    return datas;
}

void MultiViewComboBox::setMaxSelectedCount(int count)
{
    Q_D(MultiViewComboBox);
    d->m_maxSelectedCount = count;
}

int MultiViewComboBox::maxSelectedCount() const
{
    Q_D(const MultiViewComboBox);
    return d->m_maxSelectedCount;
}

int MultiViewComboBox::findText(const QString &text, Qt::MatchFlags flags) const
{
    Q_D(const MultiViewComboBox);
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

int MultiViewComboBox::findData(const QVariant &data, int role, Qt::MatchFlags flags) const
{
    Q_D(const MultiViewComboBox);
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

void MultiViewComboBox::setPlaceholderText(const QString &text)
{
    Q_D(MultiViewComboBox);
    d->m_placeholderText = text;
    if (d->m_selectedIndexes.isEmpty()) {
        d->updateTextState();
    }
}

QString MultiViewComboBox::placeholderText() const
{
    Q_D(const MultiViewComboBox);
    return d->m_placeholderText;
}

void MultiViewComboBox::setText(const QString &text)
{
    Q_D(MultiViewComboBox);
    d->m_settingCurrentIndex = true;
    QPushButton::setText(text);
    d->m_settingCurrentIndex = false;
}

void MultiViewComboBox::setMaxVisibleItems(int count)
{
    Q_D(MultiViewComboBox);
    d->m_maxVisibleItems = count;
}

int MultiViewComboBox::maxVisibleItems() const
{
    Q_D(const MultiViewComboBox);
    return d->m_maxVisibleItems;
}

void MultiViewComboBox::paintEvent(QPaintEvent *event)
{
    Q_D(MultiViewComboBox);
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

void MultiViewComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(MultiViewComboBox);

    if (d->m_isPressed) {
        d->m_isPressed = false;
        update();
        d->toggleComboMenu();
    }

    QPushButton::mouseReleaseEvent(event);
}

bool MultiViewComboBox::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(MultiViewComboBox);

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
