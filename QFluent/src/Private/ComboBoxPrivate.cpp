#include "ComboBoxPrivate.h"
#include "QFluent/ComboBox.h"
#include "QFluent/Menu/ComboBoxMenu.h"
#include "QFluent/Menu/MenuActionListWidget.h"
#include "FluentGlobal.h"

#include <QStyle>
#include <QCursor>
#include <QAction>
#include <QPointer>

ComboBoxPrivate::ComboBoxPrivate(ComboBox *parent)
    : QObject(parent)
    , q_ptr(parent)
    , m_isPressed(false)
    , m_isHover(false)
    , m_placeholderText(QString())
    , m_currentIndex(-1)
    , m_maxVisibleItems(-1)
{
}

ComboBoxMenu* ComboBoxPrivate::createComboMenu()
{
    Q_Q(ComboBox);

    ComboBoxMenu *menu = new ComboBoxMenu("menu", q);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    if (m_maxVisibleItems > 0) {
        menu->setMaxVisibleItems(m_maxVisibleItems);
    }
    QPointer<ComboBox> q_ptr = q;
    for (int i = 0; i < q->count(); ++i) {
        if (m_items[i].isSeparator) {
            menu->addSeparator();
            continue;
        }
        QAction *action = new QAction(m_items[i].icon, m_items[i].text, menu);
        action->setData(i);
        action->setCheckable(true);
        if (i == m_currentIndex) {
            action->setChecked(true);
        }
        menu->addAction(action);
        connect(action, &QAction::triggered, q, [q_ptr, action, currentIndex = m_currentIndex]() {
            if (!q_ptr) return;
            int index = action->data().toInt();
            if (index != currentIndex) {
                q_ptr->setCurrentIndex(index);
                emit q_ptr->activated(index);
                emit q_ptr->textActivated(action->text());
            }
        });
    }
    connect(menu, &ComboBoxMenu::closed, q, [q_ptr, this]() {
        if (!q_ptr) return;
        QPoint pos = q_ptr->mapFromGlobal(QCursor::pos());
        if (!q_ptr->rect().contains(pos)) {
            m_dropMenu = nullptr;
        }
    });
    return menu;
}

void ComboBoxPrivate::updateTextState(bool isPlaceholder)
{
    Q_Q(ComboBox);

    if (q->property("isPlaceholderText").toBool() == isPlaceholder) {
        return;
    }

    q->setProperty("isPlaceholderText", isPlaceholder);
    q->style()->unpolish(q);
    q->style()->polish(q);
}

void ComboBoxPrivate::showComboMenu()
{
    Q_Q(ComboBox);

    if (q->count() == 0) {
        return;
    }

    m_dropMenu = createComboMenu();

    if (m_dropMenu->view()->width() < q->width()) {
        m_dropMenu->view()->setMinimumWidth(q->width());
        m_dropMenu->adjustMenuSize();
    }

    if (q->currentIndex() >= 0 && q->currentIndex() < q->count()) {
        for (QAction *action : m_dropMenu->menuActions()) {
            if (action->data().toInt() == q->currentIndex()) {
                m_dropMenu->setDefaultAction(action);
                break;
            }
        }
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

void ComboBoxPrivate::closeComboMenu()
{
    if (!m_dropMenu) {
        return;
    }
    m_dropMenu = nullptr;
}

void ComboBoxPrivate::toggleComboMenu()
{
    if (m_dropMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void ComboBoxPrivate::handleMenuAction(QAction *action)
{
    Q_Q(ComboBox);

    int index = action->data().toInt();
    if (index < 0 || index >= q->count()) {
        return;
    }

    if (index != m_currentIndex) {
        q->setCurrentIndex(index);
    }

    emit q->activated(index);
    emit q->textActivated(q->currentText());
}
