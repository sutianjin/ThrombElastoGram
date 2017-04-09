#ifndef SAMPLETYPESDLG_H
#define SAMPLETYPESDLG_H

#include <QDialog>

namespace Ui {
class SampleTypesDlg;
}

class SampleTypesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SampleTypesDlg(QWidget *parent = 0);
    ~SampleTypesDlg();

private:
    Ui::SampleTypesDlg *ui;
};

#endif // SAMPLETYPESDLG_H
