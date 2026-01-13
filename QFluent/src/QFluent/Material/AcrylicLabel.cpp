#include "AcrylicLabel.h"
#include <QPainter>
#include <QBrush>
#include <QScreen>
#include <QApplication>
#include <QWindow>
#include <QtMath>
#include <QRandomGenerator>
#include <QPixmap>
#include <QImage>
#include <QColor>
#include <QPainterPath>
#include <QRect>
#include <QSize>

#include "Screen.h"

// ==================== GaussianBlur 实现 ====================

QPixmap GaussianBlur::blur(const QPixmap &source, int radius, double brightFactor)
{
    if (source.isNull() || radius <= 0) {
        return source;
    }
    
    QImage image = source.toImage();
    image = gaussianBlurImage(image, radius);
    
    // 应用亮度因子
    if (brightFactor != 1.0) {
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                QRgb pixel = image.pixel(x, y);
                int r = qMin(255, int(qRed(pixel) * brightFactor));
                int g = qMin(255, int(qGreen(pixel) * brightFactor));
                int b = qMin(255, int(qBlue(pixel) * brightFactor));
                image.setPixel(x, y, qRgba(r, g, b, qAlpha(pixel)));
            }
        }
    }
    
    return QPixmap::fromImage(image);
}

QPixmap GaussianBlur::blur(const QString &imagePath, int radius, double brightFactor, const QSize &maxSize)
{
    QPixmap pixmap(imagePath);
    
    if (!maxSize.isNull() && !pixmap.isNull()) {
        pixmap = pixmap.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    return blur(pixmap, radius, brightFactor);
}

QImage GaussianBlur::gaussianBlurImage(const QImage &source, int radius)
{
    QImage result = source.convertToFormat(QImage::Format_ARGB32);
    
    // 使用快速盒式模糊近似高斯模糊 (3次盒式模糊)
    for (int i = 0; i < 3; ++i) {
        boxBlur(result, radius / 3);
    }
    
    return result;
}

void GaussianBlur::boxBlur(QImage &image, int radius)
{
    if (radius <= 0) return;
    
    boxBlurHorizontal(image, radius);
    boxBlurVertical(image, radius);
}

void GaussianBlur::boxBlurHorizontal(QImage &image, int radius)
{
    int width = image.width();
    int height = image.height();
    
    for (int y = 0; y < height; ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));
        QVector<QRgb> temp(width);
        
        for (int x = 0; x < width; ++x) {
            int r = 0, g = 0, b = 0, a = 0;
            int count = 0;
            
            for (int i = -radius; i <= radius; ++i) {
                int xi = qBound(0, x + i, width - 1);
                QRgb pixel = line[xi];
                r += qRed(pixel);
                g += qGreen(pixel);
                b += qBlue(pixel);
                a += qAlpha(pixel);
                count++;
            }
            
            temp[x] = qRgba(r / count, g / count, b / count, a / count);
        }
        
        for (int x = 0; x < width; ++x) {
            line[x] = temp[x];
        }
    }
}

void GaussianBlur::boxBlurVertical(QImage &image, int radius)
{
    int width = image.width();
    int height = image.height();
    
    for (int x = 0; x < width; ++x) {
        QVector<QRgb> temp(height);
        
        for (int y = 0; y < height; ++y) {
            int r = 0, g = 0, b = 0, a = 0;
            int count = 0;
            
            for (int i = -radius; i <= radius; ++i) {
                int yi = qBound(0, y + i, height - 1);
                QRgb pixel = image.pixel(x, yi);
                r += qRed(pixel);
                g += qGreen(pixel);
                b += qBlue(pixel);
                a += qAlpha(pixel);
                count++;
            }
            
            temp[y] = qRgba(r / count, g / count, b / count, a / count);
        }
        
        for (int y = 0; y < height; ++y) {
            image.setPixel(x, y, temp[y]);
        }
    }
}

// ==================== BlurCoverThread 实现 ====================

BlurCoverThread::BlurCoverThread(QObject *parent)
    : QThread(parent)
    , m_blurRadius(7)
    , m_maxSize(450, 450)
{
}

void BlurCoverThread::blur(const QString &imagePath, int blurRadius, const QSize &maxSize)
{
    m_imagePath = imagePath;
    m_blurRadius = blurRadius;
    if (!maxSize.isNull()) {
        m_maxSize = maxSize;
    }
    start();
}

void BlurCoverThread::run()
{
    if (m_imagePath.isEmpty()) {
        return;
    }
    
    QPixmap pixmap = GaussianBlur::blur(m_imagePath, m_blurRadius, 0.85, m_maxSize);
    emit blurFinished(pixmap);
}

// ==================== AcrylicTextureLabel 实现 ====================

AcrylicTextureLabel::AcrylicTextureLabel(const QColor &tintColor,
                                       const QColor &luminosityColor,
                                       double noiseOpacity,
                                       QWidget *parent)
    : QLabel(parent)
    , m_tintColor(tintColor)
    , m_luminosityColor(luminosityColor)
    , m_noiseOpacity(noiseOpacity)
{
    setAttribute(Qt::WA_TranslucentBackground);
    m_noiseImage = createNoiseImage();
}

void AcrylicTextureLabel::setTintColor(const QColor &color)
{
    m_tintColor = color;
    update();
}

QImage AcrylicTextureLabel::createNoiseImage()
{
    return QImage(":/res/images/acrylic/noise.png");
}

void AcrylicTextureLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    // 创建亚克力纹理
    QImage acrylicTexture(64, 64, QImage::Format_ARGB32_Premultiplied);
    
    // 填充亮度层
    acrylicTexture.fill(m_luminosityColor);
    
    QPainter painter(&acrylicTexture);
    
    // 绘制色调层
    painter.fillRect(acrylicTexture.rect(), m_tintColor);
    
    // 绘制噪声层
    painter.setOpacity(m_noiseOpacity);
    painter.drawImage(acrylicTexture.rect(), m_noiseImage);
    
    painter.end();
    
    // 使用纹理画刷填充控件
    QBrush acrylicBrush(acrylicTexture);
    QPainter widgetPainter(this);
    widgetPainter.fillRect(rect(), acrylicBrush);
}

// ==================== AcrylicLabel 实现 ====================

AcrylicLabel::AcrylicLabel(int blurRadius,
                         const QColor &tintColor,
                         const QColor &luminosityColor,
                         const QSize &maxBlurSize,
                         QWidget *parent)
    : QLabel(parent)
    , m_blurRadius(blurRadius)
    , m_maxBlurSize(maxBlurSize)
{
    m_acrylicTextureLabel = new AcrylicTextureLabel(tintColor, luminosityColor, 0.03, this);
    m_blurThread = new BlurCoverThread(this);
    
    connect(m_blurThread, &BlurCoverThread::blurFinished,
            this, &AcrylicLabel::onBlurFinished);
}

AcrylicLabel::~AcrylicLabel()
{
    if (m_blurThread->isRunning()) {
        m_blurThread->quit();
        m_blurThread->wait();
    }
}

void AcrylicLabel::setImage(const QString &imagePath)
{
    m_imagePath = imagePath;
    m_blurThread->blur(imagePath, m_blurRadius, m_maxBlurSize);
}

void AcrylicLabel::setTintColor(const QColor &color)
{
    m_acrylicTextureLabel->setTintColor(color);
}

void AcrylicLabel::setBlurRadius(int value)
{
    m_blurRadius = value;
}

void AcrylicLabel::onBlurFinished(const QPixmap &blurPixmap)
{
    m_blurPixmap = blurPixmap;
    setPixmap(m_blurPixmap);
    adjustSize();
}

void AcrylicLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    m_acrylicTextureLabel->resize(size());
    
    if (!m_blurPixmap.isNull() && m_blurPixmap.size() != size()) {
        setPixmap(m_blurPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, 
                                     Qt::SmoothTransformation));
    }
}

// ==================== AcrylicBrush 实现 ====================

AcrylicBrush::AcrylicBrush(QWidget *device,
                         int blurRadius,
                         const QColor &tintColor,
                         const QColor &luminosityColor,
                         double noiseOpacity)
    : m_device(device)
    , m_blurRadius(blurRadius)
    , m_tintColor(tintColor)
    , m_luminosityColor(luminosityColor)
    , m_noiseOpacity(noiseOpacity)
{
    m_noiseImage = createNoiseImage();
}

QImage AcrylicBrush::createNoiseImage()
{
    return QImage(":/res/images/acrylic/noise.png");
}

void AcrylicBrush::setBlurRadius(int radius)
{
    if (radius == m_blurRadius) {
        return;
    }
    
    m_blurRadius = radius;
    if (!m_originalImage.isNull()) {
        setImage(m_originalImage);
    }
}

void AcrylicBrush::setTintColor(const QColor &color)
{
    m_tintColor = color;
    if (m_device) {
        m_device->update();
    }
}

void AcrylicBrush::setLuminosityColor(const QColor &color)
{
    m_luminosityColor = color;
    if (m_device) {
        m_device->update();
    }
}

void AcrylicBrush::grabImage(const QRect &rect)
{
    QScreen *screen = Screen::getCurrentScreen();
    if (!screen) {
        screen = QApplication::primaryScreen();
    }

    if (!screen) {
        return;
    }

    QRect screenGeometry = screen->geometry();
    int x = rect.x() - screenGeometry.x();
    int y = rect.y() - screenGeometry.y();

    QPixmap grabbed = screen->grabWindow(0, x, y, rect.width(), rect.height());
    setImage(grabbed);
}

void AcrylicBrush::setImage(const QPixmap &image)
{
    m_originalImage = image;
    
    if (!image.isNull()) {
        m_image = GaussianBlur::blur(image, m_blurRadius);
    }
    
    if (m_device) {
        m_device->update();
    }
}

void AcrylicBrush::setImage(const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    setImage(pixmap);
}

void AcrylicBrush::setClipPath(const QPainterPath &path)
{
    m_clipPath = path;
    if (m_device) {
        m_device->update();
    }
}

QImage AcrylicBrush::textureImage()
{
    QImage texture(64, 64, QImage::Format_ARGB32_Premultiplied);
    texture.fill(m_luminosityColor);
    
    QPainter painter(&texture);
    painter.fillRect(texture.rect(), m_tintColor);
    
    painter.setOpacity(m_noiseOpacity);
    painter.drawImage(texture.rect(), m_noiseImage);
    
    return texture;
}

void AcrylicBrush::paint()
{
    if (!m_device) {
        return;
    }

    QPainter painter(m_device);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!m_clipPath.isEmpty()) {
        painter.setClipPath(m_clipPath);
    }

    // 绘制模糊图像
    if (!m_image.isNull()) {
        QPixmap imageToDraw = m_image;
        imageToDraw.setDevicePixelRatio(1.0);
        painter.drawPixmap(m_device->rect(), imageToDraw);
    }

    // 绘制亚克力纹理
    painter.fillRect(m_device->rect(), QBrush(textureImage()));
}
