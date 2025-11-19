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

    bool _isPressed;
    bool _isHover;
    QString _placeholderText;
    int _currentIndex;
    int _maxVisibleItems;

    ComboBoxMenu *_dropMenu = nullptr;
    TranslateYAnimation *_arrowAni = nullptr;
    QVector<ComboBoxDetail::ComboItem> _items;

    ComboBoxMenu* createComboMenu();
    void updateTextState(bool isPlaceholder);
    void showComboMenu();
    void closeComboMenu();
    void toggleComboMenu();

private:
    ComboBox * const q_ptr;

};
