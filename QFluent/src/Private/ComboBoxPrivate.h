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
    bool isSeparator = false;

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

    bool m_isPressed = false;
    bool m_isHover = false;
    QString m_placeholderText;
    int m_currentIndex = -1;
    int m_maxVisibleItems = 10;

    ComboBoxMenu *m_dropMenu = nullptr;
    TranslateYAnimation *m_arrowAni = nullptr;
    QVector<ComboBoxDetail::ComboItem> m_items;

protected:
    ComboBox *q_ptr = nullptr;
};
