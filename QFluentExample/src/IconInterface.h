#ifndef ICON_INTERFACE_H
#define ICON_INTERFACE_H

#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QLineEdit>
#include <QMap>
#include <QVector>
#include <QString>
#include <QMouseEvent>
#include <QObject>

#include "GalleryInterface.h"

#include "FluentIcon.h"
#include "Theme.h"
#include "FluentGlobal.h"
#include "StyleSheet.h"
#include "QFluent/Label.h"
#include "QFluent/LineEdit.h"
#include "QFluent/PushButton.h"
#include "QFluent/ScrollArea.h"
#include "QFluent/layout/FlowLayout.h"


// Trie树实现
class TrieNode {
public:
    QMap<QChar, TrieNode*> children;
    QVector<int> indexes; // 存储图标索引

    TrieNode();
    ~TrieNode();
};

class Trie {
private:
    TrieNode* root;

public:
    Trie();
    ~Trie();

    void insert(const QString& word, int index);
    QVector<QPair<QString, int>> items(const QString& prefix);

private:
    void collectWords(TrieNode* node, const QString& prefix, QVector<QPair<QString, int>>& results);
};

// 搜索框
class CustomLineEdit : public SearchLineEdit {
    Q_OBJECT

public:
    explicit CustomLineEdit(QWidget* parent = nullptr);

signals:
    void search(const QString& text);
    void clearSignal();

private slots:
    void onTextChanged(const QString& text);
};

// 图标卡片
class IconCard : public QFrame {
    Q_OBJECT

public:
    explicit IconCard(Fluent::IconType icon, const QString &name, QWidget* parent = nullptr);
    void setSelected(bool isSelected, bool force = false);

signals:
    void clicked(Fluent::IconType icon);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    Fluent::IconType m_icon;
    bool m_isSelected;
    IconWidget* m_iconWidget;
    QLabel* m_nameLabel;
    QVBoxLayout* m_vBoxLayout;
};

// 图标信息面板
class IconInfoPanel : public QFrame {
    Q_OBJECT

public:
    explicit IconInfoPanel(Fluent::IconType icon, QWidget* parent = nullptr);
    void setIcon(Fluent::IconType icon);

private:
    QLabel* m_nameLabel;
    IconWidget* m_iconWidget;
    QLabel* m_iconNameTitleLabel;
    QLabel* m_iconNameLabel;
    QLabel* m_enumNameTitleLabel;
    QLabel* m_enumNameLabel;
    QVBoxLayout* m_vBoxLayout;
};



// 图标卡片视图
class IconCardView : public QWidget {
    Q_OBJECT

public:
    explicit IconCardView(QWidget* parent = nullptr);
    void addIcon(Fluent::IconType icon, const QString &name);
    void setSelectedIcon(Fluent::IconType icon);

    void showAllIcons();

private slots:
    void search(const QString& text);

private:
    void initWidget();
    void setQss();

    Trie* m_trie;
    StrongBodyLabel* m_iconLibraryLabel;
    CustomLineEdit* m_searchLineEdit;
    QFrame* m_view;
    ScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;
    IconInfoPanel* m_infoPanel;
    QVBoxLayout* m_vBoxLayout;
    QHBoxLayout* m_hBoxLayout;
    FlowLayout* m_flowLayout;

    QVector<IconCard*> m_cards;
    QVector<Fluent::IconType> m_icons;
    int m_currentIndex;
};

// 图标界面
class IconInterface : public GalleryInterface {
    Q_OBJECT

public:
    explicit IconInterface(QWidget* parent = nullptr);

private:
    IconCardView* m_iconView;
};

#endif // ICON_INTERFACE_H
