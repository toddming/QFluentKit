#pragma once

#include "ProgressBar.h"
#include "FluentGlobal.h"

class QFLUENT_EXPORT ProgressRing : public ProgressBar
{
    Q_OBJECT
    Q_PROPERTY(int strokeWidth READ strokeWidth WRITE setStrokeWidth NOTIFY strokeWidthChanged)

public:
    explicit ProgressRing(QWidget *parent = nullptr, bool useAni = true);
    ~ProgressRing() override = default;

    int strokeWidth() const;
    void setStrokeWidth(int w);

    void paintEvent(QPaintEvent *event) override;

signals:
    void strokeWidthChanged(int w);

private:
    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;

    int m_strokeWidth;

    void drawText(QPainter &painter, const QString &text);

};
