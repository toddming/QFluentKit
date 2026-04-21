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

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    installEventFilter(this);

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::COMBO_BOX);

    d->m_arrowAni = new TranslateYAnimation(this);
    d->updateText();  // 初始化文本
}

MultiViewComboBox::~MultiViewComboBox() = default;

void MultiViewComboBox::addItem(const QString &text, const QVariant &userData)
{
    insertItem(count(), text, userData);
}

void MultiViewComboBox::addItem(const QIcon &icon, const QString &text, const QVariant &userData)
{
    insertItem(count(), icon, text, userData);
}

void MultiViewComboBox::addItems(const QStringList &texts)
{
    for (const QString &text : texts) {
        addItem(text);
    }
}

void MultiViewComboBox::insertItem(int index, const QString &text, const QVariant &userData)
{
    Q_D(MultiViewComboBox);
    if (index < 0 || index > count()) {
        index = count();
    }

    d->m_items.insert(index, MultiViewComboBoxDetail::ComboItem(text, QIcon(), userData));

    // 调整选中索引（如果插入在选中项前，索引需后移）
    for (int &selIndex : d->m_selectedIndexes) {
        if (selIndex >= index) {
            ++selIndex;
        }
    }

    d->updateText();
}

void MultiViewComboBox::insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userData)
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

void MultiViewComboBox::insertSeparator(int index)
{
    Q_D(MultiViewComboBox);
    if (index < 0 || index > count()) {
        index = count();
    }

    MultiViewComboBoxDetail::ComboItem item;
    item.isSeparator = true;
    d->m_items.insert(index, item);

    // 调整选中索引（如果插入在选中项前，索引需后移）
    for (int &selIndex : d->m_selectedIndexes) {
        if (selIndex >= index) {
            ++selIndex;
        }
    }

    d->updateText();
}

void MultiViewComboBox::removeItem(int index)
{
    Q_D(MultiViewComboBox);

    if (index < 0 || index >= count()) {
        return;
    }

    bool wasSelected = d->m_selectedIndexes.contains(index);

    d->m_items.removeAt(index);

    // 移除选中并发射信号
    if (wasSelected) {
        d->m_selectedIndexes.removeAll(index);
        emit itemDeselected(index);
    }

    // 调整剩余选中索引
    for (int &selIndex : d->m_selectedIndexes) {
        if (selIndex > index) {
            --selIndex;
        }
    }

    d->updateText();
    if (wasSelected) {
        emit selectionChanged();
    }
}

void MultiViewComboBox::clear()
{
    Q_D(MultiViewComboBox);

    bool hadSelection = !d->m_selectedIndexes.isEmpty();

    // 发射 itemDeselected 信号
    for (int selIndex : d->m_selectedIndexes) {
        emit itemDeselected(selIndex);
    }

    d->m_items.clear();
    d->m_selectedIndexes.clear();
    d->updateText();

    if (hadSelection) {
        emit selectionChanged();
    }
}

void MultiViewComboBox::setItemSelected(int index, bool selected)
{
    Q_D(MultiViewComboBox);

    if (index < 0 || index >= count()) {
        return;
    }

    // separator 不可选中
    if (d->m_items[index].isSeparator) {
        return;
    }

    bool wasSelected = d->m_selectedIndexes.contains(index);
    if (selected == wasSelected) {
        return;
    }

    // 超过最大选择数限制时禁止选中
    if (selected && d->m_maxSelectedCount > 0
        && d->m_selectedIndexes.size() >= d->m_maxSelectedCount) {
        return;
    }

    if (selected) {
        d->m_selectedIndexes.append(index);
        std::sort(d->m_selectedIndexes.begin(), d->m_selectedIndexes.end());
        emit itemSelected(index);
    } else {
        d->m_selectedIndexes.removeAll(index);
        emit itemDeselected(index);
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

int MultiViewComboBox::maxSelectedCount() const
{
    Q_D(const MultiViewComboBox);
    return d->m_maxSelectedCount;
}

void MultiViewComboBox::setMaxSelectedCount(int max)
{
    Q_D(MultiViewComboBox);
    if (d->m_maxSelectedCount == max) {
        return;
    }

    d->m_maxSelectedCount = max;

    // 如果当前选中数已超过新限制，移除多余的选中项
    if (max > 0 && d->m_selectedIndexes.size() > max) {
        while (d->m_selectedIndexes.size() > max) {
            int index = d->m_selectedIndexes.takeLast();
            emit itemDeselected(index);
        }
        d->updateText();
        emit selectionChanged();
    }
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

QVariant MultiViewComboBox::itemData(int index, int role) const
{
    Q_D(const MultiViewComboBox);

    if (role == Qt::UserRole && index >= 0 && index < count()) {
        return d->m_items[index].userData;
    }
    return {};
}

void MultiViewComboBox::setItemText(int index, const QString &text)
{
    Q_D(MultiViewComboBox);
    if (index >= 0 && index < count()) {
        d->m_items[index].text = text;
        d->updateText();
    }
}

void MultiViewComboBox::setItemIcon(int index, const QIcon &icon)
{
    Q_D(MultiViewComboBox);
    if (index >= 0 && index < count()) {
        d->m_items[index].icon = icon;
    }
}

void MultiViewComboBox::setItemData(int index, const QVariant &value, int role)
{
    Q_D(MultiViewComboBox);
    if (role == Qt::UserRole && index >= 0 && index < count()) {
        d->m_items[index].userData = value;
    }
}

int MultiViewComboBox::findText(const QString &text, Qt::MatchFlags flags) const
{
    Q_D(const MultiViewComboBox);

    for (int i = 0; i < count(); ++i) {
        if (flags & Qt::MatchCaseSensitive) {
            if (d->m_items[i].text.compare(text, Qt::CaseSensitive) == 0) {
                return i;
            }
        } else {
            if (d->m_items[i].text.compare(text, Qt::CaseInsensitive) == 0) {
                return i;
            }
        }
    }
    return -1;
}

int MultiViewComboBox::findData(const QVariant &data, int role, Qt::MatchFlags flags) const
{
    Q_D(const MultiViewComboBox);

    if (role != Qt::UserRole) {
        return -1;
    }

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
