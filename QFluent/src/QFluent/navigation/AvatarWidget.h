#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class ImageLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int topLeftRadius READ topLeftRadius WRITE setTopLeftRadius)
    Q_PROPERTY(int topRightRadius READ topRightRadius WRITE setTopRightRadius)
    Q_PROPERTY(int bottomLeftRadius READ bottomLeftRadius WRITE setBottomLeftRadius)
    Q_PROPERTY(int bottomRightRadius READ bottomRightRadius WRITE setBottomRightRadius)

public:
    explicit ImageLabel(QWidget *parent = nullptr);
    ImageLabel(const QString &image, QWidget *parent = nullptr);
    ImageLabel(const QImage &image, QWidget *parent = nullptr);
    ImageLabel(const QPixmap &image, QWidget *parent = nullptr);

    void setBorderRadius(int topLeft, int topRight, int bottomLeft, int bottomRight);

    void scaledToWidth(int width);
    void scaledToHeight(int height);
    void setScaledSize(const QSize &size);
    bool isNull() const;
    QPixmap pixmap() const;
    void setMovie(QMovie *movie);

    int topLeftRadius() const;
    void setTopLeftRadius(int radius);

    int topRightRadius() const;
    void setTopRightRadius(int radius);

    int bottomLeftRadius() const;
    void setBottomLeftRadius(int radius);

    int bottomRightRadius() const;
    void setBottomRightRadius(int radius);

    QImage image() const;
    void setImage(const QVariant &image);

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void _onFrameChanged(int frameNumber);

private:
    void _postInit();

    QImage m_image;
    int m_topLeftRadius;
    int m_topRightRadius;
    int m_bottomLeftRadius;
    int m_bottomRightRadius;
};

class AvatarWidget : public ImageLabel
{
    Q_OBJECT
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(QColor lightBackgroundColor READ lightBackgroundColor WRITE setLightBackgroundColor)
    Q_PROPERTY(QColor darkBackgroundColor READ darkBackgroundColor WRITE setDarkBackgroundColor)

public:
    explicit AvatarWidget(QWidget *parent = nullptr);
    AvatarWidget(const QString &image, QWidget *parent = nullptr);
    AvatarWidget(const QImage &image, QWidget *parent = nullptr);
    AvatarWidget(const QPixmap &image, QWidget *parent = nullptr);

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
    void _drawImageAvatar(QPainter &painter);
    void _drawTextAvatar(QPainter &painter);

    int m_radius;
    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;
};

#endif // IMAGELABEL_H
