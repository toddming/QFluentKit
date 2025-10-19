#include "LineEdit.h"
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QCompleter>
#include <QAbstractItemModel>
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>
#include <QHBoxLayout>
#include <QEvent>
#include <QCompleter>
#include <QPainterPath>
#include <QTimer>


#include "Theme.h"
#include "Icon.h"
#include "menu/MenuItemDelegate.h"
#include "menu/MenuActionListWidget.h"

LineEditButton::LineEditButton(const QIcon& icon, QWidget* parent)
    : QToolButton(parent), m_icon(icon) {
    setObjectName("lineEditButton");
    setFixedSize(31, 23);
    setCursor(Qt::PointingHandCursor);
    setIconSize(QSize(10, 10));

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::LINE_EDIT);
}

void LineEditButton::setAction(QAction* action) {
    if (m_action) {
        disconnect(m_action, nullptr, this, nullptr);
        disconnect(this, nullptr, m_action, nullptr);
    }

    m_action = action;
    if (action) {
        connect(action, &QAction::changed, this, &LineEditButton::updateButtonState);
        connect(this, &LineEditButton::clicked, action, &QAction::trigger);
        updateButtonState();
    }
}

QAction* LineEditButton::action() const {
    return m_action;
}

void LineEditButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QToolButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    const QRect rect = this->rect();
    const QSize iconSize = this->iconSize();
    const QRect iconRect(rect.center() - QPoint(iconSize.width()/2, iconSize.height()/2), iconSize);

    if (isDown()) {
        painter.setOpacity(0.7);
    }

    m_icon.paint(&painter, iconRect, Qt::AlignCenter);
}

void LineEditButton::updateButtonState() {
    if (!m_action) return;
    
    setEnabled(m_action->isEnabled());
    setCheckable(m_action->isCheckable());
    setChecked(m_action->isChecked());
    setToolTip(m_action->toolTip());
    setIcon(m_action->icon());
    update();
}

LineEdit::LineEdit(QWidget* parent) 
    : QLineEdit(parent),
      m_layout(new QHBoxLayout(this)),
      m_clearButton(new LineEditButton(Icon::FluentIcon(IconType::FLuentIcon::CLOSE), this)),
      m_completerTimer(new QTimer(this)) {
    
    setFixedHeight(33);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    setClearButtonEnabled(true);
    setProperty("transparent", true);

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::LINE_EDIT);
    
    m_layout->setSpacing(3);
    m_layout->setContentsMargins(4, 4, 4, 4);
    m_layout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    initClearButton();
    
    connect(this, &LineEdit::textChanged, this, &LineEdit::handleTextChanged);
    connect(this, &LineEdit::textEdited, this, &LineEdit::handleTextEdited);
    m_completerTimer->setSingleShot(true);
    connect(m_completerTimer, &QTimer::timeout, this, &LineEdit::showCompleterMenu);
}

void LineEdit::initClearButton() {
    m_clearButton->setFixedSize(29, 25);
    m_clearButton->hide();
    m_layout->addWidget(m_clearButton, 0, Qt::AlignRight);
    connect(m_clearButton, &LineEditButton::clicked, this, &LineEdit::clear);
}

void LineEdit::setClearButtonEnabled(bool enable) {
    m_clearButtonEnabled = enable;
    m_clearButton->setVisible(enable && !text().isEmpty() && hasFocus());
    adjustTextMargins();
}

bool LineEdit::isClearButtonEnabled() const {
    return m_clearButtonEnabled;
}

void LineEdit::setCompleter(QCompleter* completer) {
    if (m_completer) {
        m_completer->deleteLater();
    }
    m_completer = completer;
}

QCompleter* LineEdit::completer() const {
    return m_completer;
}

void LineEdit::addAction(QAction* action, QLineEdit::ActionPosition position) {
    auto button = new LineEditButton(action->icon(), this);
    button->setAction(action);
    button->setFixedWidth(29);

    if (position == QLineEdit::LeadingPosition) {
        m_layout->insertWidget(m_leftButtons.size(), button);
        m_leftButtons.append(button);
    } else {
        m_layout->addWidget(button);
        m_rightButtons.append(button);
    }

    adjustTextMargins();
}

void LineEdit::addActions(QList<QAction*> actions, QLineEdit::ActionPosition position) {
    for (auto action : actions) {
        addAction(action, position);
    }
}

