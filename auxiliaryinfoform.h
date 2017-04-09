#ifndef AUXILIARYINFOFORM_H
#define AUXILIARYINFOFORM_H

#include <QWidget>

namespace Ui {
class AuxiliaryInfoForm;
}

class AuxiliaryInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit AuxiliaryInfoForm(QWidget *parent = 0);
    ~AuxiliaryInfoForm();

    void SetTestSampleID(QString strTestSampleID);
    void SetPatientName(QString strPatientName);

private:
    Ui::AuxiliaryInfoForm *ui;
};

#endif // AUXILIARYINFOFORM_H
