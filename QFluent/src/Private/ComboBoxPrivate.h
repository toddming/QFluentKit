#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>

#include "Define.h"

namespace ComboBoxDetail {
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
class ComboBox;
class ComboBoxMenu;
class TranslateYAnimation;
class ComboBoxPrivate : public QObject {
public:
    Q_D_CREATE(ComboBox)
    Q_PROPERTY_CREATE_D(bool, IsPressed)
    Q_PROPERTY_CREATE_D(bool, IsHover)
    Q_PROPERTY_CREATE_D(QString, PlaceholderText)
    Q_PROPERTY_CREATE_D(int, CurrentIndex)
    Q_PROPERTY_CREATE_D(int, MaxVisibleItems)

    explicit ComboBoxPrivate(QObject* parent = nullptr);

    void handleMenuAction(QAction *action);

private:
    ComboBoxMenu *_dropMenu = nullptr;
    TranslateYAnimation *_arrowAni = nullptr;
    QVector<ComboBoxDetail::ComboItem> _items;

    ComboBoxMenu* createComboMenu();
    void updateTextState(bool isPlaceholder);
    void showComboMenu();
    void closeComboMenu();
    void toggleComboMenu();

};
