#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>

#include "MaskDialogBase.h"
#include "QFluent/PushButton.h"

#include "QFluent/Label.h"
#include "Property.h"

class Ui_MessageDialog
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

class MessageDialogPrivate;
class QFLUENT_EXPORT MessageDialog : public MaskDialogBase
{
    Q_OBJECT
    Q_Q_CREATE(MessageDialog)

public:
    explicit MessageDialog(const QString &title, const QString &content, QWidget *parent = nullptr);
    ~MessageDialog();

signals:
    void yesClicked();
    void cancelClicked();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui_MessageDialog ui;
    void connectButtons();
};
