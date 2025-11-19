#pragma once

#include <QLineEdit>
#include <QToolButton>
#include <QPointer>

#include "FluentGlobal.h"
#include "menu/RoundMenu.h"

class QAction;
class QEvent;
class QCompleter;
class QHBoxLayout;
class LineEditButton;
class CompleterMenu;
class QAbstractItemModel;
class QFLUENT_EXPORT LineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit LineEdit(QWidget* parent = nullptr);
    ~LineEdit() = default;

    void setClearButtonEnabled(bool enable);
    bool isClearButtonEnabled() const;

    void setCompleter(QCompleter* completer);
    QCompleter* completer() const;

    void addAction(QAction* action, QLineEdit::ActionPosition position = QLineEdit::TrailingPosition);
    void addActions(QList<QAction*> actions, QLineEdit::ActionPosition position = QLineEdit::TrailingPosition);

    LineEditButton *getClearButton();

    QHBoxLayout* hBoxLayout() const { return m_layout; }

    void setCompleterMenu(CompleterMenu *menu);


protected:
    void focusOutEvent(QFocusEvent* e) override;
    void focusInEvent(QFocusEvent* e) override;
    void contextMenuEvent(QContextMenuEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void handleTextChanged(const QString& text);
    void handleTextEdited(const QString& text);
    void showCompleterMenu();

private:
    void adjustTextMargins();
    void initClearButton();

    QHBoxLayout* m_layout;
    LineEditButton* m_clearButton;
    QCompleter* m_completer = nullptr;
    QTimer* m_completerTimer;
    QList<LineEditButton*> m_leftButtons;
    QList<LineEditButton*> m_rightButtons;
    bool m_clearButtonEnabled = false;
    QPointer<CompleterMenu> m_completerMenu;
};




class LineEditButton : public QToolButton {
    Q_OBJECT

public:
    explicit LineEditButton(const QIcon& icon, QWidget* parent = nullptr);
    void setAction(QAction* action);
    QAction* action() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private slots:
    void updateButtonState();

private:
    QAction* m_action = nullptr;
    QIcon m_icon;
};







class QFLUENT_EXPORT SearchLineEdit : public LineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget *parent = nullptr);

signals:
    void searchSignal(const QString &text);
    void clearSignal();

public slots:
    void search();

private slots:
    void onClearButtonClicked();

private:
    LineEditButton *searchButton;
    LineEditButton *clearButton;
    QHBoxLayout *hBoxLayout;

    void initWidgets();
    void updateTextMargins();
};








class QFLUENT_EXPORT CompleterMenu : public RoundMenu
{
    Q_OBJECT

public:
    explicit CompleterMenu(LineEdit* lineEdit, QWidget* parent = nullptr);

    bool setCompletion(QAbstractItemModel* model, int column = 0);

    void setItems(const QStringList& items);

    void popup();

signals:
    void activated(const QString& text);
    void indexActivated(const QModelIndex& index);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void _onItemClicked(QListWidgetItem* item);

private:
    void _onCompletionItemSelected(const QString& text, int row);

    QPointer<LineEdit> lineEdit() const { return m_lineEdit; }

private:
    QPointer<LineEdit> m_lineEdit;

    QStringList m_items;
    QVector<QModelIndex> m_indexes;

};






class QFLUENT_EXPORT PasswordLineEdit : public LineEdit
{
    Q_OBJECT

public:
    explicit PasswordLineEdit(QWidget *parent = nullptr);

    // 设置/获取密码是否可见
    void setPasswordVisible(bool isVisible);
    bool isPasswordVisible() const;

    void setClearButtonEnabled(bool enable);

    // 控制“查看密码”按钮是否显示
    void setViewPasswordButtonVisible(bool isVisible);

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

private:
    LineEditButton *viewButton = nullptr;
};
