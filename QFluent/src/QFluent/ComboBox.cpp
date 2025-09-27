#include "ComboBox.h"
#include <QApplication>
#include <QScreen>
#include <QActionGroup>
#include <QDebug>

#include "menu/MenuActionListWidget.h"
#include "menu/MenuItemDelegate.h"

///////////////////ComboBoxMenu//////////////////////
ComboBoxMenu::ComboBoxMenu(const QString& title, QWidget *parent) : RoundMenu(title, parent)
{
    view()->setItemDelegate(new IndicatorMenuItemDelegate(this));
    view()->setObjectName("comboListWidget");
}

void ComboBoxMenu::exec(const QPoint& pos, bool animate, MenuAnimationType::MenuAnimation aniType)
{
    view()->adjustSize(pos, aniType);
    adjustMenuSize();
    RoundMenu::exec(pos, animate, aniType);
}



///////////////////ComboBox//////////////////////
ComboBox::ComboBox(QWidget *parent)
    : QPushButton(parent)
{
    installEventFilter(this);

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::COMBO_BOX);

    arrowAni = new TranslateYAnimation(this);
}

void ComboBox::addItem(const QString &text, const QIcon &icon, const QVariant &userData)
{
    insertItem(count(), text, icon, userData);
}

void ComboBox::addItems(const QStringList &texts)
{
    for (const QString &text : texts) {
        addItem(text);
    }
}

void ComboBox::insertItem(int index, const QString &text, const QIcon &icon, const QVariant &userData)
{
    if (index < 0 || index > count()) index = count();

    m_items.insert(index, ComboItem(text, icon, userData));

    if (index <= m_currentIndex) {
        setCurrentIndex(m_currentIndex + 1);
    }
}

void ComboBox::insertItems(int index, const QStringList &texts)
{
    for (const QString &text : texts) {
        insertItem(index++, text);
    }
}

void ComboBox::removeItem(int index)
{
    if (index < 0 || index >= count()) return;

    m_items.removeAt(index);

    if (index < m_currentIndex) {
        setCurrentIndex(m_currentIndex - 1);
    } else if (index == m_currentIndex) {
        if (index > 0) {
            setCurrentIndex(m_currentIndex - 1);
        } else if (count() > 0) {
            setCurrentIndex(0);
        } else {
            setCurrentIndex(-1);
        }
    }
}

void ComboBox::clear()
{
    if (m_currentIndex >= 0) {
        setText("");
        m_currentIndex = -1;
    }
    m_items.clear();
}

int ComboBox::currentIndex() const
{
    return m_currentIndex;
}

QString ComboBox::currentText() const
{
    if (m_currentIndex >= 0 && m_currentIndex < count()) {
        return m_items[m_currentIndex].text;
    }
    return "";
}

QVariant ComboBox::currentData() const
{
    if (m_currentIndex >= 0 && m_currentIndex < count()) {
        return m_items[m_currentIndex].userData;
    }
    return QVariant(NULL);
}

void ComboBox::setCurrentIndex(int index)
{
    if (index < -1 || index >= count() || index == m_currentIndex)
        return;

    QString oldText = currentText();

    if (index == -1) {
        m_currentIndex = -1;
        setPlaceholderText(m_placeholderText);
    } else {
        m_currentIndex = index;
        setText(m_items[index].text);
        updateTextState(false);
    }

    if (oldText != currentText()) {
        emit currentTextChanged(currentText());
    }

    emit currentIndexChanged(index);
}

