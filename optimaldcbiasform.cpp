#include "optimaldcbiasform.h"
#include "ui_optimaldcbiasform.h"

OptimalDCBiasForm::OptimalDCBiasForm(QSerialPort* port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptimalDCBiasForm)
{
    ui->setupUi(this);
    serialPort = port;
   // ui->DCVPlot->enableAxis(QwtPlot::xBottom,false);



    DCAmpCurves = new QwtPlotCurve();
    DCAmpResPoint = new SignalData();
    DCAmpCurves->setSamples(DCAmpResPoint);
    DCAmpCurves->setPen(Qt::red,1.0,Qt::SolidLine);
    DCAmpCurves->attach(ui->DCVPlot);
}

OptimalDCBiasForm::~OptimalDCBiasForm()
{
    delete ui;
}

void OptimalDCBiasForm::on_pbOptimalDCScan_clicked()
{
    DCAmpResPoint->clear();
    ui->DCVPlot->replot();

    iStartDC = ui->leStartDC->displayText().toInt(0,10);
    iStopDC = ui->leStopDC->displayText().toInt(0,10);
    iStepDC = ui->leStepDC->displayText().toInt(0,10);

    ui->DCVPlot->setXaxis(iStartDC,iStopDC);
    emit sendOptimalDCPara(iStartDC,iStopDC,iStepDC);
}

void OptimalDCBiasForm::drawOptimalDCBiasCurve(QPointF DCAmpPoint)
{
    DCAmpResPoint->append(DCAmpPoint);
    ui->DCVPlot->replot();
}
