#include "EditableComboBox.h"
#include <QApplication>
#include <QScreen>
#include <QActionGroup>
#include <QDebug>

#include "menu/RoundMenu.h"
#include "Theme.h"
#include "Icon.h"
#include "menu/MenuActionListWidget.h"
#include "menu/ComboBoxMenu.h"


EditableComboBox::EditableComboBox(QWidget *parent)
    : LineEdit(parent)
{
    installEventFilter(this);

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::COMBO_BOX);

    arrowAni = new TranslateYAnimation(this);

    m_dropButton = new LineEditButton(Icon::FluentIcon(IconType::FLuentIcon::ARROW_DOWN), this);
    m_dropButton->setFixedSize(30, 25);
    hBoxLayout()->addWidget(m_dropButton, 0, Qt::AlignRight);
    setTextMargins(0, 0, 29, 0);
    setClearButtonEnabled(false);

    connect(m_dropButton, &LineEditButton::clicked, this, [this]() { toggleComboMenu(); });
    connect(this, &LineEdit::textChanged, this, &EditableComboBox::onComboTextChanged);
    getClearButton()->disconnect();
    connect(getClearButton(), &LineEditButton::clicked, this, &EditableComboBox::onClearButtonClicked);
    connect(this, &EditableComboBox::returnPressed, this, &EditableComboBox::onReturnPressed);
}

void EditableComboBox::addItem(const QString &text, const QIcon &icon, const QVariant &userData)
{
    insertItem(count(), text, icon, userData);
}

void EditableComboBox::addItems(const QStringList &texts)
{
    for (const QString &text : texts) {
        addItem(text);
    }
}

void EditableComboBox::insertItem(int index, const QString &text, const QIcon &icon, const QVariant &userData)
{
    if (index < 0 || index > count()) index = count();

    m_items.insert(index, ComboItem(text, icon, userData));

    if (index <= m_currentIndex) {
        setCurrentIndex(m_currentIndex + 1);
    }
}

void EditableComboBox::insertItems(int index, const QStringList &texts)
{
    for (const QString &text : texts) {
        insertItem(index++, text);
    }
}

void EditableComboBox::removeItem(int index)
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

void EditableComboBox::clear()
{
    if (m_currentIndex >= 0) {
        setText("");
        m_currentIndex = -1;
    }
    m_items.clear();
}

int EditableComboBox::currentIndex() const
{
    return m_currentIndex;
}

QString EditableComboBox::currentText() const
{
    return text();
}

QVariant EditableComboBox::currentData() const
{
    if (m_currentIndex >= 0 && m_currentIndex < count()) {
        return m_items[m_currentIndex].userData;
    }
    return QVariant(NULL);
}

void EditableComboBox::setCurrentIndex(int index)
{
    if (index >= count() || index == m_currentIndex)
        return;

    if (index < 0) {
        m_currentIndex = -1;
        setText("");
        setPlaceholderText(m_placeholderText);
    } else {
        m_currentIndex = index;
        setText(m_items.at(index).text);
    }
}

void EditableComboBox::setCurrentText(const QString &text)
{
    if (text == currentText()) return;

    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

int EditableComboBox::count() const
{
    return m_items.size();
}

QString EditableComboBox::itemText(int index) const
{
    if (index >= 0 && index < count()) {
        return m_items[index].text;
    }
    return "";
}

QIcon EditableComboBox::itemIcon(int index) const
{
    if (index >= 0 && index < count()) {
        return m_items[index].icon;
    }
    return QIcon();
}

QVariant EditableComboBox::itemData(int index) const
{
    if (index >= 0 && index < count()) {
        return m_items[index].userData;
    }
    return QVariant(NULL);
}

int EditableComboBox::findText(const QString &text) const
{
    for (int i = 0; i < count(); ++i) {
        if (m_items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int EditableComboBox::findData(const QVariant &data) const
{
    for (int i = 0; i < count(); ++i) {
        if (m_items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void EditableComboBox::setPlaceholderText(const QString &text)
{
    m_placeholderText = text;
    if (m_currentIndex == -1) {
        setText(text);
        updateTextState(true);
    }
}

QString EditableComboBox::placeholderText() const
{
    return m_placeholderText;
}

void EditableComboBox::setMaxVisibleItems(int max)
{
    m_maxVisibleItems = max;
}

int EditableComboBox::maxVisibleItems() const
{
    return m_maxVisibleItems;
}

void EditableComboBox::setText(const QString &text)
{
    LineEdit::setText(text);
    adjustSize();
}

void EditableComboBox::setCompleterMenu(CompleterMenu *menu)
{
    LineEdit::setCompleterMenu(menu);
    connect(menu, &CompleterMenu::activated, this, &EditableComboBox::onActivated);
}

void EditableComboBox::paintEvent(QPaintEvent *event)
{
    LineEdit::paintEvent(event);
}

void EditableComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    LineEdit::mouseReleaseEvent(event);
}


bool EditableComboBox::eventFilter(QObject *watched, QEvent *event)
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
    return LineEdit::eventFilter(watched, event);
}

void EditableComboBox::toggleComboMenu()
{
    if (m_dropMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void EditableComboBox::showComboMenu()
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
    int hd = m_dropMenu->view()->heightForAnimation(pd, MenuAnimationType::DROP_DOWN);

    QPoint pu = mapToGlobal(QPoint(x, 0));
    int hu = m_dropMenu->view()->heightForAnimation(pu, MenuAnimationType::PULL_UP);

    if (hd >= hu) {
        m_dropMenu->view()->adjustSize(pd, MenuAnimationType::DROP_DOWN);
        m_dropMenu->exec(pd, true, MenuAnimationType::DROP_DOWN);
    } else {
        m_dropMenu->view()->adjustSize(pu, MenuAnimationType::PULL_UP);
        m_dropMenu->exec(pu, true, MenuAnimationType::PULL_UP);
    }
}

void EditableComboBox::closeComboMenu()
{
    if (!m_dropMenu) return;
    m_dropMenu = nullptr;
}

void EditableComboBox::updateTextState(bool isPlaceholder)
{
    if (this->property("isPlaceholderText").toBool() == isPlaceholder) return;

    this->setProperty("isPlaceholderText", isPlaceholder);
    style()->unpolish(this);
    style()->polish(this);
}

ComboBoxMenu* EditableComboBox::createComboMenu()
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

void EditableComboBox::handleMenuAction(QAction *action)
{
    int index = action->data().toInt();
    if (index < 0 || index >= count()) return;

    if (index != m_currentIndex) {
        setCurrentIndex(index);
    }

    emit activated(index);
    emit textActivated(currentText());
}

void EditableComboBox::onActivated(const QString &text)
{
    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

void EditableComboBox::onReturnPressed()
{
    if (text().isEmpty()) {
        return;
    }
    int index = findText(text());
    if (index > 0 && index != m_currentIndex) {
        m_currentIndex = index;
        emit currentIndexChanged(index);
    } else if (index == -1) {
        addItem(text());
        setCurrentIndex(count() - 1);
    }
}

void EditableComboBox::onComboTextChanged(const QString &text)
{
    m_currentIndex = -1;
    emit currentTextChanged(text);

    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].text == text) {
            m_currentIndex = i;
            emit currentIndexChanged(i);
            return;
        }
    }
}

void EditableComboBox::onDropMenuClosed()
{
    m_dropMenu = nullptr;
}

void EditableComboBox::onClearButtonClicked()
{
    LineEdit::clear();
    m_currentIndex = -1;
}