void ComboBox::setCurrentText(const QString &text)
{
    if (text == currentText()) return;

    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

int ComboBox::count() const
{
    return m_items.size();
}

QString ComboBox::itemText(int index) const
{
    if (index >= 0 && index < count()) {
        return m_items[index].text;
    }
    return "";
}

QIcon ComboBox::itemIcon(int index) const
{
    if (index >= 0 && index < count()) {
        return m_items[index].icon;
    }
    return QIcon();
}

QVariant ComboBox::itemData(int index) const
{
    if (index >= 0 && index < count()) {
        return m_items[index].userData;
    }
    return QVariant(NULL);
}

int ComboBox::findText(const QString &text) const
{
    for (int i = 0; i < count(); ++i) {
        if (m_items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int ComboBox::findData(const QVariant &data) const
{
    for (int i = 0; i < count(); ++i) {
        if (m_items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void ComboBox::setPlaceholderText(const QString &text)
{
    m_placeholderText = text;
    if (m_currentIndex == -1) {
        setText(text);
        updateTextState(true);
    }
}

QString ComboBox::placeholderText() const
{
    return m_placeholderText;
}

void ComboBox::setMaxVisibleItems(int max)
{
    m_maxVisibleItems = max;
}

int ComboBox::maxVisibleItems() const
{
    return m_maxVisibleItems;
}

void ComboBox::setText(const QString &text)
{
    QPushButton::setText(text);
    adjustSize();
}

void ComboBox::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    // 绘制下拉箭头
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_isHover) {
        painter.setOpacity(0.8);
    } else if (m_isPressed) {
        painter.setOpacity(0.7);
    }

    QRectF rect = QRectF(width()-22, height()/2-5+arrowAni->y(), 10, 10);

    Icon::drawSvgIcon(&painter, IconType::FLuentIcon::ARROW_DOWN, rect);
}

void ComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);
    toggleComboMenu();
}


bool ComboBox::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this) {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            m_isPressed = true;
            update();
            break;
        case QEvent::MouseButtonRelease:
            m_isPressed = false;
            update();
            break;
        case QEvent::Enter:
            m_isHover = true;
            update();
            break;
        case QEvent::Leave:
            m_isHover = false;
            update();
            break;
        default:
            break;
        }
    }
    return QPushButton::eventFilter(watched, event);
}

void ComboBox::toggleComboMenu()
{
    if (m_dropMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void ComboBox::showComboMenu()
{
    if (count() == 0) return;

    m_dropMenu = createComboMenu();

    if (m_dropMenu->view()->width() < this->width()) {
        m_dropMenu->view()->setMinimumWidth(this->width());
        m_dropMenu->adjustMenuSize();
    }

    if (currentIndex() >= 0 && currentIndex() < count()) {
        m_dropMenu->setDefaultAction(m_dropMenu->menuActions().at(currentIndex()));
    }

    int x = -m_dropMenu->width() / 2 + m_dropMenu->layout()->contentsMargins().left() + width() / 2;
    QPoint pd = mapToGlobal(QPoint(x, height()));
    int hd = m_dropMenu->view()->heightForAnimation(pd, MenuAnimationType::MenuAnimation::DROP_DOWN);

    QPoint pu = mapToGlobal(QPoint(x, 0));
    int hu = m_dropMenu->view()->heightForAnimation(pu, MenuAnimationType::MenuAnimation::PULL_UP);

    if (hd >= hu) {
        m_dropMenu->view()->adjustSize(pd, MenuAnimationType::MenuAnimation::DROP_DOWN);
        m_dropMenu->exec(pd, true, MenuAnimationType::MenuAnimation::DROP_DOWN);
    } else {
        m_dropMenu->view()->adjustSize(pu, MenuAnimationType::MenuAnimation::PULL_UP);
        m_dropMenu->exec(pu, true, MenuAnimationType::MenuAnimation::PULL_UP);
    }
}

void ComboBox::closeComboMenu()
{
    if (!m_dropMenu) return;
    m_dropMenu = nullptr;
}

void ComboBox::updateTextState(bool isPlaceholder)
{
    if (this->property("isPlaceholderText").toBool() == isPlaceholder) return;

    this->setProperty("isPlaceholderText", isPlaceholder);
    style()->unpolish(this);
    style()->polish(this);
}

ComboBoxMenu* ComboBox::createComboMenu()
{
    ComboBoxMenu *menu = new ComboBoxMenu("menu", this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    for (int i = 0; i < count(); ++i) {
        QAction *action = new QAction(m_items[i].icon, m_items[i].text, menu);
        action->setData(i);
        action->setCheckable(true);
        if (i == m_currentIndex) {
            action->setChecked(true);
        }
        menu->addAction(action);
        connect(action, &QAction::triggered, this, [=](){
            int index = action->property("index").toInt();
            if (index != m_currentIndex) {
                setCurrentIndex(index);
                emit activated(index);
                emit textActivated(action->text());
            }
        });
    }
    connect(menu, &ComboBoxMenu::closed, this, [=](){
        QPoint pos = mapFromGlobal(QCursor::pos());
        if (!rect().contains(pos)) {
            m_dropMenu = nullptr;
        }
    });
    return menu;
}

void ComboBox::handleMenuAction(QAction *action)
{
    int index = action->data().toInt();
    if (index < 0 || index >= count()) return;

    if (index != m_currentIndex) {
        setCurrentIndex(index);
    }

    emit activated(index);
    emit textActivated(currentText());
}
