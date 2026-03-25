#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>

#include "QFluent/LineEdit.h"
#include "QFluent/EditableComboBox.h"

namespace EditableComboBoxDetail {

struct ComboItem
{
    QString text;
    QIcon icon;
    QVariant userData;

    ComboItem(const QString &text = QString(),
              const QIcon &icon = QIcon(),
              const QVariant &userData = QVariant())
        : text(text), icon(icon), userData(userData) {}
};

} // namespace EditableComboBoxDetail

class QAction;
class ComboBoxMenu;
class TranslateYAnimation;

class EditableComboBoxPrivate : public QObject
{
    Q_DECLARE_PUBLIC(EditableComboBox)

public:
    explicit EditableComboBoxPrivate(EditableComboBox *parent);

    void handleMenuAction(QAction *action);
    void toggleComboMenu();

    void onClearButtonClicked();
    void onDropMenuClosed();
    void onComboTextChanged(const QString &text);
    void onActivated(const QString &text);
    void onReturnPressed();

    ComboBoxMenu* createComboMenu();
    void updateTextState(bool isPlaceholder);
    void showComboMenu();
    void closeComboMenu();

    bool m_isPressed;
    bool m_isHover;
    QString m_placeholderText;
    int m_currentIndex;
    int m_maxVisibleItems;

    LineEditButton *m_dropButton = nullptr;
    ComboBoxMenu *m_dropMenu = nullptr;
    TranslateYAnimation *m_arrowAni = nullptr;
    QVector<EditableComboBoxDetail::ComboItem> m_items;

private:
    EditableComboBox * const q_ptr;
};
