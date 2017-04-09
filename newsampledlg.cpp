#include "newsampledlg.h"
#include "ui_newsampledlg.h"
#include <QDateTime>
#include <QMessageBox>
#include "fdatatbase.h"

NewSampleDlg::NewSampleDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSampleDlg)
{
    ui->setupUi(this);

    QDateTime datetime = QDateTime::currentDateTime();
    QString strDate = datetime.toString("yyyy-MM-dd");
    ui->leTestDate->setText(strDate);

    QString strTime = datetime.toString("HH:mm:ss AP");
    ui->leTestTime->setText(strTime);

    QStringList patientList = get_all_patient_id();
    ui->cbPatient->addItems(patientList);

    QStringList sampletypeList = get_all_sample_type_id();
    ui->cbSampleType->addItems(sampletypeList);

    user_data current_userdata = get_user_data();
    ui->cbOperatorID->addItem(current_userdata.userName);
}

NewSampleDlg::~NewSampleDlg()
{
    delete ui;
}

QString NewSampleDlg::GetTestCaseID()
{
    return ui->leTestSampleID->displayText();
}


QString NewSampleDlg::GetPatientID()
{
    return ui->cbPatient->currentText();
}

QString NewSampleDlg::GetSampleType()
{
    return ui->cbSampleType->currentText();
}

QString NewSampleDlg::GetTestDate()
{

    return ui->leTestDate->displayText();
}

QString NewSampleDlg::GetTestTime()
{

    return ui->leTestTime->displayText();
}

QString NewSampleDlg::GetInstrumentID()
{
    return ui->cbInstrumentID->currentText();
}

QString NewSampleDlg::GetOperaterID()
{
    return ui->cbOperatorID->currentText();
}

void NewSampleDlg::on_pbContinue_clicked()
{
    QString testCaseID = GetTestCaseID();
    if(testCaseID.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("请输入待测样本的ID号");
        msgBox.exec();
        return;
    }
    QString patientID = GetPatientID();
    QString sampleType = GetSampleType();
    QString testDate = GetTestDate();
    QString testTime = GetTestTime();
    QString instrumentID = GetInstrumentID();
    QString operaterID = GetOperaterID();

    if(add_test_case_info(testCaseID,operaterID,patientID,sampleType,"",instrumentID,testDate,testTime)){
        if(add_teg_data_info(testCaseID)&&add_test_para_info(testCaseID)&&add_origin_data_info(testCaseID))
            accept();
    }else{
        QMessageBox msgBox;
        msgBox.setText("输入信息有误，或者该样本已经检测");
        msgBox.exec();
        return;
    }

}

void NewSampleDlg::AddPatientName(QString name)
{
    ui->cbPatient->addItem(name);
}
