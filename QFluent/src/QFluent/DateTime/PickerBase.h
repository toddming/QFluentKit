#pragma once

#include <QPushButton>
#include <QWidget>
#include <QObject>

#include "FluentGlobal.h"
#include "QFluent/ToolButton.h"

class QFrame;
class QHBoxLayout;
class QVBoxLayout;
class CycleListWidget;
class QListWidgetItem;
class QPropertyAnimation;
class QGraphicsDropShadowEffect;
class PickerColumnFormatter : public QObject
{
    Q_OBJECT

public:
    explicit PickerColumnFormatter(QObject* parent = nullptr);
    virtual ~PickerColumnFormatter() = default;

    virtual QString encode(const QVariant& value);
    virtual QVariant decode(const QString& value);
};

class DigitFormatter : public PickerColumnFormatter
{
    Q_OBJECT

public:
    using PickerColumnFormatter::PickerColumnFormatter;
    QVariant decode(const QString& value) override;
};

class PickerColumnButton : public QPushButton
{
    Q_OBJECT

public:
    explicit PickerColumnButton(const QString& name, const QList<QVariant>& items,
                               int width, Qt::Alignment align = Qt::AlignLeft,
                               PickerColumnFormatter* formatter = nullptr,
                               QWidget* parent = nullptr);

    Qt::Alignment align() const { return m_align; }
    void setAlignment(Qt::Alignment align);
    
    QString value() const;
    void setValue(const QVariant& v);
    
    QStringList items() const;
    void setItems(const QList<QVariant>& items);
    
    PickerColumnFormatter* formatter() const { return m_formatter; }
    void setFormatter(PickerColumnFormatter* formatter);
    
    QString name() const { return m_name; }
    void setName(const QString& name);

private:
    QString m_name;
    QVariant m_value;
    QList<QVariant> m_items;
    PickerColumnFormatter* m_formatter;
    Qt::Alignment m_align;
};

class DateSeparatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DateSeparatorWidget(Qt::Orientation orient, QWidget* parent = nullptr);
};

class ItemMaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemMaskWidget(QWidget* parent = nullptr);
    void setCustomBackgroundColor(const QColor& light, const QColor& dark);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    void drawText(QListWidgetItem* item, QPainter& painter, int y);

    QList<CycleListWidget*> m_listWidgets;
    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;
};


class PickerToolButton : public TransparentToolButton
{
    Q_OBJECT
public:
    using TransparentToolButton::TransparentToolButton;

protected:
    void drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
};


class PickerPanel;
class QFLUENT_EXPORT PickerBase : public QPushButton
{
    Q_OBJECT

public:
    explicit PickerBase(QWidget* parent = nullptr);

    void setSelectedBackgroundColor(const QColor& light, const QColor& dark);
    
    void addColumn(const QString& name, const QList<QVariant>& items, int width,
                  Qt::Alignment align = Qt::AlignCenter,
                  PickerColumnFormatter* formatter = nullptr);
    
    void setColumnAlignment(int index, Qt::Alignment align);
    void setColumnWidth(int index, int width);
    void setColumnTight(int index);
    void setColumnVisible(int index, bool isVisible);
    
    QStringList value() const;
    QStringList initialValue() const;
    
    void setColumnValue(int index, const QVariant& value);
    void setColumnFormatter(int index, PickerColumnFormatter* formatter);
    void setColumnItems(int index, const QList<QVariant>& items);
    
    QString encodeValue(int index, const QVariant& value);
    QVariant decodeValue(int index, const QString& value);
    
    void clearColumns();
    
    bool isRestEnabled() const { return m_isResetEnabled; }
    void setResetEnabled(bool isEnabled);

signals:
    void confirmed(const QStringList& value);
    void resetted();

public slots:
    virtual void reset();

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *e) override;
#else
    void enterEvent(QEvent *e) override;
#endif
    void leaveEvent(QEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    
    virtual QStringList panelInitialValue();
    virtual void onConfirmed(const QStringList& value);
    virtual void onColumnValueChanged(PickerPanel* panel, int index, const QString& value);

private slots:
    void showPanel();

private:
    void setButtonProperty(const QString& name, const QVariant& value);

protected:
    QList<PickerColumnButton*> m_columns;
    QColor m_lightSelectedBackgroundColor;
    QColor m_darkSelectedBackgroundColor;
    bool m_isResetEnabled;
    QHBoxLayout* m_hBoxLayout;
};

class PickerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit PickerPanel(QWidget* parent = nullptr);

    void setShadowEffect(int blurRadius = 30, const QPoint& offset = QPoint(0, 8),
                        const QColor& color = QColor(0, 0, 0, 30));
    
    void setResetEnabled(bool isEnabled);
    bool isResetEnabled() const;
    
    void setSelectedBackgroundColor(const QColor& light, const QColor& dark);
    
    void addColumn(const QStringList& items, int width, Qt::Alignment align = Qt::AlignCenter);
    
    QStringList value() const;
    void setValue(const QStringList& value);
    
    QString columnValue(int index) const;
    void setColumnValue(int index, const QString& value);
    
    CycleListWidget* column(int index);
    
    void exec(const QPoint& pos, bool ani = true);

    QList<CycleListWidget*> listWidgets() const;

signals:
    void confirmed(const QStringList& value);
    void resetted();
    void columnValueChanged(int index, const QString& value);

protected:
    void resizeEvent(QResizeEvent* e) override;

private slots:
    void fadeOut();
    void onAniValueChanged(const QVariant& value);

private:
    void initWidget();

    int m_itemHeight;
    QList<CycleListWidget*> m_listWidgets;
    QFrame* m_view;
    ItemMaskWidget* m_itemMaskWidget;
    DateSeparatorWidget* m_hSeparatorWidget;
    PickerToolButton* m_yesButton;
    PickerToolButton* m_resetButton;
    PickerToolButton* m_cancelButton;
    
    QHBoxLayout* m_hBoxLayout;
    QHBoxLayout* m_listLayout;
    QHBoxLayout* m_buttonLayout;
    QVBoxLayout* m_vBoxLayout;
    
    QPropertyAnimation* m_ani;
    bool m_isExpanded;
    QGraphicsDropShadowEffect* m_shadowEffect;
};
