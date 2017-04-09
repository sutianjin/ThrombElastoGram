#ifndef TESTPARAFORM_H
#define TESTPARAFORM_H

#include <QWidget>
#include "utils.h"

namespace Ui {
class TestParaForm;
}

class TestParaForm : public QWidget
{
    Q_OBJECT

public:
    explicit TestParaForm(QWidget *parent = 0);
    ~TestParaForm();
    void setTestPara(stParaInfo &stparainfo);
    void setTestResult(double res_amp,double res_fre);

private:
    Ui::TestParaForm *ui;
};

#endif // TESTPARAFORM_H
