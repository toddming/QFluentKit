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
    explicit Router(QObject* parent = nullptr);
    ~Router();

    bool isEmpty() const;

    void setDefaultRouteKey(StackedWidget* stackedWidget, const QString& routeKey);
    void push(StackedWidget* stackedWidget, const QString& routeKey);
    void pop();
    void remove(const QString& routeKey);

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
