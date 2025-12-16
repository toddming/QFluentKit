#include "Loading.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "Progress/IndeterminateProgressRing.h"


Loading::Loading(const QString &content, QWidget *parent)
    : MaskDialogBase(parent)
{
    setMaskColor(QColor(0, 0, 0, 153));

    QWidget *w = centerWidget();

    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setSpacing(12);

    auto progressRing = new IndeterminateProgressRing(w);
    progressRing->setFixedSize(45, 45);
    progressRing->setStrokeWidth(4);
    progressRing->setCustomBarColor(Qt::white, Qt::white);
    lay->addWidget(progressRing, 0, Qt::AlignHCenter);

    auto contentLabel = new QLabel(w);
    contentLabel->setStyleSheet("QLabel{color: white; font-weight: bold; font-size: 16px;}");
    contentLabel->setAlignment(Qt::AlignHCenter);
    contentLabel->setText(content.isEmpty() ? "loading..." : content);
    lay->addWidget(contentLabel, 0, Qt::AlignHCenter);

    setIsClosableOnMaskClicked(true);

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 50));
}

