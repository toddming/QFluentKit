#pragma once

#include <QLabel>
#include <QThread>
#include <QPainterPath>

#include "FluentGlobal.h"

class QWidget;

// 高斯模糊工具类
class GaussianBlur
{
public:
    static QPixmap blur(const QPixmap &source, int radius, double brightFactor = 1.0);
    static QPixmap blur(const QString &imagePath, int radius, double brightFactor = 1.0,
                       const QSize &maxSize = QSize());
    
private:
    static QImage gaussianBlurImage(const QImage &source, int radius);
    static void boxBlur(QImage &image, int radius);
    static void boxBlurHorizontal(QImage &image, int radius);
    static void boxBlurVertical(QImage &image, int radius);
};

// 模糊线程类
class BlurCoverThread : public QThread
{
    Q_OBJECT
    
public:
    explicit BlurCoverThread(QObject *parent = nullptr);
    
    void blur(const QString &imagePath, int blurRadius = 6, const QSize &maxSize = QSize(450, 450));
    
signals:
    void blurFinished(const QPixmap &pixmap);
    
protected:
    void run() override;
    
private:
    QString m_imagePath;
    int m_blurRadius;
    QSize m_maxSize;
};

// 亚克力纹理标签
class QFLUENT_EXPORT AcrylicTextureLabel : public QLabel
{
    Q_OBJECT
    
public:
    explicit AcrylicTextureLabel(const QColor &tintColor, 
                                const QColor &luminosityColor,
                                double noiseOpacity = 0.03,
                                QWidget *parent = nullptr);
    
    void setTintColor(const QColor &color);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    QColor m_tintColor;
    QColor m_luminosityColor;
    double m_noiseOpacity;
    QImage m_noiseImage;
    
    QImage createNoiseImage();
};

// 亚克力标签主类
class QFLUENT_EXPORT AcrylicLabel : public QLabel
{
    Q_OBJECT
    
public:
    explicit AcrylicLabel(int blurRadius, 
                         const QColor &tintColor,
                         const QColor &luminosityColor = QColor(255, 255, 255, 0),
                         const QSize &maxBlurSize = QSize(),
                         QWidget *parent = nullptr);
    
    ~AcrylicLabel();
    
    void setImage(const QString &imagePath);
    void setTintColor(const QColor &color);

    void setBlurRadius(int value);
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    
private slots:
    void onBlurFinished(const QPixmap &blurPixmap);
    
private:
    QString m_imagePath;
    QPixmap m_blurPixmap;
    int m_blurRadius;
    QSize m_maxBlurSize;
    AcrylicTextureLabel *m_acrylicTextureLabel;
    BlurCoverThread *m_blurThread;
};

// 亚克力画刷类
class QFLUENT_EXPORT AcrylicBrush
{
public:
    explicit AcrylicBrush(QWidget *device, 
                         int blurRadius,
                         const QColor &tintColor = QColor(242, 242, 242, 150),
                         const QColor &luminosityColor = QColor(255, 255, 255, 10),
                         double noiseOpacity = 0.02);
    
    void setBlurRadius(int radius);
    void setTintColor(const QColor &color);
    void setLuminosityColor(const QColor &color);
    
    void grabImage(const QRect &rect);
    void setImage(const QPixmap &image);
    void setImage(const QString &imagePath);
    void setClipPath(const QPainterPath &path);
    
    void paint();
    QImage textureImage();
    
private:
    QWidget *m_device;
    int m_blurRadius;
    QColor m_tintColor;
    QColor m_luminosityColor;
    double m_noiseOpacity;
    QImage m_noiseImage;
    QPixmap m_originalImage;
    QPixmap m_image;
    QPainterPath m_clipPath;
    
    QImage createNoiseImage();
};
