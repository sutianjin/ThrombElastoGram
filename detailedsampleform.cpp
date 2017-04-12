#include "detailedsampleform.h"
#include "ui_detailedsampleform.h"
#include "fdatatbase.h"

DetailedSampleForm::DetailedSampleForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DetailedSampleForm)
{
    ui->setupUi(this);
    ui->qwtTegPlot->setXaxis(0,500);
    ui->qwtTegPlot->setXButtonTitle(tr("时间[s]"));
    ui->qwtTegPlot->setYLeftTitle(tr("共振幅值变化量[V]"));
    m_qwtPhaseCurve.setPen(Qt::green,2.0,Qt::SolidLine);
    m_qwtPhaseCurve.setCurveAttribute(QwtPlotCurve::Fitted);
    m_qwtPhaseCurve.attach(ui->qwtTegPlot);


    curveFitter = new QwtSplineCurveFitter();
    curveFitter->setFitMode(QwtSplineCurveFitter::ParametricSpline);
    curveFitter->setSplineSize(50);

//    weedFitter = new QwtWeedingCurveFitter();
//    weedFitter->setTolerance(0.002);
//    weedFitter->setChunkSize(200);


    Arc_Teg_Time_Curve.setPen(Qt::red,2.0,Qt::SolidLine);
    Arc_Teg_Time_Curve.setCurveAttribute(QwtPlotCurve::Fitted);
    Arc_Teg_Time_Curve.setCurveFitter(curveFitter);

    Arc_Teg_Time_Curve.attach(ui->qwtTegPlot);

    Teg_Time_Curve.setPen(Qt::red,2.0,Qt::SolidLine);
    Teg_Time_Curve.setCurveAttribute(QwtPlotCurve::Fitted);
    Teg_Time_Curve.setCurveFitter(curveFitter);

   Teg_Time_Curve.attach(ui->qwtTegPlot);

    Teg_Fitter_Time_Curve.setPen(Qt::red,2.0,Qt::SolidLine);
    Teg_Fitter_Time_Curve.setCurveAttribute(QwtPlotCurve::Fitted);
    Teg_Fitter_Time_Curve.setCurveFitter(curveFitter);

//    Teg_Fitter_Time_Curve.attach(ui->qwtTegPlot);

    RvalueMarkers.setLineStyle(QwtPlotMarker::VLine);
    RvalueMarkers.setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    RvalueMarkers.setLinePen( QColor(255, 170, 0), 2, Qt::DashDotLine );
    RvalueMarkers.setLabel(tr("R值"));
    RvalueMarkers.attach( ui->qwtTegPlot );

    KvalueMarkers.setLineStyle( QwtPlotMarker::VLine );
    KvalueMarkers.setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    KvalueMarkers.setLinePen( QColor(85, 255, 127), 1.5, Qt::DashDotLine );
    KvalueMarkers.setLabel(tr("K值"));
//    KvalueMarkers.attach( ui->tegCurvePlot );

    MAvalueMarkers.setLineStyle(QwtPlotMarker::HLine);
    MAvalueMarkers.setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    MAvalueMarkers.setLinePen( QColor(170, 0, 127), 1.5, Qt::DashDotLine );
    MAvalueMarkers.setLabel(tr("MA值"));
    MAvalueMarkers.attach( ui->qwtTegPlot );

    AngleCruve.setStyle( QwtPlotCurve::Lines );
    AngleCruve.setPen( QPen(QColor(0, 85, 255),1.5,Qt::DashDotLine));
    AngleCruve.setRenderHint( QwtPlotItem::RenderAntialiased, true );
    AngleCruve.setPaintAttribute( QwtPlotCurve::FilterPoints, true );
    AngleCruve.attach( ui->qwtTegPlot );


//    QwtLegend *legend = new QwtLegend;
//    legend->setDefaultItemMode( QwtLegendData::Checkable );//图例可被点击
//    ui->qwtTegPlot->insertLegend( legend, QwtPlot::RightLegend );
//    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
//        SLOT( showItem( const QVariant &, bool ) ) );//点击图例操作
//    QwtPlotItemList items = ui->qwtTegPlot->itemList( QwtPlotItem::Rtti_PlotCurve );//获取画了多少条曲线,如果为获取其他形状，注意改变参数

//    for ( int i = 0; i < items.size(); i++ )
//    {
//        if ( i == 0 )
//        {
//            const QVariant itemInfo = ui->qwtTegPlot->itemToInfo( items[i] );
//            QwtLegendLabel *legendLabel =
//                qobject_cast<QwtLegendLabel *>( legend->legendWidget( itemInfo ) );
//            if ( legendLabel )
//                legendLabel->setChecked( true );//
//            items[i]->setVisible( true );
//        }
//        else
//        {
//            items[i]->setVisible( false );
//        }
//    }

    ui->qwtTegPlot->replot();
    ui->qwtTegPlot->setAutoReplot( true );//设置自动重画，相当于更新
}

