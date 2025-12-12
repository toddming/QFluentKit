#pragma once

#include <QPushButton>
#include <QVariant>
#include <QVector>
#include <QIcon>
#include <QList>

#include "FluentGlobal.h"

class ComboBoxMenu;
class MultiViewComboBoxPrivate;
class QFLUENT_EXPORT MultiViewComboBox : public QPushButton
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MultiViewComboBox)

public:
    explicit MultiViewComboBox(QWidget *parent = nullptr);
    ~MultiViewComboBox();

    // 添加项目
    void addItem(const QString &text,
                 const QIcon &icon = QIcon(),
                 const QVariant &userData = QVariant());
    void addItems(const QStringList &texts);

    // 插入项目
    void insertItem(int index, const QString &text,
                    const QIcon &icon = QIcon(),
                    const QVariant &userData = QVariant());
    void insertItems(int index, const QStringList &texts);

    // 移除项目
    void removeItem(int index);
    void clear();

    // 选中管理
    void setItemSelected(int index, bool selected);
    bool isItemSelected(int index) const;
    QList<int> selectedIndexes() const;
    QStringList selectedTexts() const;
    QList<QVariant> selectedDatas() const;

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

    void setMaxVisibleItems(int count);
    int maxVisibleItems() const;

signals:
    void selectionChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QScopedPointer<MultiViewComboBoxPrivate> d_ptr;
};
