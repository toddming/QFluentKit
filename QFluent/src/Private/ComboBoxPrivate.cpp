#include "ComboBoxPrivate.h"
#include "QFluent/ComboBox.h"
#include "QFluent/menu/ComboBoxMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"

#include <QStyle>
#include <QCursor>
#include <QAction>

ComboBoxPrivate::ComboBoxPrivate(QObject* parent) : QObject{parent}
{

}

ComboBoxMenu* ComboBoxPrivate::createComboMenu()
{
    Q_Q(ComboBox);

    ComboBoxMenu *menu = new ComboBoxMenu("menu", q);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    for (int i = 0; i < q->count(); ++i) {
        QAction *action = new QAction(_items[i].icon, _items[i].text, menu);
        action->setData(i);
        action->setCheckable(true);
        if (i == _pCurrentIndex) {
            action->setChecked(true);
        }
        menu->addAction(action);
        connect(action, &QAction::triggered, q, [=](){
            int index = action->property("index").toInt();
            if (index != _pCurrentIndex) {
                q->setCurrentIndex(index);
                emit q->activated(index);
                emit q->textActivated(action->text());
            }
        });
    }
    connect(menu, &ComboBoxMenu::closed, q, [=](){
        QPoint pos = q->mapFromGlobal(QCursor::pos());
        if (!q->rect().contains(pos)) {
            _dropMenu = nullptr;
        }
    });
    return menu;
}

void ComboBoxPrivate::updateTextState(bool isPlaceholder)
{
    Q_Q(ComboBox);

    if (q->property("isPlaceholderText").toBool() == isPlaceholder) return;

    q->setProperty("isPlaceholderText", isPlaceholder);
    q->style()->unpolish(q);
    q->style()->polish(q);
}

void ComboBoxPrivate::showComboMenu()
{
    Q_Q(ComboBox);

    if (q->count() == 0) return;

    _dropMenu = createComboMenu();

    if (_dropMenu->view()->width() < q->width()) {
        _dropMenu->view()->setMinimumWidth(q->width());
        _dropMenu->adjustMenuSize();
    }

    if (q->currentIndex() >= 0 && q->currentIndex() < q->count()) {
        _dropMenu->setDefaultAction(_dropMenu->menuActions().at(q->currentIndex()));
    }

    int x = -_dropMenu->width() / 2 + _dropMenu->layout()->contentsMargins().left() + q->width() / 2;
    QPoint pd = q->mapToGlobal(QPoint(x, q->height()));
    int hd = _dropMenu->view()->heightForAnimation(pd, MenuAnimationType::MenuAnimation::DROP_DOWN);

    QPoint pu = q->mapToGlobal(QPoint(x, 0));
    int hu = _dropMenu->view()->heightForAnimation(pu, MenuAnimationType::MenuAnimation::PULL_UP);

    if (hd >= hu) {
        _dropMenu->view()->adjustSize(pd, MenuAnimationType::MenuAnimation::DROP_DOWN);
        _dropMenu->exec(pd, true, MenuAnimationType::MenuAnimation::DROP_DOWN);
    } else {
        _dropMenu->view()->adjustSize(pu, MenuAnimationType::MenuAnimation::PULL_UP);
        _dropMenu->exec(pu, true, MenuAnimationType::MenuAnimation::PULL_UP);
    }
}

void ComboBoxPrivate::closeComboMenu()
{
    Q_Q(ComboBox);

    if (!_dropMenu) return;
    _dropMenu = nullptr;
}


void ComboBoxPrivate::toggleComboMenu()
{
    Q_Q(ComboBox);

    if (_dropMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void ComboBoxPrivate::handleMenuAction(QAction *action)
{
    Q_Q(ComboBox);

    int index = action->data().toInt();
    if (index < 0 || index >= q->count()) return;

    if (index != _pCurrentIndex) {
        q->setCurrentIndex(index);
    }

    emit q->activated(index);
    emit q->textActivated(q->currentText());
}