DetailedSampleForm::~DetailedSampleForm()
{
    delete ui;
}


void DetailedSampleForm::SetCurveData(QVector<QPointF> phasepoint)
{
    m_qwtPhaseCurve.setSamples(phasepoint);
    ui->qwtTegPlot->replot();
}


void DetailedSampleForm::SetPatientName(QString name)
{
    ui->lbPatientName->setText(name);
}

void DetailedSampleForm::SetSampleType(QString type)
{
    ui->lbSampleType->setText(type);
}

void DetailedSampleForm::SetDetectionTime(QString time)
{
    ui->lbDetectionTime->setText(time);
}

void DetailedSampleForm::SetTegCurveData(QVector<QPointF> vecTegPoint)
{


    if(vecTegPoint.isEmpty()) return;
    QVector<QPointF> arcVecTegPoint;
    unsigned numOfPoint = vecTegPoint.size();
    bool baseFlag = true;
    bool RValueFlag = false;
    arcVecTegPoint.resize(numOfPoint);
    double baseValue=vecTegPoint.at(0).y();

    arcVecTegPoint[0].setX(vecTegPoint.at(0).x());
    arcVecTegPoint[0].setY(baseValue);
    double MaxValue = 0;
    if(numOfPoint<=5){
        double sum=0;
        for(unsigned n=0; n<numOfPoint; n++){
            sum += vecTegPoint.at(n).y();
        }
        baseValue = sum/numOfPoint;
        for(unsigned n=0; n<numOfPoint; n++)
            vecTegPoint[n].setY(baseValue);
        return;

    }
    if(numOfPoint>2){
        QPointF tempPoint(0,0);
        for(unsigned i = 5; i < numOfPoint-2; i++){
            if(baseFlag){
                if(vecTegPoint.at(i+1).y()>vecTegPoint.at(i).y()+0.008){

                    vecTegPoint[i+1].setY(vecTegPoint.at(i).y());
                }


                if(((vecTegPoint.at(i).y()-vecTegPoint.at(i+1).y()>0.003)&&
                        (vecTegPoint.at(i+1).y()-vecTegPoint.at(i+2).y()>0.002))&&
                             (((baseValue - vecTegPoint.at(i+2).y() >0.012))&&
                             (baseValue - vecTegPoint.at(i+1).y()> 0.012))
                        ){
                    baseFlag = false;
                    RValueFlag = true;
                }else{
                    if((vecTegPoint.at(i).y()-vecTegPoint.at(i+1).y()<=0.003)&&
                         (vecTegPoint.at(i+1).y()-vecTegPoint.at(i+2).y()<=0.003)){
                      //  baseValue = vecTegPoint.at(i+1).y();

                        for(unsigned n=0; n <= i+2; n++){
                            vecTegPoint[n].setY(baseValue);
                            arcVecTegPoint[n].setX( vecTegPoint.at(n).x());
                            arcVecTegPoint[n].setY(baseValue);

                        }
                        RValuePoint = vecTegPoint[i+1];

                        tempPoint = vecTegPoint[i-2];

                    }
                }
                 RvalueMarkers.setValue(RValuePoint);

            }

            if(RValueFlag){

                if(vecTegPoint.at(i+1).y()>vecTegPoint.at(i).y()+0.008){

                    vecTegPoint[i+1].setY(vecTegPoint.at(i).y());
                }
//                KvalueMarkers.attach(ui->qwtTegPlot);
//                int xmax = MaxOfDiff(vecTegPoint);
//                KValuePoint = vecTegPoint[xmax];
//                KvalueMarkers.setValue(KValuePoint);
//                qDebug()<<"K值"<<xmax<<KValuePoint.x();
                if(arcVecTegPoint[i].y()< baseValue+0.05){
                    KvalueMarkers.attach(ui->qwtTegPlot);
                    KValuePoint = vecTegPoint[i];
                    KvalueMarkers.setValue(KValuePoint);

                    QPointF startPoint(tempPoint.x(),tempPoint.y()-baseValue);
                    QPointF stopPoint(vecTegPoint[i+10].x(),baseValue - vecTegPoint[i+10].y());

                    QPointF lineParaPoint = linefunc(startPoint,stopPoint);
                    double xvalue[3] = {tempPoint.x(), vecTegPoint[i+10].x(),getlineX(lineParaPoint,0.3)};
                    double yvalue[3] = {tempPoint.y()-baseValue, baseValue - vecTegPoint[i+10].y(),0.3};

                    AngleCruve.setSamples(xvalue,yvalue,3);
                }

            }

            arcVecTegPoint[i+1].setX(vecTegPoint.at(i+1).x());
            arcVecTegPoint[i+1].setY(2*baseValue - vecTegPoint.at(i+1).y());

            if(i<300 && arcVecTegPoint[i].y() > MaxValue){
                MaxValue = arcVecTegPoint[i].y();
                MAValuePoint = vecTegPoint[i];
                MAValuePoint.setY(MAValuePoint.y()-baseValue);
                MAvalueMarkers.setValue(MAValuePoint);
            }



        }
        vecTegPoint[numOfPoint-1].setY(vecTegPoint.at(numOfPoint-2).y());

        arcVecTegPoint[numOfPoint-1].setX(vecTegPoint.at(numOfPoint-1).x());
        arcVecTegPoint[numOfPoint-1].setY(2*baseValue - vecTegPoint.at(numOfPoint-1).y());



        for(unsigned i=0;i<numOfPoint;i++){
            vecTegPoint[i].setY(vecTegPoint.at(i).y()-baseValue);
            arcVecTegPoint[i].setY( arcVecTegPoint.at(i).y()-baseValue);
        }

        Arc_Teg_Time_Curve.setSamples(arcVecTegPoint);
        Teg_Time_Curve.setSamples(vecTegPoint);
        ui->qwtTegPlot->replot();
    }

 }

