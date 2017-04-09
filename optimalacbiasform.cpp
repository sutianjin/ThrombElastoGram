#include "optimalacbiasform.h"
#include "ui_optimalacbiasform.h"

OptimalACBiasForm::OptimalACBiasForm(QSerialPort* port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptimalACBiasForm)
{
    ui->setupUi(this);
    serialPort = port;
  //  ui->ACVPlot->enableAxis(QwtPlot::xBottom,false);
  //  ui->ACVPlot->enableAxis(QwtPlot::yLeft,false);

    ACAmpCurves = new QwtPlotCurve();
    ACAmpResPoint = new SignalData();
    ACAmpCurves->setSamples(ACAmpResPoint);
    ACAmpCurves->setPen(Qt::red,1.0,Qt::SolidLine);
    ACAmpCurves->attach(ui->ACVPlot);
}

OptimalACBiasForm::~OptimalACBiasForm()
{
    delete ui;
}

void OptimalACBiasForm::on_pbOptimalAC_clicked()
{
    ACAmpResPoint->clear();
    ui->ACVPlot->replot();
    iStartAC = ui->leStartAC->displayText().toInt(0,10);
    iStopAC = ui->leStopAC->displayText().toInt(0,10);
    iStepAC = ui->leStepAC->displayText().toInt(0,10);
    ui->ACVPlot->setXaxis(iStartAC,iStopAC);
    emit sendOptimalACPara(iStartAC,iStopAC,iStepAC);
}

void OptimalACBiasForm::drawOptimalACBiasCurve(QPointF ACAmpPoint)
{
    ACAmpResPoint->append(ACAmpPoint);
    ui->ACVPlot->replot();
}

