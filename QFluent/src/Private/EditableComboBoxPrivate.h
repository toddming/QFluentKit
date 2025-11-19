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
    explicit EditableComboBoxPrivate(EditableComboBox* parent);

    void handleMenuAction(QAction *action);
    void toggleComboMenu();

    bool _isPressed;
    bool _isHover;
    QString _placeholderText;
    int _currentIndex;
    int _maxVisibleItems;

    void onClearButtonClicked();
    void onDropMenuClosed();
    void onComboTextChanged(const QString &text);
    void onActivated(const QString &text);
    void onReturnPressed();

    LineEditButton *_dropButton = nullptr;
    ComboBoxMenu *_dropMenu = nullptr;
    TranslateYAnimation *_arrowAni = nullptr;
    QVector<EditableComboBoxDetail::ComboItem> _items;

    ComboBoxMenu* createComboMenu();
    void updateTextState(bool isPlaceholder);
    void showComboMenu();
    void closeComboMenu();

private:
    EditableComboBox * const q_ptr;
};