void LineEdit::adjustTextMargins() {
    const int leftMargin = m_leftButtons.size() * 30;
    const int rightMargin = m_rightButtons.size() * 30 + (m_clearButtonEnabled ? 28 : 0);
    setTextMargins(leftMargin, 0, rightMargin, 0);
}

void LineEdit::focusOutEvent(QFocusEvent* e) {
    QLineEdit::focusOutEvent(e);
    m_clearButton->hide();
}

void LineEdit::focusInEvent(QFocusEvent* e) {
    QLineEdit::focusInEvent(e);
    if (m_clearButtonEnabled) {
        m_clearButton->setVisible(!text().isEmpty());
    }
}

void LineEdit::handleTextChanged(const QString& text) {
    Q_UNUSED(text);
    if (m_clearButtonEnabled) {
        m_clearButton->setVisible(!text.isEmpty() && hasFocus());
    }
}

void LineEdit::handleTextEdited(const QString& text) {
    Q_UNUSED(text);
    if (m_completer) {
        if (!this->text().isEmpty()) {
            m_completerTimer->start(50);
        } else if (!m_completerMenu.isNull()) {
            m_completerMenu->close();
        }
    }
}

void LineEdit::setCompleterMenu(CompleterMenu *menu)
{
    if (m_completerMenu == menu) {
        return;
    }

    if (!m_completerMenu.isNull()) {
        disconnect(m_completerMenu, nullptr, m_completer, nullptr);
    }

    if (!menu) {
        return;
    }
    m_completerMenu = menu;

    connect(menu, &CompleterMenu::activated,
            m_completer, static_cast<void(QCompleter::*)(const QString&)>(&QCompleter::activated));

    connect(menu, &CompleterMenu::indexActivated,
            [this](const QModelIndex& idx) {
                QMetaObject::invokeMethod(m_completer, "activated", Qt::DirectConnection,
                                          Q_ARG(QModelIndex, idx));
            });
}

void LineEdit::showCompleterMenu() {
    if (!m_completer || text().isEmpty()) return;

    if (m_completerMenu.isNull()) {
        setCompleterMenu(new CompleterMenu(this));
    }

    m_completer->setCompletionPrefix(this->text());
    bool changed = m_completerMenu->setCompletion(m_completer->completionModel(), m_completer->completionColumn());
    m_completerMenu->setMaxVisibleItems(m_completer->maxVisibleItems());

    if (changed) m_completerMenu->popup();
}

void LineEdit::contextMenuEvent(QContextMenuEvent* e) {
    return;
}

void LineEdit::paintEvent(QPaintEvent* e) {
    QLineEdit::paintEvent(e);
    
    if (!hasFocus()) return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    const QMargins margins = contentsMargins();
    const int w = width() - margins.left() - margins.right();
    const int h = height();

    QPainterPath path;
    path.addRoundedRect(QRectF(margins.left(), h - 10, w, 10), 5, 5);

    QPainterPath rectPath;
    rectPath.addRect(margins.left(), h - 10, w, 8);
    path = path.subtracted(rectPath);

    painter.fillPath(path, Theme::instance()->themeColor());
}

void LineEdit::resizeEvent(QResizeEvent* e) {
    QLineEdit::resizeEvent(e);
    adjustTextMargins();
}

LineEditButton* LineEdit::getClearButton()
{
    return m_clearButton;
}





SearchLineEdit::SearchLineEdit(QWidget *parent)
    : LineEdit(parent)
    , searchButton(nullptr)
    , clearButton(nullptr)
    , hBoxLayout(nullptr)
{
    initWidgets();
}

void SearchLineEdit::initWidgets()
{
    setClearButtonEnabled(true);
    setTextMargins(0, 0, 59, 0);

    clearButton = getClearButton();
    hBoxLayout = qobject_cast<QHBoxLayout*>(layout());

    searchButton = new LineEditButton(Icon::FluentIcon(IconType::FLuentIcon::SEARCH), this);
    hBoxLayout->addWidget(searchButton);

    connect(searchButton, &LineEditButton::clicked, this, &SearchLineEdit::search);
    connect(searchButton, &LineEditButton::clicked, this, &SearchLineEdit::onClearButtonClicked);

    // connect(this, &SearchLineEdit::textChanged, [this](const QString &text) {
    //     clearButton->setVisible(!text.isEmpty());
    //     updateTextMargins();
    // });

    // updateTextMargins();
}

