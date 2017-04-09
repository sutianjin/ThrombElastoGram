#include "showlistcurveform.h"
#include "ui_showlistcurveform.h"
#include <qwt_symbol.h>

ShowListCurveForm::ShowListCurveForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowListCurveForm)
{
    ui->setupUi(this);
    AmpResCurve = new QwtPlotCurve();
    FreResCurve = new QwtPlotCurve();
    PhaseResCurve = new QwtPlotCurve();

    ui->listOriginCurve->setAxisTitle(QwtPlot::xBottom,"频率[kHz]");
    ui->listOriginCurve->setAxisTitle(QwtPlot::yLeft,"幅值[V]/相位");

    ui->FTPlot->setAxisTitle(QwtPlot::xBottom,"Times[S]");
    ui->FTPlot->setAxisTitle(QwtPlot::yLeft,"共振频率[kHz]");

    ui->VTPlot->setAxisTitle(QwtPlot::xBottom,"Times[S]");
    ui->VTPlot->setAxisTitle(QwtPlot::yLeft,"共振幅值[V]");

    ui->phasetimePlot->setAxisTitle(QwtPlot::xBottom,"Times[S]");
    ui->phasetimePlot->setAxisTitle(QwtPlot::yLeft,"共振相位");
}

ShowListCurveForm::~ShowListCurveForm()
{
    delete ui;
}

