#pragma once

#include <QObject>
#include <QIcon>
#include <QVariant>
#include <QList>
#include <QAbstractItemModel>

#include "ComboItemModel.h"
#include "ComboBoxHelper.h"
#include "QFluent/Menu/ComboBoxMenu.h"

class LineEditButton;
class EditableComboBox;

class EditableComboBoxPrivate : public QObject
{
    Q_OBJECT

public:
    EditableComboBoxPrivate(EditableComboBox *q);
    ~EditableComboBoxPrivate();

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
    LineEditButton *m_dropButton = nullptr;
    int m_currentIndex = -1;
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
    void onMenuAction(int index);
    void onComboTextChanged(const QString &text);
    void onClearButtonClicked();
    void onReturnPressed();
    void onActivated(const QString &text);

private:
    EditableComboBox *q_ptr;
    Q_DECLARE_PUBLIC(EditableComboBox)
};