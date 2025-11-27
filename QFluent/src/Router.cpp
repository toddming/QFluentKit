#include "Router.h"
#include <QWidget>

#include "QFluent/StackedWidget.h"

Router* Router::s_instance = nullptr;

Router* Router::instance()
{
    if (!s_instance) {
        s_instance = new Router;
    }
    return s_instance;
}

void Router::destroyInstance()
{
    delete s_instance;
    s_instance = nullptr;
}

Router::Router(QObject* parent) : QObject(parent)
{
    emit emptyChanged(true);
}

/* ====================== StackedHistory ====================== */
StackedHistory::StackedHistory(StackedWidget* stacked, QObject* parent)
    : QObject(parent), m_stacked(stacked)
{
    m_history << QString();
}

void StackedHistory::setDefaultRouteKey(const QString& routeKey)
{
    m_defaultRouteKey = routeKey;
    m_history[0] = routeKey;
}

bool StackedHistory::push(const QString& routeKey)
{
    if (m_history.constLast() == routeKey)
        return false;
    m_history.append(routeKey);
    return true;
}

void StackedHistory::pop()
{
    if (m_history.size() <= 1) return;
    m_history.removeLast();
    goToTop();
}

void StackedHistory::remove(const QString& routeKey)
{
    if (!m_history.contains(routeKey)) return;
    m_history.removeAll(routeKey);

    QStringList newList;
    newList << m_history.constFirst();
    for (int i = 1; i < m_history.size(); ++i) {
        if (m_history.at(i) != newList.constLast())
            newList << m_history.at(i);
    }
    m_history = std::move(newList);
    goToTop();
}

QString StackedHistory::top() const { return m_history.constLast(); }
bool StackedHistory::isEmpty() const { return m_history.size() <= 1; }

void StackedHistory::goToTop()
{
    if (!m_stacked) return;  // 判空

    const QString key = top();
    if (QWidget* w = m_stacked->findChild<QWidget*>(key)) {
        m_stacked->setCurrentWidget(w);
    } else if (!key.isEmpty() && key != m_defaultRouteKey) {
        if (QWidget* w = m_stacked->findChild<QWidget*>(m_defaultRouteKey))
            m_stacked->setCurrentWidget(w);
    }
}

/* ====================== Router ====================== */
void Router::setDefaultRouteKey(StackedWidget* stacked, const QString& routeKey)
{
    if (!stacked) return;

    if (!m_stackHistories.contains(stacked)) {
        auto* h = new StackedHistory(stacked, this);
        m_stackHistories[stacked] = h;
        connect(stacked, &QObject::destroyed, this, [this, stacked]() {
            cleanupStackedHistory(stacked);
        });
    }
    m_stackHistories[stacked]->setDefaultRouteKey(routeKey);
}

void Router::push(StackedWidget* stacked, const QString& routeKey)
{
    if (!stacked || routeKey.isEmpty()) return;

    if (!m_stackHistories.contains(stacked)) {
        auto* h = new StackedHistory(stacked, this);
        m_stackHistories[stacked] = h;
        connect(stacked, &QObject::destroyed, this, [this, stacked]() {
            cleanupStackedHistory(stacked);
        });
    }

    auto* sh = m_stackHistories[stacked];
    if (sh->push(routeKey)) {
        m_globalHistory.append(RouteItem(stacked, routeKey));
        emit emptyChanged(false);
    }
    sh->goToTop();
}

void Router::pop()
{
    if (m_globalHistory.isEmpty()) return;

    RouteItem item = m_globalHistory.takeLast();
    emit emptyChanged(m_globalHistory.isEmpty());

    // 防止 stacked 已经被销毁
    if (item.stacked && m_stackHistories.contains(item.stacked)) {
        m_stackHistories[item.stacked]->pop();
    }
}

void Router::remove(const QString& routeKey)
{
    if (routeKey.isEmpty()) return;

    // 全局历史：保留最早出现的那条
    QList<RouteItem> newGlobal;
    bool first = true;
    for (const auto& item : qAsConst(m_globalHistory)) {
        if (item.routeKey != routeKey || first) {
            newGlobal.append(item);
            if (item.routeKey == routeKey) first = false;
        }
    }
    m_globalHistory = std::move(newGlobal);
    emit emptyChanged(m_globalHistory.isEmpty());

    // 找到并清理
    for (auto it = m_stackHistories.constBegin(); it != m_stackHistories.constEnd(); ++it) {
        StackedWidget* stacked = it.key();
        if (stacked && stacked->findChild<QWidget*>(routeKey)) {
            it.value()->remove(routeKey);
            break;
        }
    }
}

void Router::cleanupStackedHistory(StackedWidget* stacked)
{
    if (StackedHistory* sh = m_stackHistories.take(stacked)) {
        sh->deleteLater();
    }

    // 手动删除全局历史中对应的条目
    for (int i = 0; i < m_globalHistory.size(); ) {
        if (m_globalHistory.at(i).stacked == stacked) {
            m_globalHistory.removeAt(i);
        } else {
            ++i;
        }
    }
}

#include "Router.moc"
