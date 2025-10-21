#include "AvatarWidget.h"
#include <QImageReader>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QFont>
#include <QImage>
#include <QPixmap>
#include <QMovie>
#include <QPainterPath>
#include <QUrl>
#include <QSize>
#include <QColor>
#include <QVariant>

#include "Theme.h"

ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent),
    m_topLeftRadius(0),
    m_topRightRadius(0),
    m_bottomLeftRadius(0),
    m_bottomRightRadius(0)
{
    _postInit();
}

ImageLabel::ImageLabel(const QString &image, QWidget *parent)
    : ImageLabel(parent)
{
    setImage(image);
}

ImageLabel::ImageLabel(const QImage &image, QWidget *parent)
    : ImageLabel(parent)
{
    setImage(image);
}

ImageLabel::ImageLabel(const QPixmap &image, QWidget *parent)
    : ImageLabel(parent)
{
    setImage(image);
}

void ImageLabel::_postInit()
{
    // 初始化代码
}

void ImageLabel::setBorderRadius(int topLeft, int topRight, int bottomLeft, int bottomRight)
{
    m_topLeftRadius = topLeft;
    m_topRightRadius = topRight;
    m_bottomLeftRadius = bottomLeft;
    m_bottomRightRadius = bottomRight;
    update();
}

QImage ImageLabel::image() const
{
    return m_image;
}

void ImageLabel::setImage(const QVariant &image)
{
    if (image.isNull()) {
        m_image = QImage();
    } else if (image.typeId() == QMetaType::QString) {
        QString filePath = image.toString();
        QImageReader reader(filePath);
        if (reader.supportsAnimation()) {
            QMovie *movie = new QMovie(filePath, QByteArray(), this);
            setMovie(movie);
        } else {
            m_image = reader.read();
        }
    } else if (image.typeId() == QMetaType::QPixmap) {
        m_image = image.value<QPixmap>().toImage();
    } else if (image.typeId() == QMetaType::QImage) {
        m_image = image.value<QImage>();
    } else {
        m_image = QImage();
    }

    if (!m_image.isNull()) {
        setFixedSize(m_image.size());
    }
    update();
}

void ImageLabel::scaledToWidth(int width)
{
    if (isNull()) return;

    int height = width * m_image.height() / m_image.width();
    setFixedSize(width, height);

    if (movie()) {
        movie()->setScaledSize(QSize(width, height));
    }
}

void ImageLabel::scaledToHeight(int height)
{
    if (isNull()) return;

    int width = height * m_image.width() / m_image.height();
    setFixedSize(width, height);

    if (movie()) {
        movie()->setScaledSize(QSize(width, height));
    }
}

void ImageLabel::setScaledSize(const QSize &size)
{
    if (isNull()) return;

    setFixedSize(size);

    if (movie()) {
        movie()->setScaledSize(size);
    }
}

