#include "EditableComboBoxPrivate.h"
#include "QFluent/EditableComboBox.h"

#include "Animation.h"
#include "QFluent/LineEdit.h"

#include <QAbstractItemModel>
#include <QAction>
#include <QPointer>
#include <QCursor>

EditableComboBoxPrivate::EditableComboBoxPrivate(EditableComboBox *q)
    : QObject(q)
    , q_ptr(q)
{
    m_internalModel = new ComboItemModel(this);
    m_model = m_internalModel;
    connectModel(m_model);
}

EditableComboBoxPrivate::~EditableComboBoxPrivate() = default;

void EditableComboBoxPrivate::setModel(QAbstractItemModel *model)
{
    Q_Q(EditableComboBox);

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

void EditableComboBoxPrivate::connectModel(QAbstractItemModel *model)
{
    connect(model, &QAbstractItemModel::rowsInserted, this, &EditableComboBoxPrivate::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &EditableComboBoxPrivate::onRowsRemoved);
    connect(model, &QAbstractItemModel::modelReset, this, &EditableComboBoxPrivate::onModelReset);
    connect(model, &QAbstractItemModel::dataChanged, this, &EditableComboBoxPrivate::onDataChanged);
}

void EditableComboBoxPrivate::disconnectModel(QAbstractItemModel *model)
{
    disconnect(model, &QAbstractItemModel::rowsInserted, this, &EditableComboBoxPrivate::onRowsInserted);
    disconnect(model, &QAbstractItemModel::rowsRemoved, this, &EditableComboBoxPrivate::onRowsRemoved);
    disconnect(model, &QAbstractItemModel::modelReset, this, &EditableComboBoxPrivate::onModelReset);
    disconnect(model, &QAbstractItemModel::dataChanged, this, &EditableComboBoxPrivate::onDataChanged);
}

void EditableComboBoxPrivate::createComboMenu()
{
    Q_Q(EditableComboBox);

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

    QPointer<EditableComboBox> qPtr = q;
    connect(m_comboMenu, &ComboBoxMenu::closed, q, [qPtr, this]() {
        if (!qPtr) return;
        QPoint pos = qPtr->mapFromGlobal(QCursor::pos());
        if (!qPtr->rect().contains(pos)) {
            m_comboMenu = nullptr;
        }
    });
}

void EditableComboBoxPrivate::showComboMenu()
{
    Q_Q(EditableComboBox);

    if (m_model->rowCount() == 0) {
        return;
    }

    createComboMenu();

    if (q->currentIndex() >= 0 && q->currentIndex() < m_model->rowCount()) {
        for (QAction *action : m_comboMenu->menuActions()) {
            if (action->data().toInt() == q->currentIndex()) {
                m_comboMenu->setDefaultAction(action);
                break;
            }
        }
    }

    ComboBoxHelper::showComboMenu(q, m_comboMenu, m_maxVisibleItems);
}

void EditableComboBoxPrivate::closeComboMenu()
{
    if (!m_comboMenu) {
        return;
    }
    m_comboMenu->close();
    m_comboMenu = nullptr;
}

void EditableComboBoxPrivate::toggleComboMenu()
{
    if (m_comboMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void EditableComboBoxPrivate::updateTextState()
{
    Q_Q(EditableComboBox);

    if (m_currentIndex >= 0 && m_currentIndex < m_model->rowCount()) {
        QModelIndex index = m_model->index(m_currentIndex, 0);
        QString text = m_model->data(index, Qt::DisplayRole).toString();
        m_settingCurrentIndex = true;
        q->setText(text);
        m_settingCurrentIndex = false;
    } else {
        m_settingCurrentIndex = true;
        q->setText(QString());
        m_settingCurrentIndex = false;
    }
}

void EditableComboBoxPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_Q(EditableComboBox);

    if (m_currentIndex < 0)
        return;

    if (first <= m_currentIndex) {
        int count = last - first + 1;
        m_currentIndex += count;
        emit q->currentIndexChanged(m_currentIndex);
    }
}

void EditableComboBoxPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_Q(EditableComboBox);

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

void EditableComboBoxPrivate::onModelReset()
{
    Q_Q(EditableComboBox);

    m_currentIndex = -1;
    updateTextState();
    emit q->currentIndexChanged(-1);
    emit q->currentTextChanged(QString());
}

void EditableComboBoxPrivate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_Q(EditableComboBox);

    if (m_currentIndex >= topLeft.row() && m_currentIndex <= bottomRight.row()) {
        updateTextState();
        QModelIndex index = m_model->index(m_currentIndex, 0);
        emit q->currentTextChanged(m_model->data(index, Qt::DisplayRole).toString());
    }
}

void EditableComboBoxPrivate::onMenuAction(int index)
{
    Q_Q(EditableComboBox);
    q->setCurrentIndex(index);
    closeComboMenu();
}

void EditableComboBoxPrivate::onComboTextChanged(const QString &text)
{
    Q_Q(EditableComboBox);

    if (m_settingCurrentIndex)
        return;

    m_currentIndex = -1;

    for (int i = 0; i < m_model->rowCount(); ++i) {
        if (m_model->data(m_model->index(i, 0), Qt::DisplayRole).toString() == text) {
            m_currentIndex = i;
            break;
        }
    }

    emit q->currentIndexChanged(m_currentIndex);
    emit q->currentTextChanged(text);
}

void EditableComboBoxPrivate::onClearButtonClicked()
{
    Q_Q(EditableComboBox);
    q->setCurrentIndex(-1);
}

void EditableComboBoxPrivate::onReturnPressed()
{
    Q_Q(EditableComboBox);

    QString text = q->text();
    if (text.isEmpty())
        return;

    for (int i = 0; i < m_model->rowCount(); ++i) {
        if (m_model->data(m_model->index(i, 0), Qt::DisplayRole).toString() == text) {
            q->setCurrentIndex(i);
            return;
        }
    }

    q->addItem(text);
    q->setCurrentIndex(m_model->rowCount() - 1);
}

void EditableComboBoxPrivate::onActivated(const QString &text)
{
    Q_Q(EditableComboBox);

    for (int i = 0; i < m_model->rowCount(); ++i) {
        if (m_model->data(m_model->index(i, 0), Qt::DisplayRole).toString() == text) {
            q->setCurrentIndex(i);
            return;
        }
    }
}