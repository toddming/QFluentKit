#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QPushButton>
#include <QVector>
#include <QIcon>
#include <QEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QAction>

#include "Theme.h"
#include "Icon.h"
#include "menu/RoundMenu.h"
#include "Define.h"
#include "Animation.h"

/////////////////ComboBoxMenu//////////////////////
class ComboBoxMenu : public RoundMenu
{
public:
    explicit ComboBoxMenu(const QString& title = "", QWidget *parent = nullptr);

    void exec(const QPoint& pos, bool animate = true,
              MenuAnimationType::MenuAnimation aniType = MenuAnimationType::MenuAnimation::DROP_DOWN);

};


class QFLUENT_EXPORT ComboBox : public QPushButton
{
    Q_OBJECT
public:
    struct ComboItem {
        QString text;
        QIcon icon;
        QVariant userData;

        ComboItem(const QString &text = "",
                 const QIcon &icon = QIcon(),
                 const QVariant &userData = QVariant(NULL))
            : text(text), icon(icon), userData(userData) {}
    };

    explicit ComboBox(QWidget *parent = nullptr);

    // 添加项目
    void addItem(const QString &text,
                const QIcon &icon = QIcon(),
                const QVariant &userData = QVariant(NULL));
    void addItems(const QStringList &texts);

    // 插入项目
    void insertItem(int index, const QString &text,
                   const QIcon &icon = QIcon(),
                   const QVariant &userData = QVariant(NULL));
    void insertItems(int index, const QStringList &texts);

    // 移除项目
    void removeItem(int index);
    void clear();

    // 当前选择
    int currentIndex() const;
    QString currentText() const;
    QVariant currentData() const;

    // 设置当前选择
    void setCurrentIndex(int index);
    void setCurrentText(const QString &text);

    // 项目访问
    int count() const;
    QString itemText(int index) const;
    QIcon itemIcon(int index) const;
    QVariant itemData(int index) const;

    // 查找
    int findText(const QString &text) const;
    int findData(const QVariant &data) const;

    // 设置占位文本
    void setPlaceholderText(const QString &text);
    QString placeholderText() const;

    // 设置最大可见项目数
    void setMaxVisibleItems(int max);
    int maxVisibleItems() const;

    void setText(const QString &text);

signals:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString &text);
    void activated(int index);
    void textActivated(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void handleMenuAction(QAction *action);

private:
    void toggleComboMenu();
    void showComboMenu();
    void closeComboMenu();
    void updateTextState(bool isPlaceholder);
    ComboBoxMenu* createComboMenu();

    QVector<ComboItem> m_items;
    int m_currentIndex = -1;
    int m_maxVisibleItems = -1;
    QString m_placeholderText;
    bool m_isHover = false;
    bool m_isPressed = false;
    ComboBoxMenu *m_dropMenu = nullptr;

    TranslateYAnimation *arrowAni;

};

#endif // COMBOBOX_H
