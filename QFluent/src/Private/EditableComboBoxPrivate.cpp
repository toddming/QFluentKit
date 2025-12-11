#include "EditableComboBoxPrivate.h"
#include "QFluent/menu/ComboBoxMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"
#include "FluentGlobal.h"

#include <QStyle>
#include <QCursor>
#include <QAction>

EditableComboBoxPrivate::EditableComboBoxPrivate(EditableComboBox *parent)
    : QObject(parent)
    , q_ptr(parent)
{
}

ComboBoxMenu* EditableComboBoxPrivate::createComboMenu()
{
    Q_Q(EditableComboBox);

    ComboBoxMenu *menu = new ComboBoxMenu("menu", q);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    for (int i = 0; i < q->count(); ++i) {
        QAction *action = new QAction(items[i].icon, items[i].text, menu);
        action->setData(i);
        action->setCheckable(true);
        if (i == currentIndex) {
            action->setChecked(true);
        }
        menu->addAction(action);
        connect(action, &QAction::triggered, q, [=]() {
            int index = action->data().toInt();
            if (index != currentIndex) {
                q->setCurrentIndex(index);
                emit q->activated(index);
                emit q->textActivated(action->text());
            }
        });
    }
    connect(menu, &ComboBoxMenu::closed, q, [=]() {
        QPoint pos = q->mapFromGlobal(QCursor::pos());
        if (!q->rect().contains(pos)) {
            dropMenu = nullptr;
        }
    });
    return menu;
}

void EditableComboBoxPrivate::updateTextState(bool isPlaceholder)
{
    Q_Q(EditableComboBox);

    if (q->property("isPlaceholderText").toBool() == isPlaceholder) {
        return;
    }

    q->setProperty("isPlaceholderText", isPlaceholder);
    q->style()->unpolish(q);
    q->style()->polish(q);
}

void EditableComboBoxPrivate::showComboMenu()
{
    Q_Q(EditableComboBox);

    if (q->count() == 0) {
        return;
    }

    dropMenu = createComboMenu();

    if (dropMenu->view()->width() < q->width()) {
        dropMenu->view()->setMinimumWidth(q->width());
        dropMenu->adjustMenuSize();
    }

    if (q->currentIndex() >= 0 && q->currentIndex() < q->count()) {
        dropMenu->setDefaultAction(dropMenu->menuActions().at(q->currentIndex()));
    }

    int x = -dropMenu->width() / 2 + dropMenu->layout()->contentsMargins().left() + q->width() / 2;
    QPoint pd = q->mapToGlobal(QPoint(x, q->height()));
    int hd = dropMenu->view()->heightForAnimation(pd, Fluent::MenuAnimation::DROP_DOWN);

    QPoint pu = q->mapToGlobal(QPoint(x, 0));
    int hu = dropMenu->view()->heightForAnimation(pu, Fluent::MenuAnimation::PULL_UP);

    if (hd >= hu) {
        dropMenu->view()->adjustSize(pd, Fluent::MenuAnimation::DROP_DOWN);
        dropMenu->exec(pd, true, Fluent::MenuAnimation::DROP_DOWN);
    } else {
        dropMenu->view()->adjustSize(pu, Fluent::MenuAnimation::PULL_UP);
        dropMenu->exec(pu, true, Fluent::MenuAnimation::PULL_UP);
    }
}

void EditableComboBoxPrivate::closeComboMenu()
{
    if (!dropMenu) {
        return;
    }
    dropMenu = nullptr;
}

void EditableComboBoxPrivate::toggleComboMenu()
{
    if (dropMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void EditableComboBoxPrivate::handleMenuAction(QAction *action)
{
    Q_Q(EditableComboBox);

    int index = action->data().toInt();
    if (index < 0 || index >= q->count()) {
        return;
    }

    if (index != currentIndex) {
        q->setCurrentIndex(index);
    }

    emit q->activated(index);
    emit q->textActivated(q->currentText());
}

void EditableComboBoxPrivate::onClearButtonClicked()
{
    Q_Q(EditableComboBox);

    currentIndex = -1;
    q->clear();
}

void EditableComboBoxPrivate::onDropMenuClosed()
{
    dropMenu = nullptr;
}

void EditableComboBoxPrivate::onComboTextChanged(const QString &text)
{
    Q_Q(EditableComboBox);

    currentIndex = -1;
    emit q->currentTextChanged(text);

    for (int i = 0; i < items.size(); ++i) {
        if (items[i].text == text) {
            currentIndex = i;
            emit q->currentIndexChanged(i);
            return;
        }
    }
}

void EditableComboBoxPrivate::onActivated(const QString &text)
{
    Q_Q(EditableComboBox);

    int index = q->findText(text);
    if (index >= 0) {
        q->setCurrentIndex(index);
    }
}

void EditableComboBoxPrivate::onReturnPressed()
{
    Q_Q(EditableComboBox);

    if (q->text().isEmpty()) {
        return;
    }
    int index = q->findText(q->text());
    if (index > 0 && index != currentIndex) {
        currentIndex = index;
        emit q->currentIndexChanged(index);
    } else if (index == -1) {
        q->addItem(q->text());
        q->setCurrentIndex(q->count() - 1);
    }
}
