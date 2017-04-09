#include "testparaform.h"
#include "ui_testparaform.h"

TestParaForm::TestParaForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestParaForm)
{
    ui->setupUi(this);
}

TestParaForm::~TestParaForm()
{
    delete ui;
}


void TestParaForm::setTestPara(stParaInfo &stparainfo)
{
    ui->lbStartFre->setText(QString::number(stparainfo.iStartFre,10));
    ui->lbStopFre->setText(QString::number(stparainfo.iStopFre,10));
    ui->lbStepFre->setText(QString::number(stparainfo.iStepFre,10));
    ui->lbStepTime->setText(QString::number(stparainfo.iStepInterivalTime,10));
    ui->lbOptinalDC->setText(QString::number(stparainfo.iOptionalDC,10));
    ui->lbOptinalAC->setText(QString::number(stparainfo.iOptionalAC,10));
    ui->lbScanNumbers->setText(QString::number(stparainfo.iScanNumbers,10));
    ui->lbMutilTime->setText(QString::number(stparainfo.iMutilScanInterivalTime,10));

}

void TestParaForm::setTestResult(double res_amp, double res_fre)
{
//    ui->lbResAmp->setText(QString::number(res_amp,'f',4));
//    ui->lbResFre->setText(QString::number(res_fre,'f',4));
}
