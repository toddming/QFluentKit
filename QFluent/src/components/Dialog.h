#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>

#include "../common/Theme.h"
#include "MaskDialogBase.h"
#include "PushButton.h"
#include "../common/TextWrap.h"
#include "Label.h"
#include "../Property.h"

class Ui_MessageBox
{
public:
    void setupUi(const QString &title, const QString &content, QWidget *dialog);

    QLabel *titleLabel;
    BodyLabel *contentLabel;
    QFrame *buttonGroup;
    PrimaryPushButton *yesButton;
    QPushButton *cancelButton;

    void adjustText();
    void setContentCopyable(bool isCopyable);

private:
    QString m_content;
    QWidget *m_dialog;

    void setQss();
};

class QFLUENT_EXPORT MessageBox : public MaskDialogBase
{
    Q_OBJECT

public:
    explicit MessageBox(const QString &title, const QString &content, QWidget *parent = nullptr);

signals:
    void yesClicked();
    void cancelClicked();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui_MessageBox ui;
    void connectButtons();
};
