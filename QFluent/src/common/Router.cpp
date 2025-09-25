#include "Router.h"
#include <QGlobalStatic>

Q_GLOBAL_STATIC(Router, qrouter)

RouteItem::RouteItem(QStackedWidget* stacked, const QString& routeKey)
    : stacked(stacked), routeKey(routeKey)
{
}

bool RouteItem::operator==(const RouteItem& other) const
{
    if (other.stacked == nullptr) {
        return false;
    }
    return other.stacked == stacked && routeKey == other.routeKey;
}

StackedHistory::StackedHistory(QStackedWidget* stacked, QObject* parent)
    : QObject(parent), m_stacked(stacked)
{
}

int StackedHistory::length() const
{
    return m_history.size();
}

bool StackedHistory::isEmpty() const
{
    return m_history.size() <= 1;
}

bool StackedHistory::push(const QString& routeKey)
{
    if (m_history.isEmpty() || m_history.last() == routeKey) {
        return false;
    }

    m_history.append(routeKey);
    return true;
}

void StackedHistory::pop()
{
    if (isEmpty()) {
        return;
    }

    m_history.removeLast();
    goToTop();
}

void StackedHistory::remove(const QString& routeKey)
{
    if (!m_history.contains(routeKey)) {
        return;
    }

    // 保留第一个元素（默认路由键）
    QStringList newHistory;
    newHistory.append(m_history.first());

    // 从第二个元素开始过滤
    for (int i = 1; i < m_history.size(); i++) {
        if (m_history[i] != routeKey) {
            newHistory.append(m_history[i]);
        }
    }

    // 移除连续的重复项
    QStringList compressedHistory;
    compressedHistory.append(newHistory.first());
    for (int i = 1; i < newHistory.size(); i++) {
        if (newHistory[i] != compressedHistory.last()) {
            compressedHistory.append(newHistory[i]);
        }
    }

    m_history = compressedHistory;
    goToTop();
}

QString StackedHistory::top() const
{
    return m_history.isEmpty() ? QString() : m_history.last();
}

void StackedHistory::setDefaultRouteKey(const QString& routeKey)
{
    m_defaultRouteKey = routeKey;
    if (m_history.isEmpty()) {
        m_history.append(routeKey);
    } else {
        m_history[0] = routeKey;
    }
}

void StackedHistory::goToTop()
{
    QString key = top();
    if (key.isEmpty()) {
        return;
    }

    // 在堆叠窗口的子控件中查找具有指定objectName的控件
    QWidget* widget = m_stacked->findChild<QWidget*>(key);
    if (widget) {
        m_stacked->setCurrentWidget(widget);
    } else {
        qWarning() << "StackedHistory::goToTop: widget not found for routeKey" << key;
    }
}

Router::Router(QObject* parent)
    : QObject(parent)
{
}

Router *Router::instance()
{
    return qrouter();
}

void Router::setDefaultRouteKey(QStackedWidget* stacked, const QString& routeKey)
{
    if (!m_stackHistories.contains(stacked)) {
        m_stackHistories.insert(stacked, new StackedHistory(stacked, this));
    }
    m_stackHistories[stacked]->setDefaultRouteKey(routeKey);
}

void Router::push(QStackedWidget* stacked, const QString& routeKey)
{
    RouteItem item(stacked, routeKey);

    if (!m_stackHistories.contains(stacked)) {
        m_stackHistories.insert(stacked, new StackedHistory(stacked, this));
    }

    // 尝试推入历史记录
    bool success = m_stackHistories[stacked]->push(routeKey);
    if (success) {
        m_history.append(item);
        emit emptyChanged(m_history.isEmpty());
    }
}

void Router::pop()
{
    if (m_history.isEmpty()) {
        return;
    }

    RouteItem item = m_history.takeLast();
    emit emptyChanged(m_history.isEmpty());

    if (m_stackHistories.contains(item.stacked)) {
        m_stackHistories[item.stacked]->pop();
    } else {
        qWarning() << "Router::pop: stacked widget not found in history";
    }
}

void Router::remove(const QString& routeKey)
{
    // 从全局历史中删除所有具有该routeKey的项
    for (int i = m_history.size() - 1; i >= 0; i--) {
        if (m_history[i].routeKey == routeKey) {
            m_history.removeAt(i);
        }
    }

    // 移除连续的重复项
    QList<RouteItem> newHistory;
    for (const RouteItem& item : m_history) {
        if (newHistory.isEmpty() || !(newHistory.last() == item)) {
            newHistory.append(item);
        }
    }

    m_history = newHistory;
    emit emptyChanged(m_history.isEmpty());

    // 同时从每个StackedHistory中删除
    for (StackedHistory* history : m_stackHistories) {
        // 检查这个StackedHistory是否有这个routeKey（通过查找子控件）
        QWidget* widget = history->stackedWidget()->findChild<QWidget*>(routeKey);
        if (widget) {
            history->remove(routeKey);
        }
    }
}

bool Router::isEmpty() const
{
    return m_history.isEmpty();
}
