#include "ImageLabel.h"
#include "Theme.h"

#include <QImageReader>
#include <QPainter>
#include <QMouseEvent>
#include <QFont>
#include <QImage>
#include <QPixmap>
#include <QMovie>
#include <QPainterPath>
#include <QVariant>

// ============================================================================
// ImageLabel 实现
// ============================================================================

ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent)
    , m_topLeftRadius(0)
    , m_topRightRadius(0)
    , m_bottomLeftRadius(0)
    , m_bottomRightRadius(0)
{
}

ImageLabel::ImageLabel(const QString &imagePath, QWidget *parent)
    : ImageLabel(parent)
{
    setImage(imagePath);
}

ImageLabel::ImageLabel(const QImage &image, QWidget *parent)
    : ImageLabel(parent)
{
    setImage(image);
}

ImageLabel::ImageLabel(const QPixmap &pixmap, QWidget *parent)
    : ImageLabel(parent)
{
    setImage(pixmap);
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
    // 清理之前的movie
    if (movie()) {
        movie()->disconnect(this);
        movie()->deleteLater();
        QLabel::setMovie(nullptr);
    }

    if (image.isNull()) {
        m_image = QImage();
    }
    else if (image.userType() == QMetaType::QString) {
        QString filePath = image.toString();

        // 验证文件路径：防止路径遍历攻击
        if (filePath.contains("..")) {
            qWarning("ImageLabel::setImage: Path traversal detected in '%s'", qPrintable(filePath));
            m_image = QImage();
            update();
            return;
        }

        QImageReader reader(filePath);

        if (reader.supportsAnimation()) {
            // 处理动画GIF
            QMovie *newMovie = new QMovie(filePath, QByteArray(), this);
            setMovie(newMovie);
            return; // movie设置会更新m_image
        }
        else {
            m_image = reader.read();
        }
    }
    else if (image.userType() == QMetaType::QPixmap) {
        m_image = image.value<QPixmap>().toImage();
    }
    else if (image.userType() == QMetaType::QImage) {
        m_image = image.value<QImage>();
    }
    else {
        m_image = QImage();
    }

    if (!m_image.isNull()) {
        setFixedSize(m_image.size());
    }
    update();
}

void ImageLabel::scaledToWidth(int width)
{
    if (isNull()) {
        return;
    }

    int height = width * m_image.height() / m_image.width();
    setFixedSize(width, height);

    if (movie()) {
        movie()->setScaledSize(QSize(width, height));
    }
}

void ImageLabel::scaledToHeight(int height)
{
    if (isNull()) {
        return;
    }

    int width = height * m_image.width() / m_image.height();
    setFixedSize(width, height);

    if (movie()) {
        movie()->setScaledSize(QSize(width, height));
    }
}

void ImageLabel::setScaledSize(const QSize &size)
{
    if (isNull()) {
        return;
    }

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
    if (!movie) {
        return;
    }

    QLabel::setMovie(movie);
    movie->start();
    m_image = movie->currentImage();
    connect(movie, &QMovie::frameChanged, this, &ImageLabel::onFrameChanged);
}

void ImageLabel::onFrameChanged(int frameNumber)
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
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // 构建圆角路径
    QPainterPath path;
    const int w = width();
    const int h = height();

    // 顶部线条
    path.moveTo(m_topLeftRadius, 0);
    path.lineTo(w - m_topRightRadius, 0);

    // 右上角圆弧
    if (m_topRightRadius > 0) {
        const int d = 2 * m_topRightRadius;
        path.arcTo(w - d, 0, d, d, 90, -90);
    }

    // 右侧线条
    path.lineTo(w, h - m_bottomRightRadius);

    // 右下角圆弧
    if (m_bottomRightRadius > 0) {
        const int d = 2 * m_bottomRightRadius;
        path.arcTo(w - d, h - d, d, d, 0, -90);
    }

    // 底部线条
    path.lineTo(m_bottomLeftRadius, h);

    // 左下角圆弧
    if (m_bottomLeftRadius > 0) {
        const int d = 2 * m_bottomLeftRadius;
        path.arcTo(0, h - d, d, d, -90, -90);
    }

    // 左侧线条
    path.lineTo(0, m_topLeftRadius);

    // 左上角圆弧
    if (m_topLeftRadius > 0) {
        const int d = 2 * m_topLeftRadius;
        path.arcTo(0, 0, d, d, 180, -90);
    }

    // 缩放图像以适应高DPI屏幕
    const qreal dpr = devicePixelRatioF();
    QImage scaledImage = m_image.scaled(
        size() * dpr,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    );
    scaledImage.setDevicePixelRatio(dpr);

    // 绘制裁剪后的图像
    painter.setPen(Qt::NoPen);
    painter.setClipPath(path);
    painter.drawImage(rect(), scaledImage);
}