void SearchLineEdit::updateTextMargins()
{
    // int rightMargin = searchButton->sizeHint().width() + clearButton->sizeHint().width();
    // setTextMargins(0, 0, rightMargin, 0);
}

void SearchLineEdit::search()
{
    QString text = this->text().trimmed();
    if (!text.isEmpty()) {
        emit searchSignal(text);
    } else {
        emit clearSignal();
    }
}

void SearchLineEdit::onClearButtonClicked()
{
    emit clearSignal();  // 发出清除信号
}












CompleterMenu::CompleterMenu(LineEdit* lineEdit, QWidget* parent)
    : RoundMenu("menu", parent)
    , m_lineEdit(lineEdit)
{
    setItemHeight(36);
    view()->setObjectName("completerListWidget");
    view()->setItemDelegate(new IndicatorMenuItemDelegate(this));
    view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    this->installEventFilter(this);

    connect(view(), &MenuActionListWidget::itemClicked, this, &CompleterMenu::_onItemClicked);

    setItemHeight(33);
}

bool CompleterMenu::setCompletion(QAbstractItemModel* model, int column)
{
    if (!model) return false;

    QStringList items;
    m_indexes.clear();

    const int rowCount = model->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex index = model->index(i, column);
        items.append(model->data(index).toString());
        m_indexes.append(index);
    }

    if (m_items == items && isVisible()) {
        return false;
    }

    setItems(items);
    return true;
}

void CompleterMenu::setItems(const QStringList& items)
{
    view()->clear();
    m_items = items;
    view()->addItems(items);

    for (int i = 0; i < view()->count(); ++i) {
        QListWidgetItem* item = view()->item(i);
        item->setSizeHint(QSize(1, 33));
    }
}

void CompleterMenu::_onItemClicked(QListWidgetItem* item)
{
    hideMenu();
    _onCompletionItemSelected(item->text(), view()->row(item));
}

bool CompleterMenu::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() != QEvent::KeyPress) {
        return RoundMenu::eventFilter(obj, event);
    }

    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

    m_lineEdit->event(event); // Direct call (may not capture all side effects)
    // view()->event(event);

    if (keyEvent->key() == Qt::Key_Escape) {
        close();
    }

    if ((keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) &&
        view()->currentRow() >= 0) {
        QListWidgetItem* currentItem = view()->currentItem();
        if (currentItem) {
            _onCompletionItemSelected(currentItem->text(), view()->currentRow());
            close();
        }
    }

    return RoundMenu::eventFilter(obj, event);
}

void CompleterMenu::_onCompletionItemSelected(const QString& text, int row)
{
    if (m_lineEdit) {
        m_lineEdit->setText(text);
    }
    emit activated(text);

    if (row >= 0 && row < m_indexes.size()) {
        emit indexActivated(m_indexes.at(row));
    }
}

void CompleterMenu::popup()
{
    if (m_items.isEmpty()) {
        close();
        return;
    }

    LineEdit* p = m_lineEdit;
    if (!p) return;

    // Adjust menu size
    if (view()->width() < p->width()) {
        view()->setMinimumWidth(p->width());
        adjustSize();
    }

    int x = 0;
    //int x = -width() / 2 + layout()->contentsMargins().left() + p->width() / 2;
    int y_drop = p->height() - layout()->contentsMargins().top() + 2;
    QPoint pos_drop = p->mapToGlobal(QPoint(x, y_drop));
    int height_drop = view()->heightForAnimation(pos_drop, MenuAnimationType::MenuAnimation::DROP_DOWN);

    QPoint pos_up(x, 7);
    pos_up = p->mapToGlobal(pos_up);
    int height_up = view()->heightForAnimation(pos_up, MenuAnimationType::MenuAnimation::PULL_UP);

    QPoint finalPos;
    MenuAnimationType::MenuAnimation aniType;

    if (height_drop >= height_up) {
        finalPos = pos_drop;
        aniType = MenuAnimationType::MenuAnimation::DROP_DOWN;
    } else {
        finalPos = pos_up;
        aniType = MenuAnimationType::MenuAnimation::PULL_UP;
    }

    view()->adjustSize(finalPos, aniType);
    view()->setProperty("dropDown", aniType == MenuAnimationType::MenuAnimation::DROP_DOWN);
    view()->setStyle(QApplication::style());
    view()->update();
    adjustSize();
    exec(finalPos, true, aniType);

    view()->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::NoFocus);
    p->setFocus(Qt::OtherFocusReason);
}