void ShowListCurveForm::getOriginData(QList<QVector<QPointF> > *listAmpPoint,
                                      QList<QVector<QPointF> > *listPhasePoint)
{
   // _listAmpPoint = listAmpPoint;
  //  _listPhasePoint = listPhasePoint;
    if(!_listAmpPoint.isEmpty()){
        unsigned numOfScan = _listAmpPoint.size();
        for(unsigned ci = 0;ci < numOfScan; ci++){
            delete listShowAmpCurves.takeLast();
            delete listShowPhaseCurves.takeLast();
            delete listAmpResHighMarkers.takeLast();
            delete listPhaseResMarkers.takeLast();
            _listAmpPoint.removeLast();
            _listPhasePoint.removeLast();
            AmpResPoint.removeLast();
            PhaseResPoint.removeLast();
        }
        ui->listOriginCurve->replot();

    }
    if(!AmpAndTimePoint.isEmpty()){
        AmpAndTimePoint.clear();

    }

    if(!FreAndTimePoint.isEmpty()){
        FreAndTimePoint.clear();
    }

    if(!PhaseAndTimePoint.isEmpty()){
        PhaseAndTimePoint.clear();
    }

    if(listAmpPoint->isEmpty()){
        return;
    }

    unsigned numOfScan = listAmpPoint->size();
    unsigned numOfSample = listAmpPoint->at(0).size();


    for(unsigned ci = 0;ci < numOfScan; ci++){
        _listAmpPoint.append(QVector<QPointF>(numOfSample));
        _listPhasePoint.append(QVector<QPointF>(numOfSample));
        double maxAmpRes = 0;
        double maxPhaseRes = 0;
        QPointF amptempPoint;
        QPointF phasetempPoint;
        for(unsigned i = 0; i < numOfSample; i++){

            _listAmpPoint[ci][i] = listAmpPoint->at(ci).at(i);
            _listPhasePoint[ci][i] = listPhasePoint->at(ci).at(i);
            if(_listAmpPoint[ci][i].y() > maxAmpRes){
                maxAmpRes = _listAmpPoint[ci][i].y();
                amptempPoint = _listAmpPoint[ci][i];
            }

            if(_listPhasePoint[ci][i].y() > maxPhaseRes){
                maxPhaseRes = _listPhasePoint[ci][i].y();
                phasetempPoint = _listPhasePoint[ci][i];
            }
        }
        ui->listOriginCurve->setXaxis(_listAmpPoint.at(0).at(0).x(),_listAmpPoint.at(0).at(numOfSample-1).x());

        listShowAmpCurves.append(new QwtPlotCurve());
        listShowAmpCurves.last()->setSamples(_listAmpPoint.at(ci));
        listShowAmpCurves.last()->setPen(Qt::red,1.0,Qt::SolidLine);
         listShowAmpCurves.last()->setCurveAttribute(QwtPlotCurve::Fitted);
        listShowAmpCurves.last()->attach(ui->listOriginCurve);


        listShowPhaseCurves.append(new QwtPlotCurve());
        listShowPhaseCurves.last()->setSamples(_listPhasePoint.at(ci));
        listShowPhaseCurves.last()->setPen(Qt::green,1.0,Qt::SolidLine);
        listShowAmpCurves.last()->setCurveAttribute(QwtPlotCurve::Fitted);
        listShowPhaseCurves.last()->attach(ui->listOriginCurve);

        listAmpResHighMarkers.append(new QwtPlotMarker());
        listAmpResHighMarkers.last()->setValue(amptempPoint);
        listAmpResHighMarkers.last()->setSymbol(new QwtSymbol( QwtSymbol::Cross, Qt::NoBrush,
                                                               QPen( Qt::black ), QSize( 10, 10 ) ));
        listAmpResHighMarkers.last()->attach(ui->listOriginCurve);

        listPhaseResMarkers.append(new QwtPlotMarker());
        listPhaseResMarkers.last()->setValue(phasetempPoint);
        listPhaseResMarkers.last()->setSymbol(new QwtSymbol( QwtSymbol::Cross, Qt::NoBrush,
                                                               QPen( Qt::white ), QSize( 10, 10 ) ));
        listPhaseResMarkers.last()->attach(ui->listOriginCurve);
        ui->listOriginCurve->replot();

        AmpResPoint.append(amptempPoint);
        PhaseResPoint.append(phasetempPoint);


    }

    if(numOfScan<10) return;
    for(unsigned ci = 0;ci < numOfScan-5; ci=ci+5){


        double Ampaverage=0;
         double PhaseAverage=0;
         double FreAverage=0;
        for(unsigned i = ci; i < ci+5; i++){
            FreAverage += AmpResPoint.at(i).x();
            Ampaverage += AmpResPoint.at(i).y();
            PhaseAverage += PhaseResPoint.at(i).y();
        }


        QPointF Fretemp((((iStopFre-iStartFre)/iInterivalFre)*iScanInterival*(ci+2)+iMutilScanInterival*(ci+2))/1000,FreAverage/5);
        FreAndTimePoint.append(Fretemp);
        double x = (((iStopFre-iStartFre)/iInterivalFre)*iScanInterival*(ci+2)+iMutilScanInterival*(ci+2))/1000;

       // double phase_x = (((paraList.at(1).toInt()-paraList.at(0).toInt())/paraList.at(2).toInt())*paraList.at(3).toInt()*(ci+2)+paraList.at(7).toInt()*(ci+2))/1000;
        QPointF Amptemp(x,Ampaverage/5);
        AmpAndTimePoint.append(Amptemp);
         QPointF Phasetemp((((iStopFre-iStartFre)/iInterivalFre)*iScanInterival*(ci+2)+iMutilScanInterival*(ci+2))/1000,PhaseAverage/5);
       // QPointF Phasetemp((((PhaseResPoint.at(ci+2).x()*1000-iStartFre)/iInterivalFre)*iScanInterival+iMutilScanInterival*(ci+2))/1000,PhaseAverage/5);
        PhaseAndTimePoint.append(Phasetemp);

       /* double x = (((AmpResPoint.at(ci).x()*1000-iStartFre)/iInterivalFre)*iScanInterival+iMutilScanInterival*ci)/1000;
        double y =  AmpResPoint.at(ci).y();
        QPointF Amptemp(x,y);
        AmpAndTimePoint.append(Amptemp);

        QPointF Fretemp((((AmpResPoint.at(ci).x()*1000-iStartFre)/iInterivalFre)*iScanInterival+iMutilScanInterival*ci)/1000,AmpResPoint.at(ci).x());
        FreAndTimePoint.append(Fretemp);

        QPointF Phasetemp((((PhaseResPoint.at(ci).x()*1000-iStartFre)/iInterivalFre)*iScanInterival+iMutilScanInterival*ci)/1000,PhaseResPoint.at(ci).y());
        PhaseAndTimePoint.append(Phasetemp);*/
    }
    ui->FTPlot->setXaxis(0,
                         (((iStopFre-iStartFre)/iInterivalFre)*iScanInterival*(numOfScan)+iMutilScanInterival*(numOfScan-1))/1000);

    ui->VTPlot->setXaxis(0,
                         (((iStopFre-iStartFre)/iInterivalFre)*iScanInterival*(numOfScan)+iMutilScanInterival*(numOfScan-1))/1000);
    ui->phasetimePlot->setXaxis(0,(((iStopFre-iStartFre)/iInterivalFre)*iScanInterival*(numOfScan)+iMutilScanInterival*(numOfScan-1))/1000);

    FreResCurve->setSamples(FreAndTimePoint);
    FreResCurve->setPen(Qt::red,1.0,Qt::SolidLine);
    FreResCurve->attach(ui->FTPlot);
    FreResCurve->setCurveAttribute(QwtPlotCurve::Fitted);
    ui->FTPlot->replot();

    AmpResCurve->setSamples(AmpAndTimePoint);
    AmpResCurve->setPen(Qt::red,1.0,Qt::SolidLine);
    AmpResCurve->attach(ui->VTPlot);
    AmpResCurve->setCurveAttribute(QwtPlotCurve::Fitted);
    ui->VTPlot->replot();

    PhaseResCurve->setSamples(PhaseAndTimePoint);
    PhaseResCurve->setPen(Qt::green,1.0,Qt::SolidLine);
    PhaseResCurve->attach(ui->phasetimePlot);
    PhaseResCurve->setCurveAttribute(QwtPlotCurve::Fitted);
    ui->phasetimePlot->replot();

    ui->listOriginCurve->replot();
}

void ShowListCurveForm::getDetectPara(QStringList listPara)
{
    _listPara = listPara;
    iStartFre = listPara.at(0).toInt();
    iStopFre =  listPara.at(1).toInt();
    iInterivalFre =  listPara.at(2).toInt();
    iScanInterival =  listPara.at(3).toInt();
    iDCData =  listPara.at(4).toInt();
    iACData =  listPara.at(5).toInt();
    iScanNum =  listPara.at(6).toInt();
    iMutilScanInterival =  listPara.at(7).toInt();
}

void ShowListCurveForm::showClotingCurve()
{
    unsigned int Phase_Time_Size = PhaseAndTimePoint.size();
    if(Phase_Time_Size <= 0)
    {
        return;
    }

}
