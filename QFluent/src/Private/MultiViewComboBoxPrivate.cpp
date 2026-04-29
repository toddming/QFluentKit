#include "MultiViewComboBoxPrivate.h"
#include "QFluent/MultiViewComboBox.h"

#include <QStyle>
#include <QCursor>
#include <QAction>
#include <QPointer>
#include <QAbstractItemModel>
#include <algorithm>

MultiViewComboBoxPrivate::MultiViewComboBoxPrivate(MultiViewComboBox *q)
    : QObject(q)
    , q_ptr(q)
    , m_maxSelectedCount(-1)
    , m_maxVisibleItems(-1)
    , m_isPressed(false)
    , m_isHover(false)
{
    m_internalModel = new ComboItemModel(this);
    m_model = m_internalModel;
    connectModel(m_model);
}

MultiViewComboBoxPrivate::~MultiViewComboBoxPrivate() = default;

void MultiViewComboBoxPrivate::setModel(QAbstractItemModel *model)
{
    Q_Q(MultiViewComboBox);

    if (model == m_model)
        return;

    if (!model) {
        model = m_internalModel;
    }

    disconnectModel(m_model);
    m_model = model;
    connectModel(m_model);

    m_comboMenu = nullptr;
    m_selectedIndexes.clear();

    updateTextState();
    emit q->currentIndexChanged(-1);
    emit q->currentTextChanged(QString());
}

void MultiViewComboBoxPrivate::connectModel(QAbstractItemModel *model)
{
    connect(model, &QAbstractItemModel::rowsInserted, this, &MultiViewComboBoxPrivate::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &MultiViewComboBoxPrivate::onRowsRemoved);
    connect(model, &QAbstractItemModel::modelReset, this, &MultiViewComboBoxPrivate::onModelReset);
    connect(model, &QAbstractItemModel::dataChanged, this, &MultiViewComboBoxPrivate::onDataChanged);
}

void MultiViewComboBoxPrivate::disconnectModel(QAbstractItemModel *model)
{
    disconnect(model, &QAbstractItemModel::rowsInserted, this, &MultiViewComboBoxPrivate::onRowsInserted);
    disconnect(model, &QAbstractItemModel::rowsRemoved, this, &MultiViewComboBoxPrivate::onRowsRemoved);
    disconnect(model, &QAbstractItemModel::modelReset, this, &MultiViewComboBoxPrivate::onModelReset);
    disconnect(model, &QAbstractItemModel::dataChanged, this, &MultiViewComboBoxPrivate::onDataChanged);
}

void MultiViewComboBoxPrivate::createComboMenu()
{
    Q_Q(MultiViewComboBox);

    if (m_comboMenu) {
        m_comboMenu->close();
        m_comboMenu = nullptr;
    }

    m_comboMenu = new MultiViewComboBoxMenu("menu", q);

    for (int i = 0; i < m_model->rowCount(); ++i) {
        QModelIndex index = m_model->index(i, 0);
        bool isSep = m_model->data(index, ComboItemModel::SeparatorRole).toBool();

        if (isSep) {
            m_comboMenu->addSeparator();
            continue;
        }

        QString text = m_model->data(index, Qt::DisplayRole).toString();
        QIcon icon = m_model->data(index, Qt::DecorationRole).value<QIcon>();
        QAction *action = new QAction(icon, text, m_comboMenu);
        if (icon.isNull()) {
            action = new QAction(text, m_comboMenu);
        }
        m_comboMenu->addAction(action);
        action->setData(i);
        action->setCheckable(true);
        if (m_selectedIndexes.contains(i)) {
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this, [this, i](bool checked) { onMenuAction(i, checked); });
    }

    QPointer<MultiViewComboBox> qPtr = q;
    connect(m_comboMenu, &MultiViewComboBoxMenu::closed, q, [qPtr, this]() {
        if (!qPtr) return;
        QPoint pos = qPtr->mapFromGlobal(QCursor::pos());
        if (!qPtr->rect().contains(pos)) {
            m_comboMenu = nullptr;
        }
    });
}

void MultiViewComboBoxPrivate::showComboMenu()
{
    Q_Q(MultiViewComboBox);

    if (m_model->rowCount() == 0) {
        return;
    }

    createComboMenu();
    ComboBoxHelper::showComboMenu(q, m_comboMenu, m_maxVisibleItems);
}

void MultiViewComboBoxPrivate::closeComboMenu()
{
    if (!m_comboMenu) {
        return;
    }
    m_comboMenu->close();
    m_comboMenu = nullptr;
}

void MultiViewComboBoxPrivate::toggleComboMenu()
{
    if (m_comboMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void MultiViewComboBoxPrivate::updateTextState()
{
    Q_Q(MultiViewComboBox);

    if (m_selectedIndexes.isEmpty()) {
        m_settingCurrentIndex = true;
        q->setText(m_placeholderText);
        m_settingCurrentIndex = false;
    } else {
        QStringList texts;
        for (int idx : m_selectedIndexes) {
            if (idx >= 0 && idx < m_model->rowCount()) {
                QModelIndex modelIndex = m_model->index(idx, 0);
                texts << m_model->data(modelIndex, Qt::DisplayRole).toString();
            }
        }
        m_settingCurrentIndex = true;
        q->setText(texts.join(", "));
        m_settingCurrentIndex = false;
    }
}

void MultiViewComboBoxPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_Q(MultiViewComboBox);

    int count = last - first + 1;
    for (int i = 0; i < m_selectedIndexes.size(); ++i) {
        if (m_selectedIndexes[i] >= first) {
            m_selectedIndexes[i] += count;
        }
    }
    updateTextState();
}

void MultiViewComboBoxPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_Q(MultiViewComboBox);

    int count = last - first + 1;
    bool changed = false;
    QList<int> newSelected;
    for (int idx : m_selectedIndexes) {
        if (idx >= first && idx <= last) {
            changed = true;
        } else if (idx > last) {
            newSelected << idx - count;
        } else {
            newSelected << idx;
        }
    }
    if (changed) {
        m_selectedIndexes = newSelected;
        updateTextState();
        emit q->selectionChanged();
    }
}

void MultiViewComboBoxPrivate::onModelReset()
{
    Q_Q(MultiViewComboBox);

    m_selectedIndexes.clear();
    updateTextState();
    emit q->currentIndexChanged(-1);
    emit q->currentTextChanged(QString());
    emit q->selectionChanged();
}

void MultiViewComboBoxPrivate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);

    bool affected = false;
    for (int idx : m_selectedIndexes) {
        if (idx >= topLeft.row() && idx <= bottomRight.row()) {
            affected = true;
            break;
        }
    }
    if (affected) {
        updateTextState();
    }
}

void MultiViewComboBoxPrivate::onMenuAction(int index, bool checked)
{
    Q_Q(MultiViewComboBox);

    if (checked) {
        if (m_maxSelectedCount > 0 && m_selectedIndexes.size() >= m_maxSelectedCount) {
            if (m_comboMenu) {
                QList<QAction *> actions = m_comboMenu->actions();
                if (index < actions.size()) {
                    actions[index]->setChecked(false);
                }
            }
            return;
        }
        if (!m_selectedIndexes.contains(index)) {
            m_selectedIndexes.append(index);
            std::sort(m_selectedIndexes.begin(), m_selectedIndexes.end());
            updateTextState();
            emit q->itemSelected(index);
            emit q->selectionChanged();
        }
    } else {
        if (m_selectedIndexes.removeOne(index)) {
            updateTextState();
            emit q->itemDeselected(index);
            emit q->selectionChanged();
        }
    }
}
