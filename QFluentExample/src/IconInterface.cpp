#include "IconInterface.h"
#include <QApplication>
#include <QFontMetrics>
#include <QMetaEnum>
#include <QScrollBar>

#include "QFluent/scrollbar/ScrollBar.h"

// 辅助函数（需要根据实际实现）
FluentIconType::IconType getDefaultFluentIcon() {
    // 返回一个默认的 FluentIcon，实际实现中需要具体定义
    static FluentIconType::IconType defaultIcon;
    return defaultIcon;
}

QVector<FluentIconType::IconType> getAllFluentIcons() {
    // 返回所有 FluentIcon，实际实现中需要具体定义
    QVector<FluentIconType::IconType> icons;
    // 填充 icons
    return icons;
}


// TrieNode 实现
TrieNode::TrieNode() {}

TrieNode::~TrieNode() {
    for (auto child : children) {
        delete child;
    }
}

// Trie 实现
Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    delete root;
}

void Trie::insert(const QString& word, int index) {
    TrieNode* node = root;

    for (const QChar& ch : word.toLower()) {
        if (!node->children.contains(ch)) {
            node->children[ch] = new TrieNode();
        }
        node = node->children[ch];
    }

    node->indexes.append(index);
}

QVector<QPair<QString, int>> Trie::items(const QString& prefix) {
    QVector<QPair<QString, int>> results;
    TrieNode* node = root;

    // 导航到前缀的最后一个节点
    for (const QChar& ch : prefix.toLower()) {
        if (!node->children.contains(ch)) {
            return results; // 前缀不存在
        }
        node = node->children[ch];
    }

    // 收集所有以该前缀开头的单词
    collectWords(node, prefix, results);
    return results;
}

void Trie::collectWords(TrieNode* node, const QString& prefix, QVector<QPair<QString, int>>& results) {
    // 添加当前节点的所有索引
    for (int index : node->indexes) {
        results.append(qMakePair(prefix, index));
    }

    // 递归处理子节点
    for (auto it = node->children.begin(); it != node->children.end(); ++it) {
        collectWords(it.value(), prefix + it.key(), results);
    }
}

// IconCard 实现
IconCard::IconCard(FluentIconType::IconType icon, const QString &name, QWidget* parent)
    : QFrame(parent), m_icon(icon), m_isSelected(false) {

    m_iconWidget = new IconWidget(FluentIcon(icon), this);
    m_nameLabel = new QLabel(this);
    m_vBoxLayout = new QVBoxLayout(this);

    setFixedSize(96, 96);
    m_vBoxLayout->setSpacing(0);
    m_vBoxLayout->setContentsMargins(8, 28, 8, 0);
    m_vBoxLayout->setAlignment(Qt::AlignTop);

    m_iconWidget->setFixedSize(28, 28);
    m_vBoxLayout->addWidget(m_iconWidget, 0, Qt::AlignHCenter);
    m_vBoxLayout->addSpacing(14);
    m_vBoxLayout->addWidget(m_nameLabel, 0, Qt::AlignHCenter);

    QFontMetrics metrics(m_nameLabel->font());
    QString elidedText = metrics.elidedText(name, Qt::ElideRight, 90);
    m_nameLabel->setText(elidedText);

    connect(Theme::instance(), &Theme::themeModeChanged, this, [=](ThemeType::ThemeMode theme){
         if (m_isSelected) {
             m_iconWidget->setIconTheme(theme);
         }
    });
}

void IconCard::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isSelected) {
        return;
    }

    emit clicked(m_icon);
    QFrame::mouseReleaseEvent(event);
}