void DetailedSampleForm::SetCurveDataView(int curvetype)
{
    if(curvetype==0){
        Arc_Teg_Time_Curve.hide();
        Teg_Time_Curve.hide();
        Teg_Fitter_Time_Curve.hide();
        RvalueMarkers.hide();
        KvalueMarkers.hide();
        MAvalueMarkers.hide();
        AngleCruve.hide();
       // ui->tegCurvePlot->detachItems(QwtPlotItem::Rtti_PlotCurve);
       // ui->tegCurvePlot->detachItems(QwtPlotItem::Rtti_PlotMarker);
        m_qwtPhaseCurve.show();
    }else{
        Arc_Teg_Time_Curve.show();
        Teg_Time_Curve.show();
        Teg_Fitter_Time_Curve.show();
        RvalueMarkers.show();
        KvalueMarkers.show();
        MAvalueMarkers.show();
        AngleCruve.show();
       // ui->tegCurvePlot->detachItems(QwtPlotItem::Rtti_PlotCurve);
       // ui->tegCurvePlot->detachItems(QwtPlotItem::Rtti_PlotMarker);
        m_qwtPhaseCurve.hide();
    }
    ui->qwtTegPlot->replot();
}

void DetailedSampleForm::SetTimelineLenght(int lenght)
{
    ui->qwtTegPlot->setXaxis(0,lenght);
}

