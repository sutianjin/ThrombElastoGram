#ifndef HDBEXCHANGEDLG_H
#define HDBEXCHANGEDLG_H

#include <QDialog>
#include "fdatatbase.h"

namespace Ui {
class HDBExchangeDlg;
}

class HDBExchangeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit HDBExchangeDlg(QWidget *parent = 0);
    ~HDBExchangeDlg();

private slots:
    void on_tbadd_clicked();

    void on_tbdelete_clicked();

private:
    Ui::HDBExchangeDlg *ui;

    bool OpenDatabase(const QString &dbname);
};

#endif // HDBEXCHANGEDLG_H
