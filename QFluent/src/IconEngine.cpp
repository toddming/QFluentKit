#include "IconEngine.h"

#include <QPixmap>
#include <QPainter>
#include <QSvgRenderer>

#include "Theme.h"

IconEngine::IconEngine(const QString& fillPath, const QString& baseName, const QString& lightSuffix, const QString& darkSuffix)
{
    m_fillPath = fillPath;
    m_baseName = baseName;
    m_lightSuffix = lightSuffix;
    m_darkSuffix = darkSuffix;
}

QIconEngine* IconEngine::clone() const {
    return new IconEngine(*this);
}

QPixmap IconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode);
    Q_UNUSED(state);

    const QString &path = m_fillPath.arg(m_baseName, Theme::instance()->isDarkTheme() ? m_darkSuffix : m_lightSuffix);

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    QSvgRenderer renderer(path);
    if (renderer.isValid()) {
        renderer.render(&painter, QRect(QPoint(0, 0), size));
    }

    painter.end();

    return pixmap;
}

void IconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode);
    Q_UNUSED(state);

    const QString &path = m_fillPath.arg(m_baseName, Theme::instance()->isDarkTheme() ? m_darkSuffix : m_lightSuffix);

    QSvgRenderer renderer(path);
    if (renderer.isValid()) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        renderer.render(painter, rect);
        painter->restore();
    }
}