void DetailedSampleForm::SetMergeCurveData(QStringList list)
{

    qDebug()<<list;
    int pre_curve_nums = m_listMergePoint.size();
    for(int ci = 0; ci < pre_curve_nums; ci++){

        delete list_Teg_Time_Merge_Curve.takeLast();
        delete list_Arc_Teg_Time_Curve.takeLast();

        m_listArcMergeePoint.removeLast();
        m_listMergePoint.removeLast();

    }
    ui->qwtTegPlot->replot();
    qDebug()<<list;
    int recordNum = list.size();

    if(recordNum<=0){
        return;
    }
    const char *colors[] =
    {
        "LightSalmon",
        "SteelBlue",
        "Yellow",
        "Fuchsia",
        "PaleGreen",
        "PaleTurquoise",
        "Cornsilk",
        "HotPink",
        "Peru",
        "Maroon"
    };

    for(int i = 0; i < recordNum; i++){
        QString strphase = get_teg_data_phase(list[i]);
        QString strtime = get_teg_data_time(list[i]);
        QStringList phase_str_list = strphase.split(",");
        QStringList time_str_list = strtime.split(",");
        
        int size = time_str_list.size();
        
         QVector<QPointF> phasePoint;
         for(int i=0; i<size-1; i++){ 
             QPointF phasetempPoint(time_str_list.at(i).toInt(),phase_str_list.at(i).toDouble());
             phasePoint.append(phasetempPoint);
         }
         SetTegMerge(phasePoint);
         list_Teg_Time_Merge_Curve.append(new QwtPlotCurve(list[i]+QString("下部")));
         list_Teg_Time_Merge_Curve.last()->setPen(colors[i],2.0,Qt::SolidLine);
         list_Teg_Time_Merge_Curve.last()->setCurveAttribute(QwtPlotCurve::Fitted);
         list_Teg_Time_Merge_Curve.last()->setCurveFitter(curveFitter);
         list_Teg_Time_Merge_Curve.last()->setSamples(m_listMergePoint.last());
         list_Teg_Time_Merge_Curve.last()->attach(ui->qwtTegPlot);

         list_Arc_Teg_Time_Curve.append(new QwtPlotCurve(list[i]+QString("上部")));
         list_Arc_Teg_Time_Curve.last()->setPen(colors[i],2.0,Qt::SolidLine);
         list_Arc_Teg_Time_Curve.last()->setCurveAttribute(QwtPlotCurve::Fitted);
         list_Arc_Teg_Time_Curve.last()->setCurveFitter(curveFitter);
         list_Arc_Teg_Time_Curve.last()->setSamples(m_listArcMergeePoint.last());
         list_Arc_Teg_Time_Curve.last()->attach(ui->qwtTegPlot);

         ui->qwtTegPlot->replot();

    }
}


