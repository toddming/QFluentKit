#include "DialogInputInterface.h"

#include "QFluent/Label.h"
#include "QFluent/Loading.h"
#include "QFluent/LineEdit.h"
#include "QFluent/PushButton.h"
#include "QFluent/dialog/MessageDialog.h"
#include "QFluent/dialog/MessageBoxBase.h"

DialogInputInterface::DialogInputInterface(QWidget *parent)
    : GalleryInterface("对话框", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("DialogInputInterface");

    auto messageDialogBtn = new PushButton("显示对话框", this);
    connect(messageDialogBtn, &PushButton::clicked, this, [this](){
        auto box = new MessageDialog("这是一个带有遮罩的对话框",
                                     "一生消えない傷でいいな，絆創膏の様にいつも包んでよ。貴方のそばでわがまま言いたいな，一分一秒刻み貴方を知り，あたしをあげる~",
                                     this->window());
        box->setIsClosableOnMaskClicked(true);
        box->exec();
    });
    addExampleCard("带遮罩的对话框", messageDialogBtn);

    auto loadingDialogBtn = new PushButton("显示对话框", this);
    connect(loadingDialogBtn, &PushButton::clicked, this, [this](){
        static Loading *load = new Loading(QString(), this->window());
        load->exec();
    });
    addExampleCard("加载中等待窗口", loadingDialogBtn);

    auto customDialogBtn = new PushButton("显示对话框", this);
    auto customDialog = new MessageBoxBase(this->window());
    customDialog->viewLayout()->addWidget(new SubtitleLabel("打开 URL", customDialog));
    customDialog->viewLayout()->addWidget(new LineEdit(customDialog));
    customDialog->centerWidget()->setFixedWidth(360);
    customDialog->close();

    connect(customDialogBtn, &PushButton::clicked, this, [customDialog](){
        customDialog->exec();
    });
    addExampleCard("自定义对话框", customDialogBtn);

}
