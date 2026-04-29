#include "ComboItemModel.h"

#include <QRegularExpression>

ComboItemModel::ComboItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ComboItemModel::~ComboItemModel() = default;

int ComboItemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_items.count();
}

QVariant ComboItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.count())
        return QVariant();

    const ComboItem &item = m_items[index.row()];
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return item.text;
    case Qt::DecorationRole:
        return item.icon;
    case Qt::UserRole:
        return item.userData;
    case SeparatorRole:
        return item.isSeparator;
    default:
        return QVariant();
    }
}

bool ComboItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.count())
        return false;

    ComboItem &item = m_items[index.row()];
    bool changed = false;

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (item.text != value.toString()) {
            item.text = value.toString();
            changed = true;
        }
        break;
    case Qt::DecorationRole:
        item.icon = value.value<QIcon>();
        changed = true;
        break;
    case Qt::UserRole:
        if (item.userData != value) {
            item.userData = value;
            changed = true;
        }
        break;
    case SeparatorRole:
        if (item.isSeparator != value.toBool()) {
            item.isSeparator = value.toBool();
            changed = true;
        }
        break;
    default:
        return false;
    }

    if (changed)
        emit dataChanged(index, index, {role});
    return true;
}

Qt::ItemFlags ComboItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.row() >= 0 && index.row() < m_items.count() && m_items[index.row()].isSeparator)
        f &= ~Qt::ItemIsSelectable;
    return f;
}

bool ComboItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || count <= 0 || row < 0 || row > m_items.count())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_items.insert(row, ComboItem());
    endInsertRows();
    return true;
}

bool ComboItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || count <= 0 || row < 0 || row + count > m_items.count())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_items.removeAt(row);
    endRemoveRows();
    return true;
}

// Convenience API

void ComboItemModel::addItem(const QString &text, const QVariant &userData)
{
    insertItem(m_items.count(), text, userData);
}

void ComboItemModel::addItem(const QIcon &icon, const QString &text, const QVariant &userData)
{
    insertItem(m_items.count(), icon, text, userData);
}

void ComboItemModel::addItems(const QStringList &texts)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count() + texts.size() - 1);
    for (const auto &text : texts)
        m_items.append(ComboItem(text));
    endInsertRows();
}

void ComboItemModel::insertItem(int index, const QString &text, const QVariant &userData)
{
    if (index < 0 || index > m_items.count())
        index = m_items.count();

    beginInsertRows(QModelIndex(), index, index);
    m_items.insert(index, ComboItem(text, QIcon(), userData));
    endInsertRows();
}

void ComboItemModel::insertItem(int index, const QIcon &icon, const QString &text,
                                const QVariant &userData)
{
    if (index < 0 || index > m_items.count())
        index = m_items.count();

    beginInsertRows(QModelIndex(), index, index);
    m_items.insert(index, ComboItem(text, icon, userData));
    endInsertRows();
}

void ComboItemModel::insertItems(int index, const QStringList &texts)
{
    if (index < 0 || index > m_items.count())
        index = m_items.count();

    beginInsertRows(QModelIndex(), index, index + texts.size() - 1);
    for (int i = 0; i < texts.size(); ++i)
        m_items.insert(index + i, ComboItem(texts[i]));
    endInsertRows();
}

void ComboItemModel::insertSeparator(int index)
{
    if (index < 0 || index > m_items.count())
        index = m_items.count();

    beginInsertRows(QModelIndex(), index, index);
    ComboItem item;
    item.isSeparator = true;
    m_items.insert(index, item);
    endInsertRows();
}

void ComboItemModel::removeItem(int index)
{
    removeRows(index, 1);
}

void ComboItemModel::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

int ComboItemModel::count() const
{
    return m_items.count();
}

QString ComboItemModel::itemText(int index) const
{
    if (index < 0 || index >= m_items.count())
        return QString();
    return m_items[index].text;
}

QIcon ComboItemModel::itemIcon(int index) const
{
    if (index < 0 || index >= m_items.count())
        return QIcon();
    return m_items[index].icon;
}

QVariant ComboItemModel::itemData(int index) const
{
    if (index < 0 || index >= m_items.count())
        return QVariant();
    return m_items[index].userData;
}

bool ComboItemModel::isSeparator(int index) const
{
    if (index < 0 || index >= m_items.count())
        return false;
    return m_items[index].isSeparator;
}

void ComboItemModel::setItemText(int index, const QString &text)
{
    setData(this->index(index), text, Qt::DisplayRole);
}

void ComboItemModel::setItemIcon(int index, const QIcon &icon)
{
    setData(this->index(index), icon, Qt::DecorationRole);
}

void ComboItemModel::setItemData(int index, const QVariant &value)
{
    setData(this->index(index), value, Qt::UserRole);
}

int ComboItemModel::findText(const QString &text, Qt::MatchFlags flags) const
{
    if (flags & Qt::MatchExactly) {
        for (int i = 0; i < m_items.count(); ++i) {
            if (m_items[i].text == text)
                return i;
        }
    } else {
        Qt::CaseSensitivity cs = (flags & Qt::MatchCaseSensitive) ? Qt::CaseSensitive
                                                                  : Qt::CaseInsensitive;
        for (int i = 0; i < m_items.count(); ++i) {
            if (m_items[i].text.compare(text, cs) == 0)
                return i;
        }
    }
    return -1;
}

int ComboItemModel::findData(const QVariant &data, Qt::MatchFlags flags) const
{
    for (int i = 0; i < m_items.count(); ++i) {
        if (flags & Qt::MatchExactly) {
            if (m_items[i].userData == data)
                return i;
        } else if (flags & Qt::MatchRegularExpression) {
            QRegularExpression re(data.toString());
            if (m_items[i].userData.toString().contains(re))
                return i;
        } else {
            if (m_items[i].userData.toString().contains(data.toString()))
                return i;
        }
    }
    return -1;
}