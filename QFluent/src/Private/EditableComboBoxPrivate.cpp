#include "EditableComboBoxPrivate.h"
#include "QFluent/EditableComboBox.h"
#include "QFluent/menu/ComboBoxMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"

#include <QStyle>
#include <QCursor>
#include <QAction>

EditableComboBoxPrivate::EditableComboBoxPrivate(QObject* parent) : QObject{parent}
{

}

ComboBoxMenu* EditableComboBoxPrivate::createComboMenu()
{
    Q_Q(EditableComboBox);

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

void EditableComboBoxPrivate::updateTextState(bool isPlaceholder)
{
    Q_Q(EditableComboBox);

    if (q->property("isPlaceholderText").toBool() == isPlaceholder) return;

    q->setProperty("isPlaceholderText", isPlaceholder);
    q->style()->unpolish(q);
    q->style()->polish(q);
}

void EditableComboBoxPrivate::showComboMenu()
{
    Q_Q(EditableComboBox);

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

void EditableComboBoxPrivate::closeComboMenu()
{
    Q_Q(EditableComboBox);

    if (!_dropMenu) return;
    _dropMenu = nullptr;
}


void EditableComboBoxPrivate::toggleComboMenu()
{
    Q_Q(EditableComboBox);

    if (_dropMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void EditableComboBoxPrivate::handleMenuAction(QAction *action)
{
    Q_Q(EditableComboBox);

    int index = action->data().toInt();
    if (index < 0 || index >= q->count()) return;

    if (index != _pCurrentIndex) {
        q->setCurrentIndex(index);
    }

    emit q->activated(index);
    emit q->textActivated(q->currentText());
}

void EditableComboBoxPrivate::onClearButtonClicked()
{
    Q_Q(EditableComboBox);

    _pCurrentIndex = -1;
    q->clear();
}

void EditableComboBoxPrivate::onDropMenuClosed()
{
    _dropMenu = nullptr;
}


void EditableComboBoxPrivate::onComboTextChanged(const QString &text)
{
    Q_Q(EditableComboBox);

    _pCurrentIndex = -1;
    emit q->currentTextChanged(text);

    for (int i = 0; i < _items.size(); ++i) {
        if (_items[i].text == text) {
            _pCurrentIndex = i;
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
    if (index > 0 && index != _pCurrentIndex) {
        _pCurrentIndex = index;
        emit q->currentIndexChanged(index);
    } else if (index == -1) {
        q->addItem(q->text());
        q->setCurrentIndex(q->count() - 1);
    }
}
