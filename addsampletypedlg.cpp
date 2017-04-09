#include "addsampletypedlg.h"
#include "ui_addsampletypedlg.h"
#include <QMessageBox>
#include "fdatatbase.h"

AddSampleTypeDlg::AddSampleTypeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSampleTypeDlg)
{
    ui->setupUi(this);
    QObject::connect(ui->pbOK,SIGNAL(clicked()),this,SLOT(onSureToAddSampleClicked()));
    QObject::connect(ui->pbCancel,SIGNAL(clicked()),this,SLOT(close()));
}

AddSampleTypeDlg::~AddSampleTypeDlg()
{
    delete ui;
}

void AddSampleTypeDlg::onSureToAddSampleClicked()
{
    QString str_sample_type = ui->leSampleType->displayText();
    QString str_sample_desc = ui->leSampleDesc->displayText();

    if(str_sample_desc.isEmpty()||str_sample_type.isEmpty()){
         QMessageBox::information(0, "提示信息",QObject::tr("样本类型和描述不能为空！"));
         return;
    }
    QString str_sample_comment = ui->teSampleComment->toPlainText();

    if(!add_sample_info(str_sample_type,str_sample_desc,str_sample_comment)){
        QMessageBox::information(0, "提示信息",QObject::tr("无法添加该样本类型，可能已经存在了！"));
        return;
    }

    accept();

}
