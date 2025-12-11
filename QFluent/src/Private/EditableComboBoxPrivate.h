#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>

#include "QFluent/LineEdit.h"
#include "QFluent/EditableComboBox.h"

namespace EditableComboBoxDetail {
struct ComboItem {
    QString text;
    QIcon icon;
    QVariant userData;

    ComboItem(const QString &text = "",
              const QIcon &icon = QIcon(),
              const QVariant &userData = {})
        : text(text), icon(icon), userData(userData) {}
};
}

class QAction;
class ComboBoxMenu;
class TranslateYAnimation;
class EditableComboBoxPrivate : public QObject {
    Q_DECLARE_PUBLIC(EditableComboBox)

public:
    explicit EditableComboBoxPrivate(EditableComboBox *parent);

    void handleMenuAction(QAction *action);
    void toggleComboMenu();

    bool isPressed;
    bool isHover;
    QString placeholderText;
    int currentIndex;
    int maxVisibleItems;

    void onClearButtonClicked();
    void onDropMenuClosed();
    void onComboTextChanged(const QString &text);
    void onActivated(const QString &text);
    void onReturnPressed();

    LineEditButton *dropButton = nullptr;
    ComboBoxMenu *dropMenu = nullptr;
    TranslateYAnimation *arrowAni = nullptr;
    QVector<EditableComboBoxDetail::ComboItem> items;

    ComboBoxMenu* createComboMenu();
    void updateTextState(bool isPlaceholder);
    void showComboMenu();
    void closeComboMenu();

private:
    EditableComboBox * const q_ptr;
};
