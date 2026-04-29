#include "ComboBoxPrivate.h"
#include "QFluent/ComboBox.h"

#include "Animation.h"

#include <QAbstractItemModel>
#include <QAction>
#include <QPointer>
#include <QCursor>

ComboBoxPrivate::ComboBoxPrivate(ComboBox *q)
    : QObject(q)
    , q_ptr(q)
{
    m_internalModel = new ComboItemModel(this);
    m_model = m_internalModel;
    connectModel(m_model);
}

ComboBoxPrivate::~ComboBoxPrivate() = default;

void ComboBoxPrivate::setModel(QAbstractItemModel *model)
{
    Q_Q(ComboBox);

    if (model == m_model)
        return;

    if (!model) {
        model = m_internalModel;
    }

    disconnectModel(m_model);
    m_model = model;
    connectModel(m_model);

    m_comboMenu = nullptr;

    int oldIndex = m_currentIndex;
    m_currentIndex = -1;
    q->setCurrentIndex(-1);

    if (oldIndex != -1) {
        emit q->currentIndexChanged(-1);
        emit q->currentTextChanged(QString());
    }
}

void ComboBoxPrivate::connectModel(QAbstractItemModel *model)
{
    connect(model, &QAbstractItemModel::rowsInserted, this, &ComboBoxPrivate::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &ComboBoxPrivate::onRowsRemoved);
    connect(model, &QAbstractItemModel::modelReset, this, &ComboBoxPrivate::onModelReset);
    connect(model, &QAbstractItemModel::dataChanged, this, &ComboBoxPrivate::onDataChanged);
}

void ComboBoxPrivate::disconnectModel(QAbstractItemModel *model)
{
    disconnect(model, &QAbstractItemModel::rowsInserted, this, &ComboBoxPrivate::onRowsInserted);
    disconnect(model, &QAbstractItemModel::rowsRemoved, this, &ComboBoxPrivate::onRowsRemoved);
    disconnect(model, &QAbstractItemModel::modelReset, this, &ComboBoxPrivate::onModelReset);
    disconnect(model, &QAbstractItemModel::dataChanged, this, &ComboBoxPrivate::onDataChanged);
}

void ComboBoxPrivate::createComboMenu()
{
    Q_Q(ComboBox);

    if (m_comboMenu) {
        m_comboMenu->close();
        m_comboMenu = nullptr;
    }

    m_comboMenu = new ComboBoxMenu("menu", q);

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
        connect(action, &QAction::triggered, this, [this, i]() { onMenuAction(i); });
    }

    QPointer<ComboBox> qPtr = q;
    connect(m_comboMenu, &ComboBoxMenu::closed, q, [qPtr, this]() {
        if (!qPtr) return;
        QPoint pos = qPtr->mapFromGlobal(QCursor::pos());
        if (!qPtr->rect().contains(pos)) {
            m_comboMenu = nullptr;
        }
    });
}

void ComboBoxPrivate::showComboMenu()
{
    Q_Q(ComboBox);

    if (m_model->rowCount() == 0) {
        return;
    }

    createComboMenu();

    if (m_currentIndex >= 0 && m_currentIndex < m_model->rowCount()) {
        for (QAction *action : m_comboMenu->menuActions()) {
            if (action->data().toInt() == m_currentIndex) {
                m_comboMenu->setDefaultAction(action);
                break;
            }
        }
    }

    ComboBoxHelper::showComboMenu(q, m_comboMenu, m_maxVisibleItems);
}

void ComboBoxPrivate::closeComboMenu()
{
    if (!m_comboMenu) {
        return;
    }
    m_comboMenu->close();
    m_comboMenu = nullptr;
}

void ComboBoxPrivate::toggleComboMenu()
{
    if (m_comboMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void ComboBoxPrivate::updateTextState()
{
    Q_Q(ComboBox);

    if (m_currentIndex >= 0 && m_currentIndex < m_model->rowCount()) {
        QModelIndex index = m_model->index(m_currentIndex, 0);
        QString text = m_model->data(index, Qt::DisplayRole).toString();
        m_settingCurrentIndex = true;
        q->setText(text);
        m_settingCurrentIndex = false;
    } else {
        m_settingCurrentIndex = true;
        q->setText(m_placeholderText);
        m_settingCurrentIndex = false;
    }
}

void ComboBoxPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_Q(ComboBox);

    if (m_currentIndex < 0)
        return;

    if (first <= m_currentIndex) {
        int count = last - first + 1;
        m_currentIndex += count;
        emit q->currentIndexChanged(m_currentIndex);
    }
}

void ComboBoxPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_Q(ComboBox);

    if (m_currentIndex < 0)
        return;

    int count = last - first + 1;
    if (m_currentIndex >= first && m_currentIndex <= last) {
        m_currentIndex = -1;
        updateTextState();
        emit q->currentIndexChanged(-1);
        emit q->currentTextChanged(QString());
    } else if (m_currentIndex > last) {
        m_currentIndex -= count;
        emit q->currentIndexChanged(m_currentIndex);
    }
}

void ComboBoxPrivate::onModelReset()
{
    Q_Q(ComboBox);

    m_currentIndex = -1;
    updateTextState();
    emit q->currentIndexChanged(-1);
    emit q->currentTextChanged(QString());
}

void ComboBoxPrivate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_Q(ComboBox);

    if (m_currentIndex >= topLeft.row() && m_currentIndex <= bottomRight.row()) {
        updateTextState();
        QModelIndex index = m_model->index(m_currentIndex, 0);
        emit q->currentTextChanged(m_model->data(index, Qt::DisplayRole).toString());
    }
}

void ComboBoxPrivate::onMenuAction(int index)
{
    Q_Q(ComboBox);
    q->setCurrentIndex(index);
    closeComboMenu();
}