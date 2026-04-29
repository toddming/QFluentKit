#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>
#include <QList>
#include <QAbstractItemModel>

#include "ComboItemModel.h"
#include "ComboBoxHelper.h"
#include "QFluent/Menu/MultiViewComboBoxMenu.h"

class TranslateYAnimation;
class MultiViewComboBox;

class MultiViewComboBoxPrivate : public QObject
{
    Q_OBJECT

public:
    MultiViewComboBoxPrivate(MultiViewComboBox *q);
    ~MultiViewComboBoxPrivate();

    void createComboMenu();
    void showComboMenu();
    void closeComboMenu();
    void toggleComboMenu();
    void updateTextState();

    void setModel(QAbstractItemModel *model);
    void connectModel(QAbstractItemModel *model);
    void disconnectModel(QAbstractItemModel *model);

    QAbstractItemModel *m_model;
    ComboItemModel *m_internalModel;
    MultiViewComboBoxMenu *m_comboMenu = nullptr;
    TranslateYAnimation *m_arrowAni = nullptr;
    QList<int> m_selectedIndexes;
    int m_maxSelectedCount = -1;
    int m_maxVisibleItems = -1;
    QString m_placeholderText;
    bool m_isPressed = false;
    bool m_isHover = false;
    bool m_settingCurrentIndex = false;

private slots:
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsRemoved(const QModelIndex &parent, int first, int last);
    void onModelReset();
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onMenuAction(int index, bool checked);

private:
    MultiViewComboBox *q_ptr;
    Q_DECLARE_PUBLIC(MultiViewComboBox)
};