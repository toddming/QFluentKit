#ifndef INFO_BAR_H
#define INFO_BAR_H

#include <QFrame>

#include "Define.h"

class QLabel;
class QBoxLayout;
class QHBoxLayout;
class TransparentToolButton;
class QGraphicsOpacityEffect;
class QPropertyAnimation;
class QParallelAnimationGroup;

class QFLUENT_EXPORT InfoIconWidget : public QWidget {
    Q_OBJECT
public:
    explicit InfoIconWidget(InfoBarType::BarType type, QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;

private:
    InfoBarType::BarType m_type;
};

class QFLUENT_EXPORT InfoBar : public QFrame {
    Q_OBJECT
public:
    explicit InfoBar(InfoBarType::BarType type, const QString& title, const QString& content,
                     Qt::Orientation orient = Qt::Horizontal, bool isClosable = true,
                     int duration = 1000, InfoBarType::BarPosition position = InfoBarType::BarPosition::TOP_RIGHT,
                     QWidget* parent = nullptr);

    void addWidget(QWidget* widget, int stretch = 0);
    void setCustomBackgroundColor(const QColor& light, const QColor& dark);
    bool eventFilter(QObject* obj, QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    static InfoBar* newInfoBar(InfoBarType::BarType type, const QString& title, const QString& content,
                               Qt::Orientation orient = Qt::Horizontal, bool isClosable = true,
                               int duration = 1000, InfoBarType::BarPosition position = InfoBarType::BarPosition::TOP_RIGHT,
                               QWidget* parent = nullptr);
    static InfoBar* info(const QString& title, const QString& content,
                         Qt::Orientation orient = Qt::Horizontal, bool isClosable = true,
                         int duration = 1000, InfoBarType::BarPosition position = InfoBarType::BarPosition::TOP_RIGHT,
                         QWidget* parent = nullptr);
    static InfoBar* success(const QString& title, const QString& content,
                            Qt::Orientation orient = Qt::Horizontal, bool isClosable = true,
                            int duration = 1000, InfoBarType::BarPosition position = InfoBarType::BarPosition::TOP_RIGHT,
                            QWidget* parent = nullptr);
    static InfoBar* warning(const QString& title, const QString& content,
                            Qt::Orientation orient = Qt::Horizontal, bool isClosable = true,
                            int duration = 1000, InfoBarType::BarPosition position = InfoBarType::BarPosition::TOP_RIGHT,
                            QWidget* parent = nullptr);
    static InfoBar* error(const QString& title, const QString& content,
                          Qt::Orientation orient = Qt::Horizontal, bool isClosable = true,
                          int duration = 1000, InfoBarType::BarPosition position = InfoBarType::BarPosition::TOP_RIGHT,
                          QWidget* parent = nullptr);

signals:
    void closedSignal();

private:
    void __initWidget();
    void __initLayout();
    void __setQss();
    void __fadeOut();
    void _adjustText();

    QString m_title;
    QString m_content;
    Qt::Orientation m_orient;
    InfoBarType::BarType m_type;
    int m_duration;
    bool m_isClosable;
    InfoBarType::BarPosition m_position;

    QLabel* m_titleLabel;
    QLabel* m_contentLabel;
    TransparentToolButton* m_closeButton;
    InfoIconWidget* m_iconWidget;

    QHBoxLayout* m_hBoxLayout;
    QBoxLayout* m_textLayout;
    QBoxLayout* m_widgetLayout;

    QGraphicsOpacityEffect* m_opacityEffect;
    QPropertyAnimation* m_opacityAni;

    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;
};

class QFLUENT_EXPORT InfoBarManager : public QObject {
    Q_OBJECT
public:

    void add(InfoBar* infoBar);
    void remove(InfoBar* infoBar);

    static void registerManager(InfoBarType::BarPosition position, std::function<InfoBarManager*()> creator);
    static InfoBarManager* make(InfoBarType::BarPosition position);
    static QString toString(InfoBarType::BarType type);

protected:
    explicit InfoBarManager(QObject* parent = nullptr);
    virtual QPoint _pos(InfoBar* infoBar, const QSize& parentSize = QSize()) = 0;
    virtual QPoint _slideStartPos(InfoBar* infoBar) = 0;

    QPropertyAnimation* _createSlideAni(InfoBar* infoBar);
    void _updateDropAni(QWidget* parent);

    bool eventFilter(QObject* obj, QEvent* event) override;

    int m_spacing = 16;
    int m_margin = 24;
    QMap<QWidget*, QList<QPointer<InfoBar>>> m_infoBars;
    QList<QPropertyAnimation*> m_slideAnis;
    QList<QPropertyAnimation*> m_dropAnis;

    static QMap<InfoBarType::BarPosition, std::function<InfoBarManager*()>> m_managers;
};

class QFLUENT_EXPORT TopInfoBarManager : public InfoBarManager {
    Q_OBJECT
public:
    static TopInfoBarManager* getInstance() {
        if (!s_instance) {
            s_instance = new TopInfoBarManager();
        }
        return s_instance;
    }

protected:
    QPoint _pos(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint _slideStartPos(InfoBar* infoBar) override;

private:
    static TopInfoBarManager* s_instance;
};

class QFLUENT_EXPORT TopRightInfoBarManager : public InfoBarManager {
    Q_OBJECT
public:
    static TopRightInfoBarManager* getInstance() {
        if (!s_instance) {
            s_instance = new TopRightInfoBarManager();
        }
        return s_instance;
    }

protected:
    QPoint _pos(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint _slideStartPos(InfoBar* infoBar) override;

private:
    static TopRightInfoBarManager* s_instance;

};

class QFLUENT_EXPORT BottomRightInfoBarManager : public InfoBarManager {
    Q_OBJECT
public:
    static BottomRightInfoBarManager* getInstance() {
        if (!s_instance) {
            s_instance = new BottomRightInfoBarManager();
        }
        return s_instance;
    }

protected:
    QPoint _pos(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint _slideStartPos(InfoBar* infoBar) override;

private:
    static BottomRightInfoBarManager* s_instance;

};

class QFLUENT_EXPORT TopLeftInfoBarManager : public InfoBarManager {
    Q_OBJECT
public:
    static TopLeftInfoBarManager* getInstance() {
        if (!s_instance) {
            s_instance = new TopLeftInfoBarManager();
        }
        return s_instance;
    }

protected:
    QPoint _pos(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint _slideStartPos(InfoBar* infoBar) override;

private:
    static TopLeftInfoBarManager* s_instance;

};

class QFLUENT_EXPORT BottomLeftInfoBarManager : public InfoBarManager {
    Q_OBJECT
public:
    static BottomLeftInfoBarManager* getInstance() {
        if (!s_instance) {
            s_instance = new BottomLeftInfoBarManager();
        }
        return s_instance;
    }

protected:
    QPoint _pos(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint _slideStartPos(InfoBar* infoBar) override;

private:
    static BottomLeftInfoBarManager* s_instance;

};

class QFLUENT_EXPORT BottomInfoBarManager : public InfoBarManager {
    Q_OBJECT
public:
    static BottomInfoBarManager* getInstance() {
        if (!s_instance) {
            s_instance = new BottomInfoBarManager();
        }
        return s_instance;
    }

protected:
    QPoint _pos(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint _slideStartPos(InfoBar* infoBar) override;

private:
    static BottomInfoBarManager* s_instance;

};


#endif // INFO_BAR_H
