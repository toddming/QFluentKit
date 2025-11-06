#pragma once

#include "MaskDialogBase.h"
#include "Property.h"

class QEvent;
class QPushButton;
class QVBoxLayout;
class MessageBoxBasePrivate;
class QFLUENT_EXPORT MessageBoxBase : public MaskDialogBase
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MessageBoxBase)

public:
    explicit MessageBoxBase(QWidget *parent = nullptr);
    ~MessageBoxBase();

    QVBoxLayout* viewLayout();

    void hideYesButton();
    void hideCancelButton();

    QPushButton* yesButton();
    QPushButton* cancelbutton();
};
