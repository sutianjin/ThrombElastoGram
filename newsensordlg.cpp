#include "newsensordlg.h"
#include "ui_newsensordlg.h"

NewSensorDlg::NewSensorDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSensorDlg)
{
    ui->setupUi(this);
}

NewSensorDlg::~NewSensorDlg()
{
    delete ui;
}
