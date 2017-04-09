#ifndef ADDSAMPLETYPEDLG_H
#define ADDSAMPLETYPEDLG_H

#include <QDialog>

namespace Ui {
class AddSampleTypeDlg;
}

class AddSampleTypeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddSampleTypeDlg(QWidget *parent = 0);
    ~AddSampleTypeDlg();

private slots:
    void onSureToAddSampleClicked();

private:
    Ui::AddSampleTypeDlg *ui;

};

#endif // ADDSAMPLETYPEDLG_H
