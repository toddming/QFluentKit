#pragma once

#include <QString>
#include <QIcon>
#include <QVariant>

namespace QFluent {

struct ComboItem
{
    QString text;
    QIcon icon;
    QVariant userData;
    bool isSeparator = false;

    ComboItem(const QString &text = QString(),
              const QIcon &icon = QIcon(),
              const QVariant &userData = QVariant())
        : text(text), icon(icon), userData(userData) {}
};

} // namespace QFluent
