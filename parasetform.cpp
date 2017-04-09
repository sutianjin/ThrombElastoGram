#include "parasetform.h"
#include "ui_parasetform.h"

ParaSetForm::ParaSetForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParaSetForm)
{
    ui->setupUi(this);
}

ParaSetForm::~ParaSetForm()
{
    delete ui;
}

int ParaSetForm::getStartFre()
{
    iStartFre = ui->StartFreqlineEdit->displayText().toInt(0,10);
    return iStartFre;
}

int ParaSetForm::getStopFre()
{
    iStopFre = ui->endFreqlineEdit->displayText().toInt(0,10);
    return iStopFre;
}

int ParaSetForm::getStepFre()
{
    iStepFre = ui->StepFreqlineEdit->displayText().toInt(0,10);
    return iStepFre;
}

int ParaSetForm::getScanInterival()
{
    iScanInterival = ui->oneStepTimelineEdit->displayText().toInt(0,10);
    return iScanInterival;
}

int ParaSetForm::getScanNum()
{
    iScanNum = ui->scanNumlineEdit->displayText().toInt(0,10);
    return iScanNum;
}

int ParaSetForm::getMutilScanInterival()
{
    iMutilScanInterival = ui->multiScanlineEdit->displayText().toInt(0,10);
    return iMutilScanInterival;
}

int ParaSetForm::getOptimalAC()
{
    iOptimalAC = ui->leOptimalAC->displayText().toInt(0,10);
    return iOptimalAC;
}

int ParaSetForm::getOptimalDC()
{
    iOptimalDC = ui->leOptimalDC->displayText().toInt(0,10);
    return iOptimalDC;
}
