#ifndef SELECTMERGEDLG_H
#define SELECTMERGEDLG_H

#include <QDialog>
#include "QStringList"
#include "fdatatbase.h"

namespace Ui {
class SelectMergeDlg;
}

class SelectMergeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SelectMergeDlg(QWidget *parent = 0);
    ~SelectMergeDlg();
    
    QStringList GetMergeString();

private slots:
    void on_tbadd_clicked();

    void on_tbdelete_clicked();

private:
    Ui::SelectMergeDlg *ui;
};

#endif // SELECTMERGEDLG_H
