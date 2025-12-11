#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>

#include "QFluent/ComboBox.h"

namespace ComboBoxDetail {
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
class ComboBoxPrivate : public QObject {
    Q_DECLARE_PUBLIC(ComboBox)

public:
    explicit ComboBoxPrivate(ComboBox *parent);
    void handleMenuAction(QAction *action);

    bool isPressed;
    bool isHover;
    QString placeholderText;
    int currentIndex;
    int maxVisibleItems;

    ComboBoxMenu *dropMenu = nullptr;
    TranslateYAnimation *arrowAni = nullptr;
    QVector<ComboBoxDetail::ComboItem> items;

    ComboBoxMenu* createComboMenu();
    void updateTextState(bool isPlaceholder);
    void showComboMenu();
    void closeComboMenu();
    void toggleComboMenu();

private:
    ComboBox * const q_ptr;
};
