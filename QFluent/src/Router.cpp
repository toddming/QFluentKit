#include "Router.h"

#include "QFluent/StackedWidget.h"


// ============================
// StackedHistory Implementation
// ============================
StackedHistory::StackedHistory(StackedWidget* stackedWidget, QObject* parent)
    : QObject(parent)
    , m_stackedWidget(stackedWidget)
    , m_defaultRouteKey(QString())
{
    Q_ASSERT(stackedWidget != nullptr);
    m_history.append(m_defaultRouteKey);
}

bool StackedHistory::isValid() const
{
    return !m_stackedWidget.isNull();
}

bool StackedHistory::isEmpty() const
{
    return depth() <= 1;
}

int StackedHistory::depth() const
{
    return m_history.size();
}

bool StackedHistory::push(const QString& routeKey)
{
    if (m_history.isEmpty() || m_history.last() == routeKey) {
        return false;
    }

    m_history.append(routeKey);
    goToTop();
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

    // 保留第一个元素（默认路由）
    QVector<QString> newHistory;
    newHistory.append(m_history.first());

    // 过滤掉要移除的路由
    for (int i = 1; i < m_history.size(); ++i) {
        if (m_history.at(i) != routeKey) {
            newHistory.append(m_history.at(i));
        }
    }

    m_history = newHistory;
    removeConsecutiveDuplicates();
    goToTop();
}

QString StackedHistory::top() const
{
    return m_history.isEmpty() ? QString() : m_history.last();
}

QString StackedHistory::defaultRouteKey() const
{
    return m_defaultRouteKey;
}

void StackedHistory::setDefaultRouteKey(const QString& routeKey)
{
    m_defaultRouteKey = routeKey;
    if (!m_history.isEmpty()) {
        m_history[0] = routeKey;
    }
    goToTop();
}

void StackedHistory::goToTop()
{
    if (top().isEmpty() || m_stackedWidget.isNull()) {
        return;
    }

    QWidget* widget = m_stackedWidget->findChild<QWidget*>(top());
    if (widget) {
        m_stackedWidget->setCurrentWidget(widget, false);
    }
}

void StackedHistory::removeConsecutiveDuplicates()
{
    if (m_history.size() <= 1) {
        return;
    }

    QVector<QString> deduplicated;
    deduplicated.append(m_history.first());

    for (int i = 1; i < m_history.size(); ++i) {
        if (m_history.at(i) != deduplicated.last()) {
            deduplicated.append(m_history.at(i));
        }
    }

    m_history = deduplicated;
}

// ============================
// Router Implementation
// ============================
Q_GLOBAL_STATIC(Router, s_router)

Router::Router(QObject* parent)
    : QObject(parent)
{
}

Router::~Router()
{
    // 清理所有 StackedHistory 对象
    qDeleteAll(m_stackedHistories);
    m_stackedHistories.clear();
}

bool Router::isEmpty() const
{
    return m_history.isEmpty();
}

void Router::setDefaultRouteKey(StackedWidget* stackedWidget, const QString& routeKey)
{
    if (!stackedWidget) {
        return;
    }

    StackedHistory* history = ensureHistory(stackedWidget);
    history->setDefaultRouteKey(routeKey);
}

void Router::push(StackedWidget* stackedWidget, const QString& routeKey)
{
    if (!stackedWidget) {
        return;
    }

    if (routeKey.isEmpty()) {
        return;
    }

    RouteItem item(stackedWidget, routeKey);

    StackedHistory* history = ensureHistory(stackedWidget);
    if (history->push(routeKey)) {
        m_history.append(item);
        emit emptyChanged(m_history.isEmpty());
    }
}

void Router::pop()
{
    if (m_history.isEmpty()) {
        return;
    }

    RouteItem lastItem = m_history.takeLast();
    StackedWidget* widget = lastItem.stackedWidget.data();
    if (widget) {
        StackedHistory* history = m_stackedHistories.value(widget);
        if (history) {
            history->pop();
        }
    }

    emit emptyChanged(m_history.isEmpty());
}

void Router::remove(const QString& routeKey)
{
    if (routeKey.isEmpty()) {
        return;
    }

    // 先清理无效条目
    cleanupInvalidEntries();

    // 从全局历史记录中移除
    QVector<RouteItem> newHistory;
    for (const RouteItem& item : m_history) {
        if (item.key != routeKey) {
            newHistory.append(item);
        }
    }
    m_history = newHistory;

    // 移除连续重复项
    removeConsecutiveDuplicates();

    emit emptyChanged(m_history.isEmpty());

    // 从所有有效的 StackedHistory 中移除该路由
    for (StackedHistory* history : m_stackedHistories) {
        if (history && history->isValid()) {
            history->remove(routeKey);
        }
    }
}

Router* Router::instance()
{
    return s_router();
}

void Router::removeConsecutiveDuplicates()
{
    if (m_history.size() <= 1) {
        return;
    }

    QVector<RouteItem> deduplicated;
    deduplicated.append(m_history.first());

    for (int i = 1; i < m_history.size(); ++i) {
        if (m_history.at(i).key != deduplicated.last().key) {
            deduplicated.append(m_history.at(i));
        }
    }

    m_history = deduplicated;
}

void Router::cleanupInvalidEntries()
{
    // 清理全局历史记录中的无效条目
    QVector<RouteItem> validHistory;
    for (const RouteItem& item : m_history) {
        if (!item.stackedWidget.isNull()) {
            validHistory.append(item);
        }
    }
    m_history = validHistory;

    // 清理无效的 StackedHistory
    for (auto it = m_stackedHistories.begin(); it != m_stackedHistories.end(); ) {
        StackedHistory* history = it.value();
        if (!history || !history->isValid()) {
            delete history;
            it = m_stackedHistories.erase(it);
        } else {
            ++it;
        }
    }
}

StackedHistory* Router::ensureHistory(StackedWidget* stackedWidget)
{
    StackedHistory* history = m_stackedHistories.value(stackedWidget);
    if (!history) {
        history = new StackedHistory(stackedWidget, this);
        m_stackedHistories.insert(stackedWidget, history);
    }
    return history;
}
