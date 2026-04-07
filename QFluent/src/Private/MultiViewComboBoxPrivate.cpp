#include "MultiViewComboBoxPrivate.h"
#include "QFluent/MultiViewComboBox.h"
#include "QFluent/Menu/MultiViewComboBoxMenu.h"
#include "QFluent/Menu/MenuActionListWidget.h"
#include "FluentGlobal.h"

#include <QStyle>
#include <QCursor>
#include <QPointer>

MultiViewComboBoxPrivate::MultiViewComboBoxPrivate(MultiViewComboBox *parent)
    : QObject(parent)
    , q_ptr(parent)
{
}

void MultiViewComboBoxPrivate::updateText()
{
    Q_Q(MultiViewComboBox);

    if (m_selectedIndexes.isEmpty()) {
        q->setText(m_placeholderText);
        q->setProperty("isPlaceholderText", true);
    } else {
        QStringList texts;
        for (int index : m_selectedIndexes) {
            texts.append(m_items[index].text);
        }
        q->setText(texts.join(", "));
        q->setProperty("isPlaceholderText", false);
    }

    q->style()->unpolish(q);
    q->style()->polish(q);
    q->adjustSize();
}

MultiViewComboBoxMenu* MultiViewComboBoxPrivate::createComboMenu()
{
    Q_Q(MultiViewComboBox);

    MultiViewComboBoxMenu *menu = new MultiViewComboBoxMenu("menu", q);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QPointer<MultiViewComboBox> qPtr = q;

    for (int i = 0; i < q->count(); ++i) {
        QAction *action = new QAction(m_items[i].icon, m_items[i].text, menu);
        action->setCheckable(true);
        action->setChecked(m_selectedIndexes.contains(i));
        menu->addAction(action);
        connect(action, &QAction::triggered, q, [this, i](bool checked) {
            handleCheckBoxToggled(i, checked);
        });
    }

    connect(menu, &MultiViewComboBoxMenu::closed, q, [qPtr, this]() {
        if (!qPtr) return;
        QPoint pos = qPtr->mapFromGlobal(QCursor::pos());
        if (!qPtr->rect().contains(pos)) {
            m_dropMenu = nullptr;
        }
    });

    // 如果 m_maxVisibleItems > 0，设置最大高度（假设每个项高度约 30px，您可调整）
    if (m_maxVisibleItems > 0) {
        int itemHeight = 30;  // 假设每个 CheckBox 高度
        menu->setMaximumHeight(m_maxVisibleItems * itemHeight + menu->layout()->contentsMargins().top() + menu->layout()->contentsMargins().bottom());
    }

    return menu;
}

void MultiViewComboBoxPrivate::showComboMenu()
{
    Q_Q(MultiViewComboBox);

    if (q->count() == 0) {
        return;
    }

    m_dropMenu = createComboMenu();

    if (m_dropMenu->view()->width() < q->width()) {
        m_dropMenu->view()->setMinimumWidth(q->width());
        m_dropMenu->adjustMenuSize();
    }

    int x = -m_dropMenu->width() / 2 + m_dropMenu->layout()->contentsMargins().left() + q->width() / 2;
    QPoint pd = q->mapToGlobal(QPoint(x, q->height()));
    int hd = m_dropMenu->view()->heightForAnimation(pd, Fluent::MenuAnimation::DROP_DOWN);

    QPoint pu = q->mapToGlobal(QPoint(x, 0));
    int hu = m_dropMenu->view()->heightForAnimation(pu, Fluent::MenuAnimation::PULL_UP);

    if (hd >= hu) {
        m_dropMenu->view()->adjustSize(pd, Fluent::MenuAnimation::DROP_DOWN);
        m_dropMenu->exec(pd, true, Fluent::MenuAnimation::DROP_DOWN);
    } else {
        m_dropMenu->view()->adjustSize(pu, Fluent::MenuAnimation::PULL_UP);
        m_dropMenu->exec(pu, true, Fluent::MenuAnimation::PULL_UP);
    }
}

void MultiViewComboBoxPrivate::closeComboMenu()
{
    if (!m_dropMenu) {
        return;
    }
    m_dropMenu = nullptr;
}

void MultiViewComboBoxPrivate::toggleComboMenu()
{
    if (m_dropMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void MultiViewComboBoxPrivate::handleCheckBoxToggled(int index, bool checked)
{
    Q_Q(MultiViewComboBox);

    if (checked) {
        if (!m_selectedIndexes.contains(index)) {
            m_selectedIndexes.append(index);
            std::sort(m_selectedIndexes.begin(), m_selectedIndexes.end());
        }
    } else {
        m_selectedIndexes.removeAll(index);
    }

    updateText();
    emit q->selectionChanged();
}
