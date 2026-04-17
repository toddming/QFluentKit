#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QGlobalStatic>
#include <QPointer>

#include "FluentGlobal.h"

class StackedWidget;

class QFLUENT_EXPORT StackedHistory : public QObject
{
    Q_OBJECT

public:
    explicit StackedHistory(StackedWidget* stackedWidget, QObject* parent = nullptr);

    bool isEmpty() const;
    int depth() const;

    bool push(const QString& routeKey);
    void pop();
    void remove(const QString& routeKey);

    QString top() const;
    QString defaultRouteKey() const;
    void setDefaultRouteKey(const QString& routeKey);

    bool isValid() const;

private:
    void goToTop();
    void removeConsecutiveDuplicates();

    QPointer<StackedWidget> m_stackedWidget;
    QString m_defaultRouteKey;
    QVector<QString> m_history;
};

class QFLUENT_EXPORT Router : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool empty READ isEmpty NOTIFY emptyChanged)

public:
    /**
     * @brief 构造函数 — Router 可独立实例化，不限于全局单例
     *
     * 多窗口场景下，每个窗口可持有自己的 Router 实例：
     *   auto* router = new Router(this);  // this = 窗口
     *   router->setDefaultRouteKey(stacked, "home");
     *   router->push(stacked, "settings");
     *
     * 单窗口场景可继续使用全局默认实例：
     *   Router::instance()->push(stacked, "home");
     */
    explicit Router(QObject* parent = nullptr);
    ~Router();

    bool isEmpty() const;

    void setDefaultRouteKey(StackedWidget* stackedWidget, const QString& routeKey);
    void push(StackedWidget* stackedWidget, const QString& routeKey);
    void pop();
    void remove(const QString& routeKey);

    /**
     * @brief 全局默认实例 — 适用于单窗口应用
     * 多窗口应用建议为每个窗口创建独立 Router 实例
     */
    static Router* instance();

signals:
    void emptyChanged(bool isEmpty);

private:
    struct RouteItem {
        QPointer<StackedWidget> stackedWidget;
        QString key;

        RouteItem(StackedWidget* stacked = nullptr, const QString& routeKey = QString())
            : stackedWidget(stacked), key(routeKey) {}

        bool isNull() const { return stackedWidget.isNull() || key.isEmpty(); }
        bool operator==(const RouteItem& other) const {
            return stackedWidget == other.stackedWidget && key == other.key;
        }
    };

    void removeConsecutiveDuplicates();
    StackedHistory* ensureHistory(StackedWidget* stackedWidget);
    void cleanupInvalidEntries();

    QVector<RouteItem> m_history;
    QHash<StackedWidget*, StackedHistory*> m_stackedHistories;
};

#endif // ROUTER_H
