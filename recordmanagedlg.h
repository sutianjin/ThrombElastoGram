#ifndef RECORDMANAGEDLG_H
#define RECORDMANAGEDLG_H

#include <QDialog>

namespace Ui {
class RecordManageDlg;
}

class RecordManageDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RecordManageDlg(QWidget *parent = 0);
    ~RecordManageDlg();

private:
    Ui::RecordManageDlg *ui;
};

#endif // RECORDMANAGEDLG_H
