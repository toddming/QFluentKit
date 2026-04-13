#include "Private/ComboBoxPrivate.h"

#include "ComboBox.h"
#include <QApplication>
#include <QScreen>
#include <QActionGroup>
#include <QPainter>

#include "Animation.h"
#include "FluentIcon.h"
#include "StyleSheet.h"

ComboBox::ComboBox(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new ComboBoxPrivate(this))
{
    Q_D(ComboBox);

    setCurrentIndex(-1);

    setMaxVisibleItems(-1);

    installEventFilter(this);

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::COMBO_BOX);

    d->m_arrowAni = new TranslateYAnimation(this);
}

ComboBox::~ComboBox() = default;

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
    Q_D(ComboBox);
    if (index < 0 || index > count()) {
        index = count();
    }

    d->m_items.insert(index, ComboBoxDetail::ComboItem(text, icon, userData));

    if (index <= d->m_currentIndex) {
        setCurrentIndex(d->m_currentIndex + 1);
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
    Q_D(ComboBox);

    if (index < 0 || index >= count()) {
        return;
    }

    d->m_items.removeAt(index);

    if (index < d->m_currentIndex) {
        setCurrentIndex(d->m_currentIndex - 1);
    } else if (index == d->m_currentIndex) {
        if (index > 0) {
            setCurrentIndex(d->m_currentIndex - 1);
        } else if (count() > 0) {
            setCurrentIndex(0);
        } else {
            setCurrentIndex(-1);
        }
    }
}

void ComboBox::clear()
{
    Q_D(ComboBox);

    if (d->m_currentIndex >= 0) {
        setText(QString());
        d->m_currentIndex = -1;
    }
    d->m_items.clear();
}

int ComboBox::currentIndex() const
{
    Q_D(const ComboBox);

    return d->m_currentIndex;
}

QString ComboBox::currentText() const
{
    Q_D(const ComboBox);

    if (d->m_currentIndex >= 0 && d->m_currentIndex < count()) {
        return d->m_items[d->m_currentIndex].text;
    }
    return QString();
}

QVariant ComboBox::currentData() const
{
    Q_D(const ComboBox);

    if (d->m_currentIndex >= 0 && d->m_currentIndex < count()) {
        return d->m_items[d->m_currentIndex].userData;
    }
    return QVariant();
}

void ComboBox::setCurrentIndex(int index)
{
    Q_D(ComboBox);
    if (index < -1 || index >= count() || index == d->m_currentIndex) {
        return;
    }

    QString oldText = currentText();

    if (index == -1) {
        d->m_currentIndex = -1;
        setPlaceholderText(d->m_placeholderText);
    } else {
        d->m_currentIndex = index;
        setText(d->m_items[index].text);
        d->updateTextState(false);
    }

    if (oldText != currentText()) {
        emit currentTextChanged(currentText());
    }

    emit currentIndexChanged(index);
}

void ComboBox::setCurrentText(const QString &text)
{
    if (text == currentText()) {
        return;
    }

    int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

int ComboBox::count() const
{
    Q_D(const ComboBox);
    return d->m_items.size();
}

QString ComboBox::itemText(int index) const
{
    Q_D(const ComboBox);
    if (index >= 0 && index < count()) {
        return d->m_items[index].text;
    }
    return QString();
}

QIcon ComboBox::itemIcon(int index) const
{
    Q_D(const ComboBox);
    if (index >= 0 && index < count()) {
        return d->m_items[index].icon;
    }
    return QIcon();
}

QVariant ComboBox::itemData(int index) const
{
    Q_D(const ComboBox);

    if (index >= 0 && index < count()) {
        return d->m_items[index].userData;
    }
    return QVariant();
}

int ComboBox::findText(const QString &text) const
{
    Q_D(const ComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->m_items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int ComboBox::findData(const QVariant &data) const
{
    Q_D(const ComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->m_items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void ComboBox::setPlaceholderText(const QString &text)
{
    Q_D(ComboBox);
    d->m_placeholderText = text;
    if (d->m_currentIndex == -1) {
        setText(text);
        d->updateTextState(true);
    }
}

QString ComboBox::placeholderText() const
{
    Q_D(const ComboBox);
    return d->m_placeholderText;
}

void ComboBox::setText(const QString &text)
{
    QPushButton::setText(text);
    adjustSize();
}

void ComboBox::paintEvent(QPaintEvent *event)
{
    Q_D(ComboBox);
    QPushButton::paintEvent(event);

    // 绘制下拉箭头
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (d->m_isHover) {
        painter.setOpacity(0.8);
    } else if (d->m_isPressed) {
        painter.setOpacity(0.7);
    }

    QRectF rect = QRectF(width() - 22, height() / 2 - 5 + d->m_arrowAni->y(), 10, 10);
    FluentIcon(Fluent::IconType::ARROW_DOWN).render(&painter, rect);
}

void ComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(ComboBox);
    QPushButton::mouseReleaseEvent(event);
    d->toggleComboMenu();
}

bool ComboBox::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(ComboBox);
    if (watched == this) {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            d->m_isPressed = true;
            update();
            break;
        case QEvent::MouseButtonRelease:
            d->m_isPressed = false;
            update();
            break;
        case QEvent::Enter:
            d->m_isHover = true;
            update();
            break;
        case QEvent::Leave:
            d->m_isHover = false;
            update();
            break;
        default:
            break;
        }
    }
    return QPushButton::eventFilter(watched, event);
}

void ComboBox::setMaxVisibleItems(int count)
{
    Q_D(ComboBox);
    d->m_maxVisibleItems = count;
}

int ComboBox::maxVisibleItems() const
{
    Q_D(const ComboBox);
    return d->m_maxVisibleItems;
}
