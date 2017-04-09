#ifndef OPTIMALDCBIASFORM_H
#define OPTIMALDCBIASFORM_H

#include <QWidget>
#include <QtSerialPort>
#include <qwt_plot_curve.h>
#include "signaldata.h"


namespace Ui {
class OptimalDCBiasForm;
}

class OptimalDCBiasForm : public QWidget
{
    Q_OBJECT

public:
    explicit OptimalDCBiasForm(QSerialPort* port,QWidget *parent = 0);
    ~OptimalDCBiasForm();
signals:
    void sendOptimalDCPara(int istartDC,int istopDC,int iStepDC);
public slots:
    void drawOptimalDCBiasCurve(QPointF);

private slots:
    void on_pbOptimalDCScan_clicked();


private:
    Ui::OptimalDCBiasForm *ui;
    QSerialPort* serialPort;
    SignalData *DCAmpResPoint;
    QwtPlotCurve* DCAmpCurves;

    int iStartDC;    //起始直流激励
    int iStopDC;      //终止的直流激励
    int iStepDC;      //步进的直流激励间隔

};

#endif // OPTIMALDCBIASFORM_H
