#include "TextEdit.h"
#include <QApplication>
#include <QContextMenuEvent>
#include <QStyle>

#include "Theme.h"
#include "StyleSheet.h"
#include "QFluent/scrollbar/ScrollBar.h"

// ============================================================
// EditLayer
// ============================================================
EditLayer::EditLayer(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    parent->installEventFilter(this);
}

bool EditLayer::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == parent() && e->type() == QEvent::Resize) {
        QResizeEvent *re = static_cast<QResizeEvent*>(e);
        resize(re->size());
    }
    return QWidget::eventFilter(obj, e);
}

void EditLayer::paintEvent(QPaintEvent *)
{
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    if (!parent->hasFocus())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    const QMargins m = contentsMargins();
    const int w = width() - m.left() - m.right();
    const int h = height();

    QPainterPath path;
    path.addRoundedRect(QRectF(m.left(), h - 10, w, 10), 5, 5);

    QPainterPath rectPath;
    rectPath.addRect(QRectF(m.left(), h - 10, w, 7.5));
    path = path.subtracted(rectPath);

    painter.fillPath(path, Theme::instance()->themeColor());
}

// ============================================================
// TextEdit
// ============================================================
TextEdit::TextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    layer = new EditLayer(this);
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::LINE_EDIT);

    auto scrollDelegate = new SmoothScrollDelegate(this);
    Q_UNUSED(scrollDelegate);
}

void TextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    // 假设 TextEditMenu 在 C++ 中已有对应实现
    // TextEditMenu menu(this);
    // menu.exec(e->globalPos());
    QTextEdit::contextMenuEvent(e);
}

// ============================================================
// PlainTextEdit
// ============================================================
PlainTextEdit::PlainTextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{
    layer = new EditLayer(this);
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::LINE_EDIT);

    auto scrollDelegate = new SmoothScrollDelegate(this);
    Q_UNUSED(scrollDelegate);
}

void PlainTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    // TextEditMenu menu(this);
    // menu.exec(e->globalPos());
    QPlainTextEdit::contextMenuEvent(e);
}

// ============================================================
// TextBrowser
// ============================================================
TextBrowser::TextBrowser(QWidget *parent)
    : QTextBrowser(parent)
{
    layer = new EditLayer(this);
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::LINE_EDIT);

    // ScrollBar* scrollBar = new ScrollBar(Qt::Vertical, this);
    // setVerticalScrollBar(scrollBar);
}

void TextBrowser::contextMenuEvent(QContextMenuEvent *e)
{
    // TextEditMenu menu(this);
    // menu.exec(e->globalPos());
    QTextBrowser::contextMenuEvent(e);
}
