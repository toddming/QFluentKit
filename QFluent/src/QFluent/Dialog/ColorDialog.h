#pragma once

#include "FluentGlobal.h"
#include "MaskDialogBase.h"
#include "QFluent/Slider.h"
#include "QFluent/LineEdit.h"

class QLabel;
class PushButton;
class QVBoxLayout;
class QPushButton;
class PrimaryPushButton;
class SingleDirectionScrollArea;

/**
 * @brief 色调面板
 */
class HuePanel : public QWidget
{
    Q_OBJECT

public:
    explicit HuePanel(const QColor& color = QColor(255, 0, 0), QWidget* parent = nullptr);

    void setColor(const QColor& color);
    int hue() const { return m_color.hue(); }
    int saturation() const { return m_color.saturation(); }

signals:
    void colorChanged(const QColor& color);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    void setPickerPosition(const QPoint& pos);

    QPixmap m_huePixmap;
    QColor m_color;
    QPoint m_pickerPos;
};


/**
 * @brief 亮度滑块
 */
class BrightnessSlider : public ClickableSlider
{
    Q_OBJECT

public:
    explicit BrightnessSlider(const QColor& color, QWidget* parent = nullptr);

    void setColor(const QColor& color);

signals:
    void colorChanged(const QColor& color);

private slots:
    void onValueChanged(int value);

private:
    QColor m_color;
};


/**
 * @brief 颜色卡片
 */
class ColorCard : public QWidget
{
    Q_OBJECT

public:
    explicit ColorCard(const QColor& color, QWidget* parent = nullptr, bool enableAlpha = false);

    void setColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QPixmap createTitledBackground();

    QColor m_color;
    bool m_enableAlpha;
    QPixmap m_titledPixmap;
};


/**
 * @brief 颜色行编辑框
 */
class ColorLineEdit : public LineEdit
{
    Q_OBJECT

public:
    explicit ColorLineEdit(const QString& value, QWidget* parent = nullptr);

signals:
    void valueChanged(const QString& value);

private slots:
    void onTextEdited(const QString& text);
};


/**
 * @brief 十六进制颜色行编辑框
 */
class HexColorLineEdit : public ColorLineEdit
{
    Q_OBJECT

public:
    explicit HexColorLineEdit(const QColor& color, QWidget* parent = nullptr, bool enableAlpha = false);

    void setColor(const QColor& color);

private:
    QColor::NameFormat m_colorFormat;
    QLabel* m_prefixLabel;
};


/**
 * @brief 不透明度行编辑框
 */
class OpacityLineEdit : public ColorLineEdit
{
    Q_OBJECT

public:
    explicit OpacityLineEdit(int value, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* e) override;

private slots:
    void adjustSuffixPos();

private:
    QLabel* m_suffixLabel;
};


/**
 * @brief 颜色选择对话框
 */
class QFLUENT_EXPORT ColorDialog : public MaskDialogBase
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param color 初始颜色
     * @param title 对话框标题
     * @param parent 父窗口
     * @param enableAlpha 是否启用 Alpha 通道
     */
    explicit ColorDialog(const QColor& color, const QString& title,
                        QWidget* parent = nullptr, bool enableAlpha = false);

    void setColor(const QColor& color, bool movePicker = true);
    QColor color() const { return m_color; }
    void updateStyle();

signals:
    void colorChanged(const QColor& color);

protected:
    void showEvent(QShowEvent* e) override;

private slots:
    void onHueChanged(const QColor& color);
    void onBrightnessChanged(const QColor& color);
    void onRedChanged(const QString& red);
    void onBlueChanged(const QString& blue);
    void onGreenChanged(const QString& green);
    void onOpacityChanged(const QString& opacity);
    void onHexColorChanged(const QString& color);
    void onYesButtonClicked();

private:
    void initWidget();
    void initLayout();
    void setQss();
    void connectSignalToSlot();

    bool m_enableAlpha;
    QColor m_oldColor;
    QColor m_color;

    // Widgets
    SingleDirectionScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;
    QFrame* m_buttonGroup;
    PrimaryPushButton* m_yesButton;
    QPushButton* m_cancelButton;

    QLabel* m_titleLabel;
    HuePanel* m_huePanel;
    ColorCard* m_newColorCard;
    ColorCard* m_oldColorCard;
    BrightnessSlider* m_brightSlider;

    QLabel* m_editLabel;
    QLabel* m_redLabel;
    QLabel* m_blueLabel;
    QLabel* m_greenLabel;
    QLabel* m_opacityLabel;

    HexColorLineEdit* m_hexLineEdit;
    ColorLineEdit* m_redLineEdit;
    ColorLineEdit* m_greenLineEdit;
    ColorLineEdit* m_blueLineEdit;
    OpacityLineEdit* m_opacityLineEdit;

    QVBoxLayout* m_vBoxLayout;
};
