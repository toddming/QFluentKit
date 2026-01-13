#pragma once

#include "MaskDialogBase.h"
#include "FluentGlobal.h"

class QEvent;
class MessageDialogPrivate;
class QFLUENT_EXPORT MessageDialog : public MaskDialogBase
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MessageDialog)

public:
    explicit MessageDialog(const QString &title, const QString &content, QWidget *parent = nullptr);
    ~MessageDialog();

signals:
    void yesClicked();
    void cancelClicked();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

};