void IconCard::setSelected(bool isSelected, bool force) {
    if (isSelected == m_isSelected && !force) {
        return;
    }
    m_isSelected = isSelected;

    if (!isSelected) {
        m_iconWidget->setIconTheme(ThemeType::AUTO);
    } else {
        m_iconWidget->setIconTheme(Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
    }

    setProperty("isSelected", isSelected);

    style()->unpolish(this);
    style()->polish(this);

    m_nameLabel->style()->unpolish(m_nameLabel);
    m_nameLabel->style()->polish(m_nameLabel);
}

// IconInfoPanel 实现
IconInfoPanel::IconInfoPanel(FluentIconType::IconType icon, QWidget* parent)
    : QFrame(parent) {

    m_nameLabel = new QLabel("value", this);
    m_iconWidget = new IconWidget(FluentIcon(icon), this);
    m_iconNameTitleLabel = new QLabel("图标名字", this);
    m_iconNameLabel = new QLabel("value", this);
    m_enumNameTitleLabel = new QLabel("枚举成员", this);
    m_enumNameLabel = new QLabel("FluentIcon.name", this);

    m_vBoxLayout = new QVBoxLayout(this);
    m_vBoxLayout->setContentsMargins(16, 20, 16, 20);
    m_vBoxLayout->setSpacing(0);
    m_vBoxLayout->setAlignment(Qt::AlignTop);

    m_vBoxLayout->addWidget(m_nameLabel);
    m_vBoxLayout->addSpacing(16);
    m_vBoxLayout->addWidget(m_iconWidget);
    m_vBoxLayout->addSpacing(45);
    m_vBoxLayout->addWidget(m_iconNameTitleLabel);
    m_vBoxLayout->addSpacing(5);
    m_vBoxLayout->addWidget(m_iconNameLabel);
    m_vBoxLayout->addSpacing(34);
    m_vBoxLayout->addWidget(m_enumNameTitleLabel);
    m_vBoxLayout->addSpacing(5);
    m_vBoxLayout->addWidget(m_enumNameLabel);

    m_iconWidget->setFixedSize(48, 48);
    setFixedWidth(216);

    m_nameLabel->setObjectName("nameLabel");
    m_iconNameTitleLabel->setObjectName("subTitleLabel");
    m_enumNameTitleLabel->setObjectName("subTitleLabel");
}

void IconInfoPanel::setIcon(FluentIconType::IconType icon) {
    static QMap<FluentIconType::IconType, QString> icons = FluentIcon::fluentIcons();

    m_iconWidget->setFluentIcon(FluentIcon(icon));
    m_nameLabel->setText(icons.value(icon));
    m_iconNameLabel->setText(icons.value(icon));
    QMetaEnum metaEnum = QMetaEnum::fromType<FluentIconType::IconType>();
    QString enumQString;
    if (metaEnum.isValid()) {
        const char* enumName = metaEnum.valueToKey(static_cast<int>(icon));
        enumQString = QString::fromUtf8(enumName);
    }
    enumQString = enumQString.isEmpty() ? "NONE" : enumQString;
    m_enumNameLabel->setText(QString("FluentIconType::%1").arg(enumQString));
}

// LineEdit 实现
CustomLineEdit::CustomLineEdit(QWidget* parent)
    : SearchLineEdit(parent) {

    setPlaceholderText("搜索图标");
    setFixedWidth(304);
    connect(this, &CustomLineEdit::textChanged, this, &CustomLineEdit::onTextChanged);
}

void CustomLineEdit::onTextChanged(const QString& text) {
    if (text.isEmpty()) {
        emit clearSignal();
    } else {
        emit search(text);
    }
}

IconCardView::IconCardView(QWidget* parent)
    : QWidget(parent),
      m_currentIndex(-1) {

    m_trie = new Trie();
    m_iconLibraryLabel = new StrongBodyLabel("流畅图标库", this);
    Theme::instance()->setFont(m_iconLibraryLabel, 14, QFont::Normal);
    m_searchLineEdit = new CustomLineEdit(this);

    m_view = new QFrame(this);
    m_scrollArea = new ScrollArea(Qt::Vertical, m_view);


    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ScrollBar* floatVScrollBar = new ScrollBar(m_scrollArea->verticalScrollBar(), m_scrollArea);
    floatVScrollBar->setIsAnimation(true);


    m_scrollWidget = new QWidget(m_scrollArea);
    m_infoPanel = new IconInfoPanel(getDefaultFluentIcon(), this);

    m_vBoxLayout = new QVBoxLayout(this);
    m_hBoxLayout = new QHBoxLayout(m_view);

    m_flowLayout = new FlowLayout(m_scrollWidget, false);

    initWidget();
}

void IconCardView::initWidget() {
    m_scrollArea->setWidget(m_scrollWidget);
    m_scrollArea->setViewportMargins(0, 5, 0, 5);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_vBoxLayout->setSpacing(12);
    m_vBoxLayout->addWidget(m_iconLibraryLabel);
    m_vBoxLayout->addWidget(m_searchLineEdit);
    m_vBoxLayout->addWidget(m_view);

    m_hBoxLayout->setSpacing(0);
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_hBoxLayout->addWidget(m_scrollArea);
    m_hBoxLayout->addWidget(m_infoPanel, 0, Qt::AlignRight);

    m_flowLayout->setVerticalSpacing(8);
    m_flowLayout->setHorizontalSpacing(8);
    m_flowLayout->setContentsMargins(8, 3, 8, 8);

    setQss();

    connect(m_searchLineEdit, &CustomLineEdit::search, this, &IconCardView::search);
    connect(m_searchLineEdit, &CustomLineEdit::clearSignal, this, &IconCardView::showAllIcons);

    const QMap<FluentIconType::IconType, QString> allIcons = FluentIcon::fluentIcons();
    for (FluentIconType::IconType icon : allIcons.keys()) {
        addIcon(icon, allIcons.value(icon));
    }

    if (!m_icons.isEmpty()) {
        setSelectedIcon(m_icons[0]);
    }
}

void IconCardView::addIcon(FluentIconType::IconType icon, const QString &name) {
    IconCard* card = new IconCard(icon, name, m_scrollWidget);
    connect(card, &IconCard::clicked, this, &IconCardView::setSelectedIcon);

    m_trie->insert(name, m_cards.size());
    m_cards.append(card);
    m_icons.append(icon);
    m_flowLayout->addWidget(card);
}

void IconCardView::setSelectedIcon(FluentIconType::IconType icon) {
    int index = m_icons.indexOf(icon);
    if (index == -1) {
        return;
    }

    if (m_currentIndex >= 0 && m_currentIndex < m_cards.size()) {
        m_cards[m_currentIndex]->setSelected(false);
    }

    m_currentIndex = index;
    m_cards[index]->setSelected(true);
    m_infoPanel->setIcon(icon);
}

void IconCardView::setQss() {
    m_view->setObjectName("iconView");
    m_scrollWidget->setObjectName("scrollWidget");

    {
        auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/icon_interface.qss");
        StyleSheetManager::instance()->registerWidget(styleSource, this);
    }
    {
        auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/icon_interface.qss");
        StyleSheetManager::instance()->registerWidget(styleSource, m_scrollWidget);
    }

    if (m_currentIndex >= 0 && m_currentIndex < m_cards.size()) {
        m_cards[m_currentIndex]->setSelected(true, true);
    }
}

void IconCardView::search(const QString& text) {
    QVector<QPair<QString, int>> items = m_trie->items(text.toLower());
    QSet<int> indexes;
    for (const auto& item : items) {
        indexes.insert(item.second);
    }

    m_flowLayout->removeAllWidgets();

    for (int i = 0; i < m_cards.size(); ++i) {
        bool isVisible = indexes.contains(i);
        m_cards[i]->setVisible(isVisible);
        if (isVisible) {
            m_flowLayout->addWidget(m_cards[i]);
        }
    }
}

void IconCardView::showAllIcons() {
    m_flowLayout->removeAllWidgets();
    for (IconCard* card : m_cards) {
        card->show();
        m_flowLayout->addWidget(card);
    }
}

// IconInterface 实现
IconInterface::IconInterface(QWidget* parent)
    : GalleryInterface("图标", "qfluentwidgets.common.icon", parent) {

    setObjectName("iconInterface");
    m_iconView = new IconCardView(this);
    layout()->addWidget(m_iconView);
}


