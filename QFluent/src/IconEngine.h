#pragma once

#include <QIconEngine>


class IconEngine : public QIconEngine {

public:
    IconEngine(const QString& fillPath, const QString& baseName, const QString& lightSuffix, const QString& darkSuffix);

    ~IconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QString m_fillPath;
    QString m_baseName;
    QString m_lightSuffix;
    QString m_darkSuffix;
};
