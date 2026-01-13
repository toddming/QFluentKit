#pragma once

#include "ProgressBar.h"
#include "FluentGlobal.h"

class QFLUENT_EXPORT ProgressRing : public ProgressBar
{
    Q_OBJECT
    Q_PROPERTY(int strokeWidth READ getStrokeWidth WRITE setStrokeWidth NOTIFY strokeWidthChanged)

public:
    explicit ProgressRing(QWidget *parent = nullptr, bool useAni = true);
    ~ProgressRing() override = default;

    int getStrokeWidth() const;
    void setStrokeWidth(int w);

    void paintEvent(QPaintEvent *event) override;

signals:
    void strokeWidthChanged(int w);

private:
    QColor lightBackgroundColor;
    QColor darkBackgroundColor;

    int _strokeWidth;

    void _drawText(QPainter &painter, const QString &text);

};
