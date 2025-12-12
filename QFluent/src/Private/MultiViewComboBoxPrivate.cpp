#include "MultiViewComboBoxPrivate.h"
#include "QFluent/MultiViewComboBox.h"
#include "QFluent/menu/ComboBoxMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"
#include "QFluent/CheckBox.h"
#include "FluentGlobal.h"

#include <QStyle>
#include <QCursor>

MultiViewComboBoxPrivate::MultiViewComboBoxPrivate(MultiViewComboBox *parent)
    : QObject(parent)
    , q_ptr(parent)
{
}

void MultiViewComboBoxPrivate::updateText()
{
    Q_Q(MultiViewComboBox);

    if (selectedIndexes.isEmpty()) {
        q->setText(placeholderText);
        q->setProperty("isPlaceholderText", true);
    } else {
        QStringList texts;
        for (int index : selectedIndexes) {
            texts.append(items[index].text);
        }
        q->setText(texts.join(", "));
        q->setProperty("isPlaceholderText", false);
    }

    q->style()->unpolish(q);
    q->style()->polish(q);
    q->adjustSize();
}

ComboBoxMenu* MultiViewComboBoxPrivate::createComboMenu()
{
    Q_Q(MultiViewComboBox);

    ComboBoxMenu *menu = new ComboBoxMenu("menu", q);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    for (int i = 0; i < q->count(); ++i) {
        CheckBox *checkBox = new CheckBox(items[i].text, menu);
        checkBox->setChecked(selectedIndexes.contains(i));
        connect(checkBox, &CheckBox::toggled, [=](bool checked) {
            handleCheckBoxToggled(i, checked);
        });
        menu->addWidget(checkBox);
    }

    connect(menu, &ComboBoxMenu::closed, q, [=]() {
        QPoint pos = q->mapFromGlobal(QCursor::pos());
        if (!q->rect().contains(pos)) {
            dropMenu = nullptr;
        }
    });

    // 如果 maxVisibleItems > 0，设置最大高度（假设每个项高度约 30px，您可调整）
    if (maxVisibleItems > 0) {
        int itemHeight = 30;  // 假设每个 CheckBox 高度
        menu->setMaximumHeight(maxVisibleItems * itemHeight + menu->layout()->contentsMargins().top() + menu->layout()->contentsMargins().bottom());
    }

    return menu;
}

void MultiViewComboBoxPrivate::showComboMenu()
{
    Q_Q(MultiViewComboBox);

    if (q->count() == 0) {
        return;
    }

    dropMenu = createComboMenu();

    if (dropMenu->view()->width() < q->width()) {
        dropMenu->view()->setMinimumWidth(q->width());
        dropMenu->adjustMenuSize();
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

void MultiViewComboBoxPrivate::closeComboMenu()
{
    if (!dropMenu) {
        return;
    }
    dropMenu->close();
    dropMenu = nullptr;
}

void MultiViewComboBoxPrivate::toggleComboMenu()
{
    if (dropMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void MultiViewComboBoxPrivate::handleCheckBoxToggled(int index, bool checked)
{
    Q_Q(MultiViewComboBox);

    if (checked) {
        if (!selectedIndexes.contains(index)) {
            selectedIndexes.append(index);
            std::sort(selectedIndexes.begin(), selectedIndexes.end());
        }
    } else {
        selectedIndexes.removeAll(index);
    }

    updateText();
    emit q->selectionChanged();
}