bool ImageLabel::isNull() const
{
    return m_image.isNull();
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

QPixmap ImageLabel::pixmap() const
{
    return QPixmap::fromImage(m_image);
}

void ImageLabel::setMovie(QMovie *movie)
{
    QLabel::setMovie(movie);
    if (movie) {
        movie->start();
        m_image = movie->currentImage();
        connect(movie, &QMovie::frameChanged, this, &ImageLabel::_onFrameChanged);
    }
}

void ImageLabel::_onFrameChanged(int frameNumber)
{
    Q_UNUSED(frameNumber);
    if (movie()) {
        m_image = movie()->currentImage();
    }
    update();
}

void ImageLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (isNull()) {
        QLabel::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QPainterPath path;
    int w = width();
    int h = height();

    // 绘制圆角路径
    path.moveTo(m_topLeftRadius, 0);
    path.lineTo(w - m_topRightRadius, 0);

    if (m_topRightRadius > 0) {
        path.arcTo(w - 2 * m_topRightRadius, 0,
                   2 * m_topRightRadius, 2 * m_topRightRadius,
                   90, -90);
    }

    path.lineTo(w, h - m_bottomRightRadius);

    if (m_bottomRightRadius > 0) {
        path.arcTo(w - 2 * m_bottomRightRadius, h - 2 * m_bottomRightRadius,
                   2 * m_bottomRightRadius, 2 * m_bottomRightRadius,
                   0, -90);
    }

    path.lineTo(m_bottomLeftRadius, h);

    if (m_bottomLeftRadius > 0) {
        path.arcTo(0, h - 2 * m_bottomLeftRadius,
                   2 * m_bottomLeftRadius, 2 * m_bottomLeftRadius,
                   -90, -90);
    }

    path.lineTo(0, m_topLeftRadius);

    if (m_topLeftRadius > 0) {
        path.arcTo(0, 0,
                   2 * m_topLeftRadius, 2 * m_topLeftRadius,
                   -180, -90);
    }

    // 缩放图像
    qreal dpr = devicePixelRatioF();
    QImage scaledImage = m_image.scaled(
        size() * dpr, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(dpr);

    painter.setPen(Qt::NoPen);
    painter.setClipPath(path);
    painter.drawImage(rect(), scaledImage);
}

int ImageLabel::topLeftRadius() const { return m_topLeftRadius; }
void ImageLabel::setTopLeftRadius(int radius) { setBorderRadius(radius, m_topRightRadius, m_bottomLeftRadius, m_bottomRightRadius); }

int ImageLabel::topRightRadius() const { return m_topRightRadius; }
void ImageLabel::setTopRightRadius(int radius) { setBorderRadius(m_topLeftRadius, radius, m_bottomLeftRadius, m_bottomRightRadius); }

int ImageLabel::bottomLeftRadius() const { return m_bottomLeftRadius; }
void ImageLabel::setBottomLeftRadius(int radius) { setBorderRadius(m_topLeftRadius, m_topRightRadius, radius, m_bottomRightRadius); }

int ImageLabel::bottomRightRadius() const { return m_bottomRightRadius; }
void ImageLabel::setBottomRightRadius(int radius) { setBorderRadius(m_topLeftRadius, m_topRightRadius, m_bottomLeftRadius, radius); }

// AvatarWidget 实现
AvatarWidget::AvatarWidget(QWidget *parent)
    : ImageLabel(parent),
    m_radius(48),
    m_lightBackgroundColor(0, 0, 0, 50),
    m_darkBackgroundColor(255, 255, 255, 50)
{
    setRadius(m_radius);
}

AvatarWidget::AvatarWidget(const QString &image, QWidget *parent)
    : AvatarWidget(parent)
{
    setImage(image);
}

AvatarWidget::AvatarWidget(const QImage &image, QWidget *parent)
    : AvatarWidget(parent)
{
    setImage(image);
}

AvatarWidget::AvatarWidget(const QPixmap &image, QWidget *parent)
    : AvatarWidget(parent)
{
    setImage(image);
}

int AvatarWidget::radius() const { return m_radius; }

void AvatarWidget::setRadius(int radius)
{
    m_radius = radius;
    QFont font = this->font();
    font.setPixelSize(radius);
    setFont(font);
    setFixedSize(2 * radius, 2 * radius);
    update();
}

QColor AvatarWidget::lightBackgroundColor() const { return m_lightBackgroundColor; }
void AvatarWidget::setLightBackgroundColor(const QColor &color) { m_lightBackgroundColor = color; update(); }

QColor AvatarWidget::darkBackgroundColor() const { return m_darkBackgroundColor; }
void AvatarWidget::setDarkBackgroundColor(const QColor &color) { m_darkBackgroundColor = color; update(); }

void AvatarWidget::setBackgroundColor(const QColor &light, const QColor &dark)
{
    m_lightBackgroundColor = light;
    m_darkBackgroundColor = dark;
    update();
}

void AvatarWidget::setImage(const QVariant &image)
{
    ImageLabel::setImage(image);
    setRadius(m_radius);
}

void AvatarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    if (!isNull()) {
        _drawImageAvatar(painter);
    } else {
        _drawTextAvatar(painter);
    }
}

void AvatarWidget::_drawImageAvatar(QPainter &painter)
{
    qreal dpr = devicePixelRatioF();
    QSize targetSize = size() * dpr;

    // 居中裁剪图像
    QImage scaledImage = image().scaled(
        targetSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(dpr);

    int iw = scaledImage.width();
    int ih = scaledImage.height();
    int d = m_radius * 2 * dpr;
    int x = (iw - d) / 2;
    int y = (ih - d) / 2;

    QImage croppedImage = scaledImage.copy(x, y, d, d);

    // 绘制圆形图像
    QPainterPath path;
    path.addEllipse(rect());

    painter.setPen(Qt::NoPen);
    painter.setClipPath(path);
    painter.drawImage(rect(), croppedImage);
}

void AvatarWidget::_drawTextAvatar(QPainter &painter)
{
    if (text().isEmpty()) return;

    bool isDark = Theme::instance()->isDarkTheme();
    QColor bgColor = isDark ? m_darkBackgroundColor : m_lightBackgroundColor;

    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect());

    QColor textColor = isDark ? Qt::white : Qt::black;
    painter.setPen(textColor);
    painter.setFont(font());
    painter.drawText(rect(), Qt::AlignCenter, text().left(1).toUpper());
}
