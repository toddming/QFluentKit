#pragma once

#include <QFrame>
#include <QMap>
#include <QPointer>
#include <functional>

#include "FluentGlobal.h"

// 前置声明，减少头文件依赖
class QLabel;
class QBoxLayout;
class QHBoxLayout;
class QGraphicsOpacityEffect;
class QPropertyAnimation;
class QPaintEvent;
class QShowEvent;
class QHideEvent;
class QCloseEvent;
class TransparentToolButton;

/**
 * @brief 信息栏图标组件
 * 显示不同类型消息对应的图标
 */
class QFLUENT_EXPORT InfoIconWidget : public QWidget {
    Q_OBJECT

public:
    explicit InfoIconWidget(Fluent::MessageType type, QWidget* parent = nullptr);
    ~InfoIconWidget() override = default;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Fluent::MessageType m_type;
};

/**
 * @brief 信息栏主类
 * 用于显示临时通知消息，支持自动关闭、自定义样式和动画效果
 */
class QFLUENT_EXPORT InfoBar : public QFrame {
    Q_OBJECT

public:
    explicit InfoBar(Fluent::MessageType type,
                     const QString& title,
                     const QString& content,
                     Qt::Orientation orientation = Qt::Horizontal,
                     bool isClosable = true,
                     int duration = 1000,
                     Fluent::MessagePosition position = Fluent::MessagePosition::TOP_RIGHT,
                     QWidget* parent = nullptr);
    ~InfoBar() override = default;

    /**
     * @brief 添加自定义控件到信息栏
     * @param widget 要添加的控件
     * @param stretch 拉伸因子
     */
    void addWidget(QWidget* widget, int stretch = 0);

    /**
     * @brief 设置自定义背景颜色
     * @param light 浅色模式背景色
     * @param dark 深色模式背景色
     */
    void setCustomBackgroundColor(const QColor& light, const QColor& dark);

    // 静态工厂方法
    static InfoBar* newInfoBar(Fluent::MessageType type,
                               const QString& title,
                               const QString& content,
                               Qt::Orientation orientation = Qt::Horizontal,
                               bool isClosable = true,
                               int duration = 1000,
                               Fluent::MessagePosition position = Fluent::MessagePosition::TOP_RIGHT,
                               QWidget* parent = nullptr);

    static InfoBar* info(const QString& title,
                        const QString& content,
                        Qt::Orientation orientation = Qt::Horizontal,
                        bool isClosable = true,
                        int duration = 1000,
                        Fluent::MessagePosition position = Fluent::MessagePosition::TOP_RIGHT,
                        QWidget* parent = nullptr);

    static InfoBar* success(const QString& title,
                           const QString& content,
                           Qt::Orientation orientation = Qt::Horizontal,
                           bool isClosable = true,
                           int duration = 1000,
                           Fluent::MessagePosition position = Fluent::MessagePosition::TOP_RIGHT,
                           QWidget* parent = nullptr);

    static InfoBar* warning(const QString& title,
                           const QString& content,
                           Qt::Orientation orientation = Qt::Horizontal,
                           bool isClosable = true,
                           int duration = 1000,
                           Fluent::MessagePosition position = Fluent::MessagePosition::TOP_RIGHT,
                           QWidget* parent = nullptr);

    static InfoBar* error(const QString& title,
                         const QString& content,
                         Qt::Orientation orientation = Qt::Horizontal,
                         bool isClosable = true,
                         int duration = 1000,
                         Fluent::MessagePosition position = Fluent::MessagePosition::TOP_RIGHT,
                         QWidget* parent = nullptr);

signals:
    void closedSignal();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void initWidget();
    void initLayout();
    void setStyleSheet();
    void fadeOut();
    void adjustText();

private:
    // 配置参数
    QString m_title;
    QString m_content;
    Qt::Orientation m_orientation;
    Fluent::MessageType m_type;
    int m_duration;
    bool m_isClosable;
    Fluent::MessagePosition m_position;

    // UI组件（使用原始指针，因为Qt的父子关系会自动管理内存）
    QLabel* m_titleLabel;
    QLabel* m_contentLabel;
    TransparentToolButton* m_closeButton;
    InfoIconWidget* m_iconWidget;

    // 布局
    QHBoxLayout* m_horizontalLayout;
    QBoxLayout* m_textLayout;
    QBoxLayout* m_widgetLayout;

    // 动画效果
    QGraphicsOpacityEffect* m_opacityEffect;
    QPropertyAnimation* m_opacityAnimation;

