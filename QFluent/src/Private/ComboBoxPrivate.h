#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>

#include "QFluent/ComboBox.h"

namespace ComboBoxDetail {

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

} // namespace ComboBoxDetail

class QAction;
class ComboBoxMenu;
class TranslateYAnimation;

class ComboBoxPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ComboBox)

public:
    explicit ComboBoxPrivate(ComboBox *parent);

    void handleMenuAction(QAction *action);

    ComboBoxMenu* createComboMenu();
    void updateTextState(bool isPlaceholder);
    void showComboMenu();
    void closeComboMenu();
    void toggleComboMenu();

    bool m_isPressed;
    bool m_isHover;
    QString m_placeholderText;
    int m_currentIndex;
    int m_maxVisibleItems;

    ComboBoxMenu *m_dropMenu = nullptr;
    TranslateYAnimation *m_arrowAni = nullptr;
    QVector<ComboBoxDetail::ComboItem> m_items;

private:
    ComboBox * const q_ptr;
};
