#pragma once

#include "Dialog/MaskDialogBase.h"
#include "FluentGlobal.h"

class IndeterminateProgressRing;
class QFLUENT_EXPORT Loading : public MaskDialogBase
{
    Q_OBJECT
public:
    explicit Loading(const QString &content = QString(), QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    IndeterminateProgressRing *m_progressRing{nullptr};
};