    // 自定义颜色
    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;
};

/**
 * @brief 信息栏管理器基类
 * 管理信息栏的显示位置和动画效果
 */
class QFLUENT_EXPORT InfoBarManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 添加信息栏
     */
    void add(InfoBar* infoBar);

    /**
     * @brief 移除信息栏
     */
    void remove(InfoBar* infoBar);

    /**
     * @brief 注册管理器
     */
    static void registerManager(Fluent::MessagePosition position,
                                std::function<InfoBarManager*()> creator);

    /**
     * @brief 创建指定位置的管理器
     */
    static InfoBarManager* make(Fluent::MessagePosition position);

    /**
     * @brief 将消息类型转换为字符串
     */
    static QString toString(Fluent::MessageType type);

protected:
    explicit InfoBarManager(QObject* parent = nullptr);
    ~InfoBarManager() override = default;

    /**
     * @brief 计算信息栏位置
     */
    virtual QPoint calculatePosition(InfoBar* infoBar, const QSize& parentSize = QSize()) = 0;

    /**
     * @brief 计算滑动动画起始位置
     */
    virtual QPoint calculateSlideStartPosition(InfoBar* infoBar) = 0;

    QPropertyAnimation* createSlideAnimation(InfoBar* infoBar);
    void updateDropAnimations(QWidget* parent);

    bool eventFilter(QObject* obj, QEvent* event) override;

protected:
    int m_spacing = 16;
    int m_margin = 24;
    QMap<QWidget*, QList<QPointer<InfoBar>>> m_infoBars;
    QList<QPropertyAnimation*> m_slideAnimations;
    QList<QPropertyAnimation*> m_dropAnimations;

    static QMap<Fluent::MessagePosition, std::function<InfoBarManager*()>> s_managers;
};

/**
 * @brief 顶部居中信息栏管理器
 */
class QFLUENT_EXPORT TopInfoBarManager : public InfoBarManager {
    Q_OBJECT

public:
    static TopInfoBarManager* getInstance();

protected:
    QPoint calculatePosition(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint calculateSlideStartPosition(InfoBar* infoBar) override;

private:
    TopInfoBarManager() = default;
    static TopInfoBarManager* s_instance;
};

/**
 * @brief 右上角信息栏管理器
 */
class QFLUENT_EXPORT TopRightInfoBarManager : public InfoBarManager {
    Q_OBJECT

public:
    static TopRightInfoBarManager* getInstance();

protected:
    QPoint calculatePosition(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint calculateSlideStartPosition(InfoBar* infoBar) override;

private:
    TopRightInfoBarManager() = default;
    static TopRightInfoBarManager* s_instance;
};

/**
 * @brief 右下角信息栏管理器
 */
class QFLUENT_EXPORT BottomRightInfoBarManager : public InfoBarManager {
    Q_OBJECT

public:
    static BottomRightInfoBarManager* getInstance();

protected:
    QPoint calculatePosition(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint calculateSlideStartPosition(InfoBar* infoBar) override;

private:
    BottomRightInfoBarManager() = default;
    static BottomRightInfoBarManager* s_instance;
};

/**
 * @brief 左上角信息栏管理器
 */
class QFLUENT_EXPORT TopLeftInfoBarManager : public InfoBarManager {
    Q_OBJECT

public:
    static TopLeftInfoBarManager* getInstance();

protected:
    QPoint calculatePosition(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint calculateSlideStartPosition(InfoBar* infoBar) override;

private:
    TopLeftInfoBarManager() = default;
    static TopLeftInfoBarManager* s_instance;
};

/**
 * @brief 左下角信息栏管理器
 */
class QFLUENT_EXPORT BottomLeftInfoBarManager : public InfoBarManager {
    Q_OBJECT

public:
    static BottomLeftInfoBarManager* getInstance();

protected:
    QPoint calculatePosition(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint calculateSlideStartPosition(InfoBar* infoBar) override;

private:
    BottomLeftInfoBarManager() = default;
    static BottomLeftInfoBarManager* s_instance;
};

/**
 * @brief 底部居中信息栏管理器
 */
class QFLUENT_EXPORT BottomInfoBarManager : public InfoBarManager {
    Q_OBJECT

public:
    static BottomInfoBarManager* getInstance();

protected:
    QPoint calculatePosition(InfoBar* infoBar, const QSize& parentSize = QSize()) override;
    QPoint calculateSlideStartPosition(InfoBar* infoBar) override;

private:
    BottomInfoBarManager() = default;
    static BottomInfoBarManager* s_instance;
};
