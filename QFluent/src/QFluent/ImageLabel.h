#pragma once
#include <QLabel>
#include <QColor>

#include "FluentGlobal.h"
// 前置声明
class QImage;
class QPixmap;
class QMovie;
class QVariant;
class QPainter;

/**
 * @brief 支持圆角和动画的图片标签
 */
class QFLUENT_EXPORT ImageLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int topLeftRadius READ topLeftRadius WRITE setTopLeftRadius)
    Q_PROPERTY(int topRightRadius READ topRightRadius WRITE setTopRightRadius)
    Q_PROPERTY(int bottomLeftRadius READ bottomLeftRadius WRITE setBottomLeftRadius)
    Q_PROPERTY(int bottomRightRadius READ bottomRightRadius WRITE setBottomRightRadius)

public:
    explicit ImageLabel(QWidget *parent = nullptr);
    ImageLabel(const QString &imagePath, QWidget *parent = nullptr);
    ImageLabel(const QImage &image, QWidget *parent = nullptr);
    ImageLabel(const QPixmap &pixmap, QWidget *parent = nullptr);
    ~ImageLabel() override = default;

    // 圆角设置
    void setBorderRadius(int topLeft, int topRight, int bottomLeft, int bottomRight);

    int topLeftRadius() const;
    void setTopLeftRadius(int radius);

    int topRightRadius() const;
    void setTopRightRadius(int radius);

    int bottomLeftRadius() const;
    void setBottomLeftRadius(int radius);

    int bottomRightRadius() const;
    void setBottomRightRadius(int radius);

    // 图片操作
    QImage image() const;
    void setImage(const QVariant &image);
    QPixmap pixmap() const;
    void setMovie(QMovie *movie);

    // 缩放操作
    void scaledToWidth(int width);
    void scaledToHeight(int height);
    void setScaledSize(const QSize &size);

    bool isNull() const;

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onFrameChanged(int frameNumber);

private:
    void postInit();

    QImage m_image;
    int m_topLeftRadius;
    int m_topRightRadius;
    int m_bottomLeftRadius;
    int m_bottomRightRadius;
};

/**
 * @brief 圆形头像控件
 * 支持显示图片或文本首字母
 */
class QFLUENT_EXPORT AvatarWidget : public ImageLabel
{
    Q_OBJECT
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(QColor lightBackgroundColor READ lightBackgroundColor WRITE setLightBackgroundColor)
    Q_PROPERTY(QColor darkBackgroundColor READ darkBackgroundColor WRITE setDarkBackgroundColor)

public:
    explicit AvatarWidget(QWidget *parent = nullptr);
    AvatarWidget(const QString &imagePath, QWidget *parent = nullptr);
    AvatarWidget(const QImage &image, QWidget *parent = nullptr);
    AvatarWidget(const QPixmap &pixmap, QWidget *parent = nullptr);
    ~AvatarWidget() override = default;

    int radius() const;
    void setRadius(int radius);

    QColor lightBackgroundColor() const;
    void setLightBackgroundColor(const QColor &color);

    QColor darkBackgroundColor() const;
    void setDarkBackgroundColor(const QColor &color);

    void setBackgroundColor(const QColor &light, const QColor &dark);
    void setImage(const QVariant &image);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawImageAvatar(QPainter &painter);
    void drawTextAvatar(QPainter &painter);

    int m_radius;
    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;
};
