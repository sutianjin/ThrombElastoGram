#ifndef NEWPATIENTDLG_H
#define NEWPATIENTDLG_H

#include <QDialog>

namespace Ui {
class NewPatientDlg;
}

class NewPatientDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewPatientDlg(QWidget *parent = 0);
    ~NewPatientDlg();
    bool m_bIsNewSample;
private slots:
    void OnOKClicked();
    void OnNewSample();
    void OnDateChanged();


private:
    Ui::NewPatientDlg *ui;
};

#endif // NEWPATIENTDLG_H
