#pragma once

#include <QPushButton>
#include <QVariant>
#include <QIcon>
#include <Qt>

#include "FluentGlobal.h"

class ComboBoxMenu;
class ComboBoxPrivate;
class QFLUENT_EXPORT ComboBox : public QPushButton
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, ComboBox)

public:
    explicit ComboBox(QWidget *parent = nullptr);
    ~ComboBox() override;

    // 添加项目
    void addItem(const QString &text, const QVariant &userData = QVariant());
    void addItem(const QIcon &icon, const QString &text, const QVariant &userData = QVariant());
    void addItems(const QStringList &texts);

    // 插入项目
    void insertItem(int index, const QString &text, const QVariant &userData = QVariant());
    void insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userData = QVariant());
    void insertItems(int index, const QStringList &texts);
    void insertSeparator(int index);

    // 移除项目
    void removeItem(int index);
    void clear();

    // 当前选择
    int currentIndex() const;
    QString currentText() const;
    QVariant currentData(int role = Qt::UserRole) const;

    // 设置当前选择
    void setCurrentIndex(int index);
    void setCurrentText(const QString &text);

    // 项目访问
    int count() const;
    QString itemText(int index) const;
    QIcon itemIcon(int index) const;
    QVariant itemData(int index, int role = Qt::UserRole) const;

    // 项目修改
    void setItemText(int index, const QString &text);
    void setItemIcon(int index, const QIcon &icon);
    void setItemData(int index, const QVariant &value, int role = Qt::UserRole);

    // 查找
    int findText(const QString &text, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;
    int findData(const QVariant &data, int role = Qt::UserRole, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;

    // 设置占位文本
    void setPlaceholderText(const QString &text);
    QString placeholderText() const;

    void setText(const QString &text);

    void setMaxVisibleItems(int count);

    int maxVisibleItems() const;

signals:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString &text);
    void activated(int index);
    void textActivated(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QScopedPointer<ComboBoxPrivate> d_ptr;
};
