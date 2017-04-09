#include "sampletypesdlg.h"
#include "ui_sampletypesdlg.h"

SampleTypesDlg::SampleTypesDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SampleTypesDlg)
{
    ui->setupUi(this);
}

SampleTypesDlg::~SampleTypesDlg()
{
    delete ui;
}