void DetailedSampleForm::SetTegMerge(QVector<QPointF> vecTegPoint)
{
    if(vecTegPoint.isEmpty()) return;
    QVector<QPointF> arcVecTegPoint;
    unsigned numOfPoint = vecTegPoint.size();
    bool baseFlag = true;
    bool RValueFlag = false;
    arcVecTegPoint.resize(numOfPoint);
    double baseValue=vecTegPoint.at(0).y();

    arcVecTegPoint[0].setX(vecTegPoint.at(0).x());
    arcVecTegPoint[0].setY(baseValue);
    double MaxValue = 0;
    if(numOfPoint<=5){
        double sum=0;
        for(unsigned n=0; n<numOfPoint; n++){
            sum += vecTegPoint.at(n).y();
        }
        baseValue = sum/numOfPoint;
        for(unsigned n=0; n<numOfPoint; n++)
            vecTegPoint[n].setY(baseValue);
        return;

    }
    if(numOfPoint>2){
        for(unsigned i = 5; i < numOfPoint-2; i++){
            if(baseFlag){
                if(vecTegPoint.at(i+1).y()>vecTegPoint.at(i).y()+0.008){

                    vecTegPoint[i+1].setY(vecTegPoint.at(i).y());
                }


                if(((vecTegPoint.at(i).y()-vecTegPoint.at(i+1).y()>0.002)&&
                        (vecTegPoint.at(i+1).y()-vecTegPoint.at(i+2).y()>0.002))&&
                             (((baseValue - vecTegPoint.at(i+2).y() >0.008))&&
                             (baseValue - vecTegPoint.at(i+1).y()> 0.008))||
                             (((baseValue - vecTegPoint.at(i+2).y() >0.008))&&
                             (baseValue - vecTegPoint.at(i+1).y()> 0.008))
                        ){
                    baseFlag = false;
                    RValueFlag = true;
                }else{
                    if((vecTegPoint.at(i).y()-vecTegPoint.at(i+1).y()<=0.002)&&
                         (vecTegPoint.at(i+1).y()-vecTegPoint.at(i+2).y()<=0.002)){
                      //  baseValue = vecTegPoint.at(i+1).y();

                        for(unsigned n=0; n <= i+2; n++){
                            vecTegPoint[n].setY(baseValue);
                            arcVecTegPoint[n].setX( vecTegPoint.at(n).x());
                            arcVecTegPoint[n].setY(baseValue);

                        }
                     //   RValuePoint = vecTegPoint[i+1];

                    }
                }
                // RvalueMarkers.setValue(RValuePoint);

            }

            if(RValueFlag){

                if(vecTegPoint.at(i+1).y()>vecTegPoint.at(i).y()+0.008){

                    vecTegPoint[i+1].setY(vecTegPoint.at(i).y());
                }



                if(arcVecTegPoint[i].y()< baseValue+0.05){
                 //   KvalueMarkers.attach(ui->qwtTegPlot);
                  //  KValuePoint = vecTegPoint[i];
                  //  KvalueMarkers.setValue(KValuePoint);
                }

            }

            arcVecTegPoint[i+1].setX(vecTegPoint.at(i+1).x());
            arcVecTegPoint[i+1].setY(2*baseValue - vecTegPoint.at(i+1).y());

            if(arcVecTegPoint[i].y() > MaxValue){
                MaxValue = arcVecTegPoint[i].y();
                MAValuePoint = vecTegPoint[i];
                MAValuePoint.setY(MAValuePoint.y()-baseValue);
                MAvalueMarkers.setValue(MAValuePoint);
            }



        }
        vecTegPoint[numOfPoint-1].setY(vecTegPoint.at(numOfPoint-2).y());

        arcVecTegPoint[numOfPoint-1].setX(vecTegPoint.at(numOfPoint-1).x());
        arcVecTegPoint[numOfPoint-1].setY(2*baseValue - vecTegPoint.at(numOfPoint-1).y());



        for(unsigned i=0;i<numOfPoint;i++){
            vecTegPoint[i].setY(vecTegPoint.at(i).y()-baseValue);
            arcVecTegPoint[i].setY( arcVecTegPoint.at(i).y()-baseValue);
        }


      m_listMergePoint.append(vecTegPoint);
      m_listArcMergeePoint.append(arcVecTegPoint);

    }
}


void DetailedSampleForm::showItem(const QVariant &itemInfo, bool on)
{
    QwtPlotItem *plotItem = ui->qwtTegPlot->infoToItem( itemInfo );
    if ( plotItem )
        plotItem->setVisible( on );
}


int DetailedSampleForm::MaxOfDiff(QVector<QPointF> phasepoint)
{
    QVector<double> diff;

    int size = phasepoint.size();

    for(int i = 0; i < size-1; i++){
        diff.append(phasepoint[i].y()-phasepoint[i+1].y());
    }
    double ymax =0;
    int xmax =0;
    for(int i = 0; i <size-1;i++){
        if(diff[i]>ymax){
            xmax =i;
            ymax = diff[i];
        }
    }

    return xmax;

}

QPointF DetailedSampleForm::linefunc(QPointF point1, QPointF point2)
{
    QPointF linepara;
    linepara.setX((point1.ry()-point2.ry())/(point1.rx()-point2.rx()));
    linepara.setY(point1.ry()-(linepara.rx()*point1.rx()));
    return linepara;
}


double DetailedSampleForm::getlineX(QPointF lineSlope, double yValue)
{
    double  xValue;
    xValue = (yValue-lineSlope.ry())/lineSlope.rx();
    return xValue;
}
