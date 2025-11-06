#pragma once

#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QEvent>

#include "Property.h"
// ============================================================
// EditLayer
// ============================================================
class EditLayer : public QWidget
{
    Q_OBJECT

public:
    explicit EditLayer(QWidget *parent);

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
};

// ============================================================
// TextEdit
// ============================================================
class QFLUENT_EXPORT TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit TextEdit(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    EditLayer *layer = nullptr;
};

// ============================================================
// PlainTextEdit
// ============================================================
class QFLUENT_EXPORT PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit PlainTextEdit(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    EditLayer *layer = nullptr;
};

// ============================================================
// TextBrowser
// ============================================================
class QFLUENT_EXPORT TextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    explicit TextBrowser(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    EditLayer *layer = nullptr;
};
