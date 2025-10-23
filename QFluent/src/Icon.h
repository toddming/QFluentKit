#pragma once

#include <QAction>

#include "Define.h"

class QPainter;
class IconEngine;
class QFLUENT_EXPORT Icon
{
public:
    static QIcon FluentIcon(IconType::FLuentIcon icon);

    static QIcon SvgIcon(const QString& fillPath, const QString& baseName,
                         const QString& lightSuffix, const QString& darkSuffix);


    static void drawSvgIcon(QPainter *painter, IconType::FLuentIcon icon,
                            const QRectF &rect, const QMap<QString, QString> &attributes = {});

    static void drawSvgIcon(QPainter *painter, const QString& fillPath, const QString& baseName,
                            const QString& lightSuffix, const QString& darkSuffix,
                            const QRectF &rect, const QMap<QString, QString> &attributes = {});

    static QMap<IconType::FLuentIcon, QString> fluentIcons();
};



class QFLUENT_EXPORT Action : public QAction
{
    Q_OBJECT

public:
    explicit Action(QObject* parent = nullptr);
    explicit Action(const QString& text, QObject* parent = nullptr);
    Action(const QIcon& icon, const QString& text, QObject* parent = nullptr);
    Action(const IconType::FLuentIcon icon, const QString& text, QObject* parent = nullptr);
    ~Action() override;

    QIcon icon() const;

    void setIcon(const IconType::FLuentIcon icon);
};
