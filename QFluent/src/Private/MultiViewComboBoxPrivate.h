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

    bool isPressed = false;
    bool isHover = false;
    QString placeholderText;
    int maxVisibleItems = -1;
    QList<int> selectedIndexes;  // 选中的索引列表

    MultiViewComboBoxMenu *dropMenu = nullptr;
    TranslateYAnimation *arrowAni = nullptr;
    QVector<MultiViewComboBoxDetail::ComboItem> items;

    MultiViewComboBoxMenu* createComboMenu();
    void showComboMenu();
    void closeComboMenu();
    void toggleComboMenu();

    // 槽函数：处理 CheckBox toggled
    void handleCheckBoxToggled(int index, bool checked);

private:
    MultiViewComboBox * const q_ptr;
};
