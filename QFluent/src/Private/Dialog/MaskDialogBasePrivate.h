#pragma once

#include <QColor>
#include <QPoint>

#include "QFluent/Dialog/MaskDialogBase.h"

class QFrame;
class QWidget;
class QHBoxLayout;

class MaskDialogBasePrivate
{
    Q_DECLARE_PUBLIC(MaskDialogBase)

public:
    MaskDialogBase *q_ptr = nullptr;

    virtual ~MaskDialogBasePrivate() = default;

    bool m_isClosableOnMaskClicked;
    QHBoxLayout *m_hBoxLayout;
    QWidget *m_windowMask = nullptr;
    QFrame *m_centerWidget = nullptr;

    // 记录阴影参数，done() 移除阴影后需要在下次 showEvent 时恢复
    int m_shadowBlurRadius = 60;
    QPoint m_shadowOffset = QPoint(0, 10);
    QColor m_shadowColor = QColor(0, 0, 0, 100);
};
