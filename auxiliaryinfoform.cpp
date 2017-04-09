#include "auxiliaryinfoform.h"
#include "ui_auxiliaryinfoform.h"

AuxiliaryInfoForm::AuxiliaryInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuxiliaryInfoForm)
{
    ui->setupUi(this);
}

AuxiliaryInfoForm::~AuxiliaryInfoForm()
{
    delete ui;
}


void AuxiliaryInfoForm::SetTestSampleID(QString strTestSampleID)
{
    ui->lbSampleID->setText(strTestSampleID);
}


void AuxiliaryInfoForm::SetPatientName(QString strPatientName)
{
    ui->cbPatientName->addItem(strPatientName);
    ui->cbPatientName->setCurrentText(strPatientName);
}
