#include "Private/MultiViewComboBoxPrivate.h"

#include "MultiViewComboBox.h"
#include <QApplication>
#include <QScreen>
#include <QPainter>

#include "Animation.h"
#include "FluentIcon.h"
#include "StyleSheet.h"

MultiViewComboBox::MultiViewComboBox(QWidget *parent)
    : QPushButton(parent)
    , d_ptr(new MultiViewComboBoxPrivate(this))
{
    Q_D(MultiViewComboBox);

    installEventFilter(this);

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::COMBO_BOX);

    d->m_arrowAni = new TranslateYAnimation(this);
    d->updateText();  // 初始化文本
}

MultiViewComboBox::~MultiViewComboBox() = default;

void MultiViewComboBox::addItem(const QString &text, const QIcon &icon, const QVariant &userData)
{
    insertItem(count(), text, icon, userData);
}

void MultiViewComboBox::addItems(const QStringList &texts)
{
    for (const QString &text : texts) {
        addItem(text);
    }
}

void MultiViewComboBox::insertItem(int index, const QString &text, const QIcon &icon, const QVariant &userData)
{
    Q_D(MultiViewComboBox);
    if (index < 0 || index > count()) {
        index = count();
    }

    d->m_items.insert(index, MultiViewComboBoxDetail::ComboItem(text, icon, userData));

    // 调整选中索引（如果插入在选中项前，索引需后移）
    for (int &selIndex : d->m_selectedIndexes) {
        if (selIndex >= index) {
            ++selIndex;
        }
    }

    d->updateText();
}

void MultiViewComboBox::insertItems(int index, const QStringList &texts)
{
    for (const QString &text : texts) {
        insertItem(index++, text);
    }
}

void MultiViewComboBox::removeItem(int index)
{
    Q_D(MultiViewComboBox);

    if (index < 0 || index >= count()) {
        return;
    }

    d->m_items.removeAt(index);

    // 移除选中
    d->m_selectedIndexes.removeAll(index);

    // 调整剩余选中索引
    for (int &selIndex : d->m_selectedIndexes) {
        if (selIndex > index) {
            --selIndex;
        }
    }

    d->updateText();
}

void MultiViewComboBox::clear()
{
    Q_D(MultiViewComboBox);

    d->m_items.clear();
    d->m_selectedIndexes.clear();
    d->updateText();
}

void MultiViewComboBox::setItemSelected(int index, bool selected)
{
    Q_D(MultiViewComboBox);

    if (index < 0 || index >= count()) {
        return;
    }

    if (selected) {
        if (!d->m_selectedIndexes.contains(index)) {
            d->m_selectedIndexes.append(index);
            std::sort(d->m_selectedIndexes.begin(), d->m_selectedIndexes.end());
        }
    } else {
        d->m_selectedIndexes.removeAll(index);
    }

    d->updateText();
    emit selectionChanged();
}

bool MultiViewComboBox::isItemSelected(int index) const
{
    Q_D(const MultiViewComboBox);
    return d->m_selectedIndexes.contains(index);
}

QList<int> MultiViewComboBox::selectedIndexes() const
{
    Q_D(const MultiViewComboBox);
    return d->m_selectedIndexes;
}

QStringList MultiViewComboBox::selectedTexts() const
{
    Q_D(const MultiViewComboBox);
    QStringList texts;
    for (int index : d->m_selectedIndexes) {
        texts.append(d->m_items[index].text);
    }
    return texts;
}

QList<QVariant> MultiViewComboBox::selectedDatas() const
{
    Q_D(const MultiViewComboBox);
    QList<QVariant> datas;
    for (int index : d->m_selectedIndexes) {
        datas.append(d->m_items[index].userData);
    }
    return datas;
}

int MultiViewComboBox::count() const
{
    Q_D(const MultiViewComboBox);
    return d->m_items.size();
}

QString MultiViewComboBox::itemText(int index) const
{
    Q_D(const MultiViewComboBox);
    if (index >= 0 && index < count()) {
        return d->m_items[index].text;
    }
    return "";
}

QIcon MultiViewComboBox::itemIcon(int index) const
{
    Q_D(const MultiViewComboBox);
    if (index >= 0 && index < count()) {
        return d->m_items[index].icon;
    }
    return QIcon();
}

QVariant MultiViewComboBox::itemData(int index) const
{
    Q_D(const MultiViewComboBox);

    if (index >= 0 && index < count()) {
        return d->m_items[index].userData;
    }
    return {};
}

int MultiViewComboBox::findText(const QString &text) const
{
    Q_D(const MultiViewComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->m_items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int MultiViewComboBox::findData(const QVariant &data) const
{
    Q_D(const MultiViewComboBox);

    for (int i = 0; i < count(); ++i) {
        if (d->m_items[i].userData == data) {
            return i;
        }
    }
    return -1;
}

void MultiViewComboBox::setPlaceholderText(const QString &text)
{
    Q_D(MultiViewComboBox);
    d->m_placeholderText = text;
    d->updateText();
}

QString MultiViewComboBox::placeholderText() const
{
    Q_D(const MultiViewComboBox);
    return d->m_placeholderText;
}

void MultiViewComboBox::setMaxVisibleItems(int count)
{
    Q_D(MultiViewComboBox);
    d->m_maxVisibleItems = count;
}

int MultiViewComboBox::maxVisibleItems() const
{
    Q_D(const MultiViewComboBox);
    return d->m_maxVisibleItems;
}

void MultiViewComboBox::paintEvent(QPaintEvent *event)
{
    Q_D(MultiViewComboBox);
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

void MultiViewComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(MultiViewComboBox);
    QPushButton::mouseReleaseEvent(event);
    d->toggleComboMenu();
}

bool MultiViewComboBox::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(MultiViewComboBox);
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
