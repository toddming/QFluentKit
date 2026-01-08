#include "AcrylicMenu.h"
#include <QApplication>
#include <QStyle>
#include <QDebug>  // 可选，用于调试

AcrylicMenuActionListWidget::AcrylicMenuActionListWidget(QWidget *parent)
    : MenuActionListWidget(parent),
      acrylicBrush(viewport(), 35, QColor(242, 242, 242, 150)) {  // 假设AcrylicBrush构造函数为(parent, opacity)
    setViewportMargins(0, 0, 0, 0);
    setProperty("transparent", true);
    MenuActionListWidget::addItem(createPlaceholderItem(_topMargin()));
    MenuActionListWidget::addItem(createPlaceholderItem(_bottomMargin()));
}

void AcrylicMenuActionListWidget::_updateAcrylicColor() {
    QColor tintColor, luminosityColor;
    if (Theme::instance()->isDarkTheme()) {
        tintColor = QColor(32, 32, 32, 200);
        luminosityColor = QColor(0, 0, 0, 0);
    } else {
        tintColor = QColor(255, 255, 255, 160);
        luminosityColor = QColor(255, 255, 255, 50);
    }
    acrylicBrush.setTintColor(tintColor);  // 假设AcrylicBrush有setTintColor和setLuminosityColor方法
    acrylicBrush.setLuminosityColor(luminosityColor);
}

int AcrylicMenuActionListWidget::_topMargin() const {
    return 6;
}

int AcrylicMenuActionListWidget::_bottomMargin() const {
    return 6;
}

void AcrylicMenuActionListWidget::setItemHeight(int height) {
    if (height == _itemHeight) {
        return;
    }
    for (int i = 1; i < count() - 1; ++i) {
        QListWidgetItem *item = this->item(i);
        if (!itemWidget(item)) {
            QSize size = item->sizeHint();
            size.setHeight(height);
            item->setSizeHint(size);
        }
    }
    _itemHeight = height;
    adjustSize();
}

void AcrylicMenuActionListWidget::addItem(QListWidgetItem *item) {
    insertItem(count() - 1, item);
}

QListWidgetItem* AcrylicMenuActionListWidget::createPlaceholderItem(int height) {
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(1, height));
    item->setFlags(Qt::NoItemFlags);
    return item;
}

QPainterPath AcrylicMenuActionListWidget::clipPath() const {
    QPainterPath path;
    path.addRoundedRect(QRectF(rect()).adjusted(0, 0, -2.5, -2.5), 8, 8);
    return path;
}

void AcrylicMenuActionListWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    acrylicBrush.setClipPath(clipPath());  // 假设AcrylicBrush有setClipPath方法
    _updateAcrylicColor();
    acrylicBrush.paint();  // 假设AcrylicBrush有paint方法
    MenuActionListWidget::paintEvent(event);
}

AcrylicMenu::AcrylicMenu(const QString &title, QWidget *parent)
    : RoundMenu(title, parent)
{
    listWidget = new AcrylicMenuActionListWidget(this);
    setUpMenu(listWidget);
}

void AcrylicMenu::setUpMenu(AcrylicMenuActionListWidget *newView) {
    if (view()) {
        hBoxLayout()->removeWidget(view());
        view()->deleteLater();
    }
    setView(newView);
    hBoxLayout()->addWidget(newView);
    // setShadowEffect();  // 假设RoundMenu有setShadowEffect方法
    // connect(newView, &AcrylicMenuActionListWidget::itemClicked, this, &AcrylicMenu::_onItemClicked);  // 假设_onItemClicked是RoundMenu的槽
    // connect(newView, &AcrylicMenuActionListWidget::itemEntered, this, &AcrylicMenu::_onItemEntered);  // 同上
}

void AcrylicMenu::exec(const QPoint &pos, bool animate, Fluent::MenuAnimation aniType) {
    QPoint p = MenuAnimationManager::make(this, aniType)->endPosition(pos);  // 假设make返回对象，有endPosition方法
    listWidget->acrylicBrush.grabImage(QRect(p, layout()->sizeHint()));  // 假设acrylicBrush有grabImage方法
    RoundMenu::exec(pos, animate, aniType);
}