// 圆角属性访问器
int ImageLabel::topLeftRadius() const
{
    return m_topLeftRadius;
}

void ImageLabel::setTopLeftRadius(int radius)
{
    setBorderRadius(radius, m_topRightRadius, m_bottomLeftRadius, m_bottomRightRadius);
}

int ImageLabel::topRightRadius() const
{
    return m_topRightRadius;
}

void ImageLabel::setTopRightRadius(int radius)
{
    setBorderRadius(m_topLeftRadius, radius, m_bottomLeftRadius, m_bottomRightRadius);
}

int ImageLabel::bottomLeftRadius() const
{
    return m_bottomLeftRadius;
}

void ImageLabel::setBottomLeftRadius(int radius)
{
    setBorderRadius(m_topLeftRadius, m_topRightRadius, radius, m_bottomRightRadius);
}

int ImageLabel::bottomRightRadius() const
{
    return m_bottomRightRadius;
}

void ImageLabel::setBottomRightRadius(int radius)
{
    setBorderRadius(m_topLeftRadius, m_topRightRadius, m_bottomLeftRadius, radius);
}

// ============================================================================
// AvatarWidget 实现
// ============================================================================

AvatarWidget::AvatarWidget(QWidget *parent)
    : ImageLabel(parent)
    , m_radius(48)
    , m_lightBackgroundColor(0, 0, 0, 50)
    , m_darkBackgroundColor(255, 255, 255, 50)
{
    setRadius(m_radius);
}

AvatarWidget::AvatarWidget(const QString &imagePath, QWidget *parent)
    : AvatarWidget(parent)
{
    setImage(imagePath);
}

AvatarWidget::AvatarWidget(const QImage &image, QWidget *parent)
    : AvatarWidget(parent)
{
    setImage(image);
}

AvatarWidget::AvatarWidget(const QPixmap &pixmap, QWidget *parent)
    : AvatarWidget(parent)
{
    setImage(pixmap);
}

int AvatarWidget::radius() const
{
    return m_radius;
}

void AvatarWidget::setRadius(int radius)
{
    m_radius = radius;

    // 设置字体大小与半径相同
    QFont font = this->font();
    font.setPixelSize(radius);
    setFont(font);

    setFixedSize(2 * radius, 2 * radius);
    update();
}

QColor AvatarWidget::lightBackgroundColor() const
{
    return m_lightBackgroundColor;
}

void AvatarWidget::setLightBackgroundColor(const QColor &color)
{
    m_lightBackgroundColor = color;
    update();
}

QColor AvatarWidget::darkBackgroundColor() const
{
    return m_darkBackgroundColor;
}

void AvatarWidget::setDarkBackgroundColor(const QColor &color)
{
    m_darkBackgroundColor = color;
    update();
}

void AvatarWidget::setBackgroundColor(const QColor &light, const QColor &dark)
{
    m_lightBackgroundColor = light;
    m_darkBackgroundColor = dark; // 修复:原代码错误地设置了light
    update();
}

void AvatarWidget::setImage(const QVariant &image)
{
    ImageLabel::setImage(image);
    setRadius(m_radius); // 重新应用半径以更新尺寸
}

void AvatarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (!isNull()) {
        drawImageAvatar(painter);
    } else {
        drawTextAvatar(painter);
    }
}

void AvatarWidget::drawImageAvatar(QPainter &painter)
{
    const qreal dpr = devicePixelRatioF();
    const QSize targetSize = size() * (dpr + 0.1);

    // 按比例扩展缩放图像(保持宽高比,填充整个区域)
    QImage scaledImage = image().scaled(
        targetSize,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
    );
    scaledImage.setDevicePixelRatio(dpr);

    // 中心裁剪
    const int iw = scaledImage.width();
    const int ih = scaledImage.height();
    const int d = m_radius * 2 * dpr;
    const int x = (iw - d) / 2;
    const int y = (ih - d) / 2;

    QImage croppedImage = scaledImage.copy(x, y, d, d);

    // 绘制圆形裁剪路径
    QPainterPath path;
    path.addEllipse(rect());

    painter.setPen(Qt::NoPen);
    painter.setClipPath(path);
    painter.drawImage(rect(), croppedImage);
}

void AvatarWidget::drawTextAvatar(QPainter &painter)
{
    if (text().isEmpty()) {
        return;
    }

    // 根据主题选择背景色
    const bool isDark = Theme::isDark();
    const QColor bgColor = isDark ? m_darkBackgroundColor : m_lightBackgroundColor;

    // 绘制圆形背景
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect());

    // 绘制文本首字母(大写)
    const QColor textColor = isDark ? Qt::white : Qt::black;
    painter.setPen(textColor);
    painter.setFont(font());
    painter.drawText(rect(), Qt::AlignCenter, text().left(1).toUpper());
}
