#include "newpatientdlg.h"
#include "ui_newpatientdlg.h"
#include "fdatatbase.h"
#include "newsampledlg.h"
#include <QCalendarWidget>
#include <QMessageBox>

NewPatientDlg::NewPatientDlg(QWidget *parent) :
    QDialog(parent),
    m_bIsNewSample(false),
    ui(new Ui::NewPatientDlg)

{
    ui->setupUi(this);
    OnDateChanged();
    ui->deBirthday->setMaximumDate(QDate::currentDate());
    QObject::connect(ui->pbNewSample,SIGNAL(clicked()),this,SLOT(OnNewSample()));
    QObject::connect(ui->pbOK,SIGNAL(clicked()),this,SLOT(OnOKClicked()));
    QObject::connect(ui->pbCancel,SIGNAL(clicked()),this,SLOT(close()));
    //QObject::connect(ui->tbCalendar,SIGNAL(clicked()),this,SLOT(OnCalendarClicked()));
    QObject::connect(ui->deBirthday,SIGNAL(dateChanged(QDate)),this,SLOT(OnDateChanged()));
}

NewPatientDlg::~NewPatientDlg()
{
    delete ui;
}

void NewPatientDlg::OnNewSample()
{
    QString patient_id = ui->lePatientID->text();
    QString patient_name = ui->lePatientName->text();
    if(patient_id.isEmpty()||patient_name.isEmpty()){
        QMessageBox::critical(0,"提示信息",QObject::tr("病人ID和姓名不能为空！"));
        return;
    }
    QString sex = ui->cbSex->currentText();
    int iage = ui->leAge->text().toInt(0,10);
    float iweight = ui->leWeight->text().toFloat();
    float iheight = ui->leHeight->text().toFloat();
    QDate currentDate = ui->deBirthday->date();
    QString strdate = currentDate.toString("yyyy/MM/dd");
    if(add_patient_info(patient_id,patient_name,strdate,sex,iage,iweight,iheight,"")){
        m_bIsNewSample = true;
        accept();
    }
}

void NewPatientDlg::OnOKClicked()
{
    QString patient_id = ui->lePatientID->text();
    QString patient_name = ui->lePatientName->text();
    if(patient_id.isEmpty()||patient_name.isEmpty()){
        QMessageBox::critical(0,"提示信息",QObject::tr("病人ID和姓名不能为空！"));
        return;
    }
    QString sex = ui->cbSex->currentText();
    int iage = ui->leAge->text().toInt(0,10);
    float iweight = ui->leWeight->text().toFloat();
    float iheight = ui->leHeight->text().toFloat();
    QDate currentDate = ui->deBirthday->date();
    QString strdate = currentDate.toString("yyyy/MM/dd");
    if(add_patient_info(patient_id,patient_name,strdate,sex,iage,iweight,iheight,"")){
        accept();
    }
}

void NewPatientDlg::OnDateChanged()
{
    QDate birthday = ui->deBirthday->date();
    QDate currentDate = QDate::currentDate();

    int age = birthday.daysTo(currentDate)/365;

    ui->leAge->setText(QString::number(age,10));
}
