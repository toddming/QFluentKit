#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QStackedWidget>
#include <QHash>
#include <QList>
#include <QSet>
#include <QDebug>

#define sRouter Router::instance()

class RouteItem
{
public:
    RouteItem(QStackedWidget* stacked, const QString& routeKey);

    QStackedWidget* stacked;
    QString routeKey;

    bool operator==(const RouteItem& other) const;
};

class StackedHistory : public QObject
{
    Q_OBJECT
public:
    explicit StackedHistory(QStackedWidget* stacked, QObject* parent = nullptr);

    int length() const;
    bool isEmpty() const;

    bool push(const QString& routeKey);
    void pop();
    void remove(const QString& routeKey);
    QString top() const;

    void setDefaultRouteKey(const QString& routeKey);
    void goToTop();

    QStackedWidget* stackedWidget() const { return m_stacked; }

private:
    QStackedWidget* m_stacked;
    QString m_defaultRouteKey;
    QStringList m_history;
};

class Router : public QObject
{
    Q_OBJECT
public:
    explicit Router(QObject* parent = nullptr);

    static Router *instance();
    void setDefaultRouteKey(QStackedWidget* stacked, const QString& routeKey);
    void push(QStackedWidget* stacked, const QString& routeKey);
    void pop();
    void remove(const QString& routeKey);

    bool isEmpty() const;

signals:
    void emptyChanged(bool isEmpty);

private:
    Q_DISABLE_COPY_MOVE(Router)

    QList<RouteItem> m_history;
    QHash<QStackedWidget*, StackedHistory*> m_stackHistories;
};


#endif // ROUTER_H
