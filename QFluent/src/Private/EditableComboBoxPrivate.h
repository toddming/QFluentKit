#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>

#include "Define.h"
#include "QFluent/LineEdit.h"


namespace EditableComboBoxDetail {
struct ComboItem {
    QString text;
    QIcon icon;
    QVariant userData;

    ComboItem(const QString &text = "",
              const QIcon &icon = QIcon(),
              const QVariant &userData = QVariant(NULL))
        : text(text), icon(icon), userData(userData) {}
};
}

class QAction;
class ComboBoxMenu;
class EditableComboBox;
class TranslateYAnimation;
class EditableComboBoxPrivate : public QObject {
public:
    Q_D_CREATE(EditableComboBox)
    Q_PROPERTY_CREATE_D(bool, IsPressed)
    Q_PROPERTY_CREATE_D(bool, IsHover)
    Q_PROPERTY_CREATE_D(QString, PlaceholderText)
    Q_PROPERTY_CREATE_D(int, CurrentIndex)
    Q_PROPERTY_CREATE_D(int, MaxVisibleItems)

    explicit EditableComboBoxPrivate(QObject* parent = nullptr);

    void handleMenuAction(QAction *action);
    void toggleComboMenu();

    void onClearButtonClicked();
    void onDropMenuClosed();
    void onComboTextChanged(const QString &text);
    void onActivated(const QString &text);
    void onReturnPressed();

private:
    LineEditButton *_dropButton = nullptr;
    ComboBoxMenu *_dropMenu = nullptr;
    TranslateYAnimation *_arrowAni = nullptr;
    QVector<EditableComboBoxDetail::ComboItem> _items;

    ComboBoxMenu* createComboMenu();
    void updateTextState(bool isPlaceholder);
    void showComboMenu();
    void closeComboMenu();

};
