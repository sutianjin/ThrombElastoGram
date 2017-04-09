#ifndef OPTIMALACBIASFORM_H
#define OPTIMALACBIASFORM_H

#include <QWidget>
#include <QtSerialPort>
#include <qwt_plot_curve.h>
#include "signaldata.h"

namespace Ui {
class OptimalACBiasForm;
}

class OptimalACBiasForm : public QWidget
{
    Q_OBJECT

public:
    explicit OptimalACBiasForm(QSerialPort* port, QWidget *parent = 0);
    ~OptimalACBiasForm();

public slots:
    void drawOptimalACBiasCurve(QPointF);

signals:
    void sendOptimalACPara(int istartAC,int istopAC,int iStepAC);

private slots:
    void on_pbOptimalAC_clicked();


private:
    Ui::OptimalACBiasForm *ui;

    QSerialPort* serialPort;
    SignalData *ACAmpResPoint;
    QwtPlotCurve* ACAmpCurves;

    int iStartAC;    //起始直流激励
    int iStopAC;      //终止的直流激励
    int iStepAC;      //步进的直流激励间隔
};

#endif // OPTIMALACBIASFORM_H
