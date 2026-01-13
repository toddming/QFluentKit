// ==================== GaussianBlur 优化实现 ====================
// 主要改进：
// 1. 修正 radius 到 sigma 的映射关系
// 2. 提供更精确的高斯模糊近似
// 3. 优化边界处理

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

    // 优化: 快速亮度调整
    if (brightFactor != 1.0) {
        adjustBrightness(image, brightFactor);
    }

    return QPixmap::fromImage(image);
}

QPixmap GaussianBlur::blur(const QString &imagePath, int radius, double brightFactor, const QSize &maxSize)
{
    QPixmap pixmap(imagePath);

    if (!maxSize.isNull() && !pixmap.isNull()) {
        pixmap = pixmap.scaled(maxSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    return blur(pixmap, radius, brightFactor);
}

QImage GaussianBlur::gaussianBlurImage(const QImage &source, int radius)
{
    QImage result = source.convertToFormat(QImage::Format_ARGB32);

    // 【关键修复】将 radius 转换为 sigma
    // 标准映射关系：sigma = radius / 3.0
    // 这样 radius=6 时，sigma=2，产生适中的模糊效果
    // 你可以调整这个系数：
    // - 除以 2.0：模糊效果稍强
    // - 除以 3.0：标准效果（推荐）
    // - 除以 4.0：模糊效果更弱
    double sigma = radius / 3.0;

    // 确保 sigma 至少为 0.5，避免过小的值导致无效果
    if (sigma < 0.5) {
        sigma = 0.5;
    }

    // 使用精确计算的盒式模糊半径
    std::vector<int> boxes = boxesForGauss(sigma, 3);

    for (int i = 0; i < 3; ++i) {
        boxBlur(result, boxes[i]);
    }

    return result;
}

// 根据高斯分布计算最优盒式模糊半径
// 参考论文: "Fast Almost-Gaussian Filtering" by Peter Kovesi
// 注意：这里的 sigma 是真正的高斯标准差，不是用户输入的 radius
std::vector<int> GaussianBlur::boxesForGauss(double sigma, int n)
{
    // 计算理想的盒宽
    double wIdeal = std::sqrt((12.0 * sigma * sigma / n) + 1.0);
    int wl = std::floor(wIdeal);
    if (wl % 2 == 0) wl--;  // 确保是奇数
    int wu = wl + 2;

    double mIdeal = (12.0 * sigma * sigma - n * wl * wl - 4 * n * wl - 3 * n) /
                    (-4.0 * wl - 4);
    int m = std::round(mIdeal);

    std::vector<int> sizes(n);
    for (int i = 0; i < n; i++) {
        // 返回半径值（(width - 1) / 2）
        sizes[i] = ((i < m ? wl : wu) - 1) / 2;
    }

    return sizes;
}

// 优化: 快速亮度调整，直接操作内存
void GaussianBlur::adjustBrightness(QImage &image, double factor)
{
    if (image.format() != QImage::Format_ARGB32 &&
        image.format() != QImage::Format_ARGB32_Premultiplied) {
        image = image.convertToFormat(QImage::Format_ARGB32);
    }

    uint32_t* pixels = reinterpret_cast<uint32_t*>(image.bits());
    int totalPixels = image.width() * image.height();

    // 使用查找表优化 (对于常见的亮度因子)
    if (factor > 0.5 && factor < 2.0) {
        uint8_t lut[256];
        for (int i = 0; i < 256; i++) {
            lut[i] = std::min(255, static_cast<int>(i * factor));
        }

        for (int i = 0; i < totalPixels; ++i) {
            uint32_t pixel = pixels[i];
            uint8_t r = lut[qRed(pixel)];
            uint8_t g = lut[qGreen(pixel)];
            uint8_t b = lut[qBlue(pixel)];
            pixels[i] = qRgba(r, g, b, qAlpha(pixel));
        }
    } else {
        // 直接计算
        for (int i = 0; i < totalPixels; ++i) {
            uint32_t pixel = pixels[i];
            int r = std::min(255, static_cast<int>(qRed(pixel) * factor));
            int g = std::min(255, static_cast<int>(qGreen(pixel) * factor));
            int b = std::min(255, static_cast<int>(qBlue(pixel) * factor));
            pixels[i] = qRgba(r, g, b, qAlpha(pixel));
        }
    }
}

void GaussianBlur::boxBlur(QImage &image, int radius)
{
    if (radius <= 0) return;

    int w = image.width();
    int h = image.height();
    int size = w * h;

    if (image.format() != QImage::Format_ARGB32 &&
        image.format() != QImage::Format_ARGB32_Premultiplied) {
        image = image.convertToFormat(QImage::Format_ARGB32);
    }

    uint32_t* pixels = reinterpret_cast<uint32_t*>(image.bits());
    std::vector<uint32_t> buffer(size);

    boxBlurHorizontal(pixels, buffer.data(), w, h, radius);
    boxBlurVertical(buffer.data(), pixels, w, h, radius);
}

// 优化: 使用整数运算代替浮点运算
void GaussianBlur::boxBlurHorizontal(uint32_t* src, uint32_t* dest, int w, int h, int r)
{
    // 使用定点数代替浮点数 (缩放 2^16)
    const int SCALE = 65536;
    int iarr = SCALE / (r + r + 1);

    for (int i = 0; i < h; i++) {
        int ti = i * w;
        int li = ti;
        int ri = ti + r;

        uint32_t fv = src[ti];
        uint32_t lv = src[ti + w - 1];

        // 使用 64 位整数避免溢出
        uint64_t val_r = qRed(fv) * (r + 1);
        uint64_t val_g = qGreen(fv) * (r + 1);
        uint64_t val_b = qBlue(fv) * (r + 1);
        uint64_t val_a = qAlpha(fv) * (r + 1);

        for (int j = 0; j < r; j++) {
            val_r += qRed(src[ti + j]);
            val_g += qGreen(src[ti + j]);
            val_b += qBlue(src[ti + j]);
            val_a += qAlpha(src[ti + j]);
        }

        for (int j = 0; j <= r; j++) {
            val_r += qRed(src[ri]) - qRed(fv);
            val_g += qGreen(src[ri]) - qGreen(fv);
            val_b += qBlue(src[ri]) - qBlue(fv);
            val_a += qAlpha(src[ri]) - qAlpha(fv);

            // 定点数除法
            dest[ti++] = qRgba(
                (val_r * iarr) >> 16,
                (val_g * iarr) >> 16,
                (val_b * iarr) >> 16,
                (val_a * iarr) >> 16
            );
            ri++;
        }

        for (int j = r + 1; j < w - r; j++) {
            val_r += qRed(src[ri]) - qRed(src[li]);
            val_g += qGreen(src[ri]) - qGreen(src[li]);
            val_b += qBlue(src[ri]) - qBlue(src[li]);
            val_a += qAlpha(src[ri]) - qAlpha(src[li]);

            dest[ti++] = qRgba(
                (val_r * iarr) >> 16,
                (val_g * iarr) >> 16,
                (val_b * iarr) >> 16,
                (val_a * iarr) >> 16
            );
            ri++; li++;
        }

        for (int j = w - r; j < w; j++) {
            val_r += qRed(lv) - qRed(src[li]);
            val_g += qGreen(lv) - qGreen(src[li]);
            val_b += qBlue(lv) - qBlue(src[li]);
            val_a += qAlpha(lv) - qAlpha(src[li]);

            dest[ti++] = qRgba(
                (val_r * iarr) >> 16,
                (val_g * iarr) >> 16,
                (val_b * iarr) >> 16,
                (val_a * iarr) >> 16
            );
            li++;
        }
    }
}

void GaussianBlur::boxBlurVertical(uint32_t* src, uint32_t* dest, int w, int h, int r)
{
    const int SCALE = 65536;
    int iarr = SCALE / (r + r + 1);

    for (int i = 0; i < w; i++) {
        int ti = i;
        int li = ti;
        int ri = ti + r * w;

        uint32_t fv = src[ti];
        uint32_t lv = src[ti + w * (h - 1)];

        uint64_t val_r = qRed(fv) * (r + 1);
        uint64_t val_g = qGreen(fv) * (r + 1);
        uint64_t val_b = qBlue(fv) * (r + 1);
        uint64_t val_a = qAlpha(fv) * (r + 1);

        for (int j = 0; j < r; j++) {
            val_r += qRed(src[ti + j * w]);
            val_g += qGreen(src[ti + j * w]);
            val_b += qBlue(src[ti + j * w]);
            val_a += qAlpha(src[ti + j * w]);
        }

        for (int j = 0; j <= r; j++) {
            val_r += qRed(src[ri]) - qRed(fv);
            val_g += qGreen(src[ri]) - qGreen(fv);
            val_b += qBlue(src[ri]) - qBlue(fv);
            val_a += qAlpha(src[ri]) - qAlpha(fv);

            dest[ti] = qRgba(
                (val_r * iarr) >> 16,
                (val_g * iarr) >> 16,
                (val_b * iarr) >> 16,
                (val_a * iarr) >> 16
            );
            ri += w; ti += w;
        }

        for (int j = r + 1; j < h - r; j++) {
            val_r += qRed(src[ri]) - qRed(src[li]);
            val_g += qGreen(src[ri]) - qGreen(src[li]);
            val_b += qBlue(src[ri]) - qBlue(src[li]);
            val_a += qAlpha(src[ri]) - qAlpha(src[li]);

            dest[ti] = qRgba(
                (val_r * iarr) >> 16,
                (val_g * iarr) >> 16,
                (val_b * iarr) >> 16,
                (val_a * iarr) >> 16
            );
            li += w; ri += w; ti += w;
        }

        for (int j = h - r; j < h; j++) {
            val_r += qRed(lv) - qRed(src[li]);
            val_g += qGreen(lv) - qGreen(src[li]);
            val_b += qBlue(lv) - qBlue(src[li]);
            val_a += qAlpha(lv) - qAlpha(src[li]);

            dest[ti] = qRgba(
                (val_r * iarr) >> 16,
                (val_g * iarr) >> 16,
                (val_b * iarr) >> 16,
                (val_a * iarr) >> 16
            );
            li += w; ti += w;
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

int AcrylicLabel::blurRadius() const
{
    return m_blurRadius;
}

void AcrylicLabel::onBlurFinished(const QPixmap &blurPixmap) {
    m_blurPixmap = blurPixmap;

    if (!m_blurPixmap.isNull()) {
        if (width() > 0 && height() > 0) {
            setPixmap(m_blurPixmap.scaled(
                size(),
                Qt::KeepAspectRatioByExpanding,
                Qt::SmoothTransformation));
        } else {
            setPixmap(m_blurPixmap);
        }
    }

    update();
}

void AcrylicLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    m_acrylicTextureLabel->resize(size());

    if (!m_blurPixmap.isNull() && size().width() > 0 && size().height() > 0) {
        setPixmap(m_blurPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
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
