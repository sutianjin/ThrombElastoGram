#ifndef NEWSENSORDLG_H
#define NEWSENSORDLG_H

#include <QDialog>

namespace Ui {
class NewSensorDlg;
}

class NewSensorDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewSensorDlg(QWidget *parent = 0);
    ~NewSensorDlg();

private:
    Ui::NewSensorDlg *ui;
};

#endif // NEWSENSORDLG_H
