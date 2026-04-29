#pragma once

#include <QPushButton>
#include <QVariant>
#include <QIcon>
#include <QList>
#include <Qt>

#include "FluentGlobal.h"

class QAbstractItemModel;
class ComboBoxMenu;
class ComboItemModel;
class ComboBoxPrivate;

class QFLUENT_EXPORT ComboBox : public QPushButton
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ComboBox)

public:
    explicit ComboBox(QWidget *parent = nullptr);
    ~ComboBox() override;

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void addItem(const QString &text, const QVariant &userData = QVariant());
    void addItem(const QIcon &icon, const QString &text, const QVariant &userData = QVariant());
    void addItems(const QStringList &texts);

    void insertItem(int index, const QString &text, const QVariant &userData = QVariant());
    void insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userData = QVariant());
    void insertItems(int index, const QStringList &texts);
    void insertSeparator(int index);

    void removeItem(int index);
    void clear();

    int currentIndex() const;
    QString currentText() const;
    QVariant currentData(int role = Qt::UserRole) const;
    void setCurrentIndex(int index);
    void setCurrentText(const QString &text);

    int count() const;
    QString itemText(int index) const;
    QIcon itemIcon(int index) const;
    QVariant itemData(int index, int role = Qt::UserRole) const;

    void setItemText(int index, const QString &text);
    void setItemIcon(int index, const QIcon &icon);
    void setItemData(int index, const QVariant &value, int role = Qt::UserRole);

    int findText(const QString &text, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;
    int findData(const QVariant &data, int role = Qt::UserRole, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;

    void setPlaceholderText(const QString &text);
    QString placeholderText() const;

    void setText(const QString &text);

    void setMaxVisibleItems(int count);
    int maxVisibleItems() const;

signals:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QScopedPointer<ComboBoxPrivate> d_ptr;
};