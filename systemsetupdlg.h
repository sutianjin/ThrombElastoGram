#ifndef SYSTEMSETUPDLG_H
#define SYSTEMSETUPDLG_H

#include <QDialog>
#include <QSqlTableModel>

#include "fdatatbase.h"

namespace Ui {
class SystemSetupDlg;
}

class SystemSetupDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SystemSetupDlg(QWidget *parent = 0);
    ~SystemSetupDlg();

private slots:
    void onAddSampleTypeClicked();

private:
    Ui::SystemSetupDlg *ui;
    QSqlTableModel *m_pSampleModel;

    user_data currentUserData;
};

#endif // SYSTEMSETUPDLG_H
