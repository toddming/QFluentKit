#pragma once

#include <QObject>
#include <QList>
#include <QMap>

#include "FluentGlobal.h"

class StackedWidget;
class RouteItem
{
public:
    RouteItem() = default;
    RouteItem(StackedWidget* stacked, const QString& routeKey)
        : stacked(stacked), routeKey(routeKey) {}

    bool operator==(const RouteItem& other) const
    {
        return stacked == other.stacked && routeKey == other.routeKey;
    }

    StackedWidget* stacked{nullptr};   // 改回原始指针
    QString routeKey;
};

class StackedHistory : public QObject
{
    Q_OBJECT
public:
    explicit StackedHistory(StackedWidget* stacked, QObject* parent = nullptr);

    void setDefaultRouteKey(const QString& routeKey);
    bool push(const QString& routeKey);
    void pop();
    void remove(const QString& routeKey);
    QString top() const;
    bool isEmpty() const;
    void goToTop();

private:
    StackedWidget* m_stacked{nullptr};
    QString m_defaultRouteKey;
    QStringList m_history;
};

class QFLUENT_EXPORT Router : public QObject
{
    Q_OBJECT

public:
    static Router* instance();
    static void destroyInstance();

    Router(const Router&) = delete;
    Router& operator=(const Router&) = delete;

    void setDefaultRouteKey(StackedWidget* stacked, const QString& routeKey);
    void push(StackedWidget* stacked, const QString& routeKey);
    void pop();
    void remove(const QString& routeKey);

signals:
    void emptyChanged(bool empty);

private:
    explicit Router(QObject* parent = nullptr);
    ~Router() override = default;

    void cleanupStackedHistory(StackedWidget* stacked);

private:
    static Router* s_instance;

    QList<RouteItem> m_globalHistory;
    QMap<StackedWidget*, StackedHistory*> m_stackHistories;
};

#define qrouter Router::instance()
