#ifndef NEWSAMPLEDLG_H
#define NEWSAMPLEDLG_H

#include <QDialog>

namespace Ui {
class NewSampleDlg;
}

class NewSampleDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewSampleDlg(QWidget *parent = 0);
    ~NewSampleDlg();
    QString GetTestCaseID();
    QString GetPatientID();
    QString GetSampleType();
    QString GetTestDate();
    QString GetTestTime();
    QString GetInstrumentID();
    QString GetOperaterID();

    void AddPatientName(QString name);

private slots:
    void on_pbContinue_clicked();

private:
    Ui::NewSampleDlg *ui;

};

#endif // NEWSAMPLEDLG_H
