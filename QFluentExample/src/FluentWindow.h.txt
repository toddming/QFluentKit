#ifndef FLUENTWINDOW_H
#define FLUENTWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QPainter>
#include <QPushButton>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QStyle>
#include <QStackedWidget>
#include <QTimer>

#include "Property.h"

#include "Theme.h"
#include "SwitchButton.h"
#include "RadioButton.h"
#include "LineEdit.h"
#include "PushButton.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include "menu/RoundMenu.h"
#include "SpinBox.h"
#include "Label.h"
#include "TableView.h"
#include "InfoBar.h"
#include "dialog/MessageDialog.h"
#include "Loading.h"
#include "ToolButton.h"
#include "PagiNation.h"
#include "navigation/NavigationPanel.h"
#include "StackedWidget.h"
#include "FluentTitleBar.h"
#include "EditableComboBox.h"
#include "date_time/CalendarView.h"

#include "settings/SettingCard.h"
#include "settings/SettingCardGroup.h"
#include "Icon.h"
#include "layout/ExpandLayout.h"
#include "ScrollArea.h"
#include "settings/OptionsSettingCard.h"
#include "date_time/CalendarPicker.h"

#include "navigation/Pivot.h"
#include "navigation/NavigationBar.h"
#include "menu/CheckableMenu.h"

#include "layout/FlowLayout.h"

#include "progress/ProgressBar.h"
#include "progress/ProgressRing.h"
#include "progress/IndeterminateProgressBar.h"

#include "dialog/MessageBoxBase.h"

#include "examples/GalleryInterface.h"

class QFLUENT_EXPORT FluentWindow : public QMainWindow
{
    Q_OBJECT

public:
    FluentWindow(QMainWindow *parent = nullptr);
    ~FluentWindow();

protected:
    bool event(QEvent *event) override;

private:
    void installAgent();

    void initUI();

    void loadStyleSheet(bool);

    QObject *windowAgent{nullptr};

    QStackedWidget *stackedWidget{nullptr};

    void switchTo(QWidget *w);

    StackedWidget *stacked;

    FlowLayout *m_flay;

    QList<PushButton*> m_btns;

    ProgressBar *m_progressBar;
};
#endif // FLUENTWINDOW_H
