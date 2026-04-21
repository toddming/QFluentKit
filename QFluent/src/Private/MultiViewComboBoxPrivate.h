#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>
#include <QList>

#include "QFluent/MultiViewComboBox.h"

namespace MultiViewComboBoxDetail {
struct ComboItem {
    QString text;
    QIcon icon;
    QVariant userData;
    bool isSeparator = false;

    ComboItem(const QString &text = "",
              const QIcon &icon = QIcon(),
              const QVariant &userData = {})
        : text(text), icon(icon), userData(userData) {}
};
}

class MultiViewComboBoxMenu;
class TranslateYAnimation;
class CheckBox;
class MultiViewComboBoxPrivate : public QObject {
    Q_DECLARE_PUBLIC(MultiViewComboBox)

public:
    explicit MultiViewComboBoxPrivate(MultiViewComboBox *parent);
    void updateText();  // 更新按钮文本

    bool m_isPressed = false;
    bool m_isHover = false;
    QString m_placeholderText;
    int m_maxVisibleItems = 10;
    QList<int> m_selectedIndexes;  // 选中的索引列表

    MultiViewComboBoxMenu *m_dropMenu = nullptr;
    TranslateYAnimation *m_arrowAni = nullptr;
    QVector<MultiViewComboBoxDetail::ComboItem> m_items;

    MultiViewComboBoxMenu* createComboMenu();
    void showComboMenu();
    void closeComboMenu();
    void toggleComboMenu();

    // 槽函数：处理 CheckBox toggled
    void handleCheckBoxToggled(int index, bool checked);

protected:
    MultiViewComboBox *q_ptr = nullptr;
};
