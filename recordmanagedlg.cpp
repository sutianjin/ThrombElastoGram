#include "recordmanagedlg.h"
#include "ui_recordmanagedlg.h"

RecordManageDlg::RecordManageDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordManageDlg)
{
    ui->setupUi(this);
}

RecordManageDlg::~RecordManageDlg()
{
    delete ui;
}
