#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>
#include <QList>
#include <QAbstractItemModel>
#include <QPersistentModelIndex>

#include "ComboItemModel.h"
#include "ComboBoxHelper.h"
#include "QFluent/Menu/ComboBoxMenu.h"

class TranslateYAnimation;

class ComboBox;

class ComboBoxPrivate : public QObject
{
    Q_OBJECT

public:
    ComboBoxPrivate(ComboBox *q);
    ~ComboBoxPrivate();

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
    ComboBoxMenu *m_comboMenu = nullptr;
    TranslateYAnimation *m_arrowAni = nullptr;
    int m_currentIndex = -1;
    int m_maxVisibleItems = -1;
    QString m_placeholderText;
    bool m_isPressed = false;
    bool m_isHover = false;
    bool m_settingCurrentIndex = false;
    bool m_inserting = false;
    int m_indexBeforeChange = -1;

private slots:
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsRemoved(const QModelIndex &parent, int first, int last);
    void onModelReset();
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onMenuAction(int index);

private:
    ComboBox *q_ptr;
    Q_DECLARE_PUBLIC(ComboBox)
};