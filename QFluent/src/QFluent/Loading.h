#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "Theme.h"
#include "dialog/MaskDialogBase.h"
#include "ProgressRing.h"
#include "Property.h"

class QFLUENT_EXPORT Loading : public MaskDialogBase
{
    Q_OBJECT
  public:
    explicit Loading(const QString &content = QString(), QWidget *parent = nullptr);

private:
    IndeterminateProgressRing *progressRing;
    QLabel *contentLabel;

private slots:
    void onThemeChanged(ThemeType::ThemeMode theme);

};
