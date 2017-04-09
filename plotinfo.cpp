#include "plotinfo.h"
#include "ui_plotinfo.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "fdatatbase.h"

PlotInfo::PlotInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotInfo),
    m_timeline(1200)
{
    ui->setupUi(this);
    ui->tegCurvePlot->setXaxis(0,m_timeline);
    m_qwtPhaseCurve.setPen(Qt::red,1.0,Qt::SolidLine);
    m_qwtPhaseCurve.attach(ui->tegCurvePlot);
    actionViewOriginData = new QAction("查看原始数据",this);
    actionExportOriginData = new QAction("导出原始数据",this);
    actionSetTimeline = new QAction("改变坐标轴",this);
    actionSetAmplitudeRange = new QAction("设置Y轴范围",this);
    actionExportPhaseTimeData = new QAction("导出共振数据",this);
    addAction(actionViewOriginData);
    addAction(actionExportOriginData);
    addAction(actionExportPhaseTimeData);
    addAction(actionSetTimeline);
    addAction(actionSetAmplitudeRange);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QObject::connect(actionViewOriginData,SIGNAL(triggered()),this,SLOT(emitViewOriginDataSignal()));
    QObject::connect(actionSetTimeline,SIGNAL(triggered()),this,SLOT(onSetTimelineTriggered()));
    QObject::connect(actionExportOriginData,SIGNAL(triggered()),this,SLOT(onExportOriginDataTriggered()));
    QObject::connect(actionExportPhaseTimeData,SIGNAL(triggered(bool)),this,SLOT(onExportPhaseTimeDataTriggered()));
    QObject::connect(actionSetAmplitudeRange,SIGNAL(triggered(bool)),this,SLOT(onSetAmplitudeRangeTriggered()));

    Arc_Teg_Time_Curve.setPen(Qt::green,2.0,Qt::SolidLine);
    Arc_Teg_Time_Curve.attach(ui->tegCurvePlot);

    Teg_Time_Curve.setPen(Qt::green,2.0,Qt::SolidLine);
    Teg_Time_Curve.attach(ui->tegCurvePlot);

    RvalueMarkers.setLineStyle(QwtPlotMarker::VLine);
    RvalueMarkers.setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    RvalueMarkers.setLinePen( QColor(255, 170, 0), 2, Qt::DashDotLine );
    RvalueMarkers.setLabel(tr("R值"));
    RvalueMarkers.attach( ui->tegCurvePlot );

    KvalueMarkers.setLineStyle( QwtPlotMarker::VLine );
    KvalueMarkers.setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    KvalueMarkers.setLinePen( QColor(85, 255, 127), 1.5, Qt::DashDotLine );
    KvalueMarkers.setLabel(tr("K值"));
   // KvalueMarkers.attach( ui->tegCurvePlot );

    MAvalueMarkers.setLineStyle(QwtPlotMarker::HLine);
    MAvalueMarkers.setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    MAvalueMarkers.setLinePen( QColor(170, 0, 127), 1.5, Qt::DashDotLine );
    MAvalueMarkers.setLabel(tr("MA值"));
    MAvalueMarkers.attach( ui->tegCurvePlot );

    AngleCruve.setStyle( QwtPlotCurve::Lines );
    AngleCruve.setPen( QPen(QColor(0, 85, 255),1.5,Qt::DashDotLine));
    AngleCruve.setRenderHint( QwtPlotItem::RenderAntialiased, true );
    AngleCruve.setPaintAttribute( QwtPlotCurve::FilterPoints, true );
    AngleCruve.attach( ui->tegCurvePlot );
}

PlotInfo::~PlotInfo()
{
    delete ui;
    delete actionExportOriginData;
    delete actionExportPhaseTimeData;
    delete actionSetTimeline;
    delete actionViewOriginData;
    delete actionSetAmplitudeRange;
}


void PlotInfo::SetTestSampleID(QString strTestSampleID)
{
    ui->lbTestSampleID->setText(strTestSampleID);
}


void PlotInfo::SetPatientName(QString strPatientName)
{
    ui->lbPatientName->setText(strPatientName);
}

void PlotInfo::SetStartTestTime(QString strStartTime)
{
    ui->lbStartToEndTime->setText(strStartTime);
}

void PlotInfo::SetEndTestTime(QString strEndTime)
{
    QString strtime = ui->lbStartToEndTime->text()+"--";
    ui->lbStartToEndTime->setText(strtime+strEndTime);
}

void PlotInfo::SetCurveData(QVector<QPointF> phaseResPoint)
{
    m_vecPhasePoint = phaseResPoint;
    m_qwtPhaseCurve.setSamples(phaseResPoint);
    ui->tegCurvePlot->replot();
}

QString PlotInfo::GetTestSampleID()
{
    return ui->lbTestSampleID->text();
}

QString PlotInfo::GetPatientName()
{
    return ui->lbPatientName->text();
}

QString PlotInfo::GetDetectionTime()
{
    return ui->lbStartToEndTime->text();
}


void PlotInfo::emitViewOriginDataSignal()
{
    emit viewOriginData(ui->lbTestSampleID->text());
}

void PlotInfo::SetTegCurveData(QVector<QPointF> phaseResPoint)
{

    if(phaseResPoint.isEmpty()) return;
  //  m_vecPhasePoint = phaseResPoint;
    m_VecTegPoint = phaseResPoint;
    unsigned numOfPoint = m_VecTegPoint.size();
    bool baseFlag = true;
    bool RValueFlag = false;
    m_ArcVecTegPoint.resize(numOfPoint);
    double baseValue=m_VecTegPoint.at(0).y();

    m_ArcVecTegPoint[0].setX(m_VecTegPoint.at(0).x());
    m_ArcVecTegPoint[0].setY(baseValue);
    double MaxValue = 0;

    if(numOfPoint<=5){
        double sum=0;
        for(unsigned n=0; n<numOfPoint;n++){
            sum +=  m_VecTegPoint.at(0).y();
        }
        baseValue = sum/numOfPoint;
        for(unsigned n=0; n<numOfPoint; n++)
            m_VecTegPoint[n].setY(baseValue);
        return;
    }

    if(numOfPoint>2){
        QPointF tempPoint(0,0);
        for(unsigned i = 5; i < numOfPoint-2; i++){
            if(baseFlag){

                if(m_VecTegPoint.at(i+1).y()>m_VecTegPoint.at(i).y()+0.008){

                    m_VecTegPoint[i+1].setY(m_VecTegPoint.at(i).y());
                }


                //判断
                if(((m_VecTegPoint.at(i).y()-m_VecTegPoint.at(i+1).y()>0.003)&&
                        (m_VecTegPoint.at(i+1).y()-m_VecTegPoint.at(i+2).y()>0.003))||
                        (((baseValue - m_VecTegPoint.at(i+2).y() >0.012))&&
                        (baseValue - m_VecTegPoint.at(i+1).y()> 0.012))){
                //    baseValue = m_VecTegPoint[i].y();
                    baseFlag = false;
                    RValueFlag = true;
                }else{
                    if((m_VecTegPoint.at(i).y()-m_VecTegPoint.at(i+1).y()<=0.003)&&
                         (m_VecTegPoint.at(i+1).y()-m_VecTegPoint.at(i+2).y()<=0.003)){
                      //  baseValue = m_VecTegPoint.at(i+1).y();

                        for(unsigned n=0; n <= i+2; n++){
                  //          baseValue = m_VecTegPoint[i].y();
                            m_VecTegPoint[n].setY(baseValue);
                            m_ArcVecTegPoint[n].setX( m_VecTegPoint.at(n).x());
                            m_ArcVecTegPoint[n].setY(baseValue);
                            RValuePoint = m_VecTegPoint[n];
                        }

                        RValuePoint = m_VecTegPoint[i+1];
                        baseValue = m_VecTegPoint[i].y();
                        tempPoint = m_VecTegPoint[i-2];
                    }
                }




                RvalueMarkers.setValue(RValuePoint);
            }

            if(RValueFlag){

                if(m_VecTegPoint.at(i+1).y()>m_VecTegPoint.at(i).y()+0.008){

                    m_VecTegPoint[i+1].setY(m_VecTegPoint.at(i).y());
                }

                if(m_ArcVecTegPoint[i].y()< baseValue+0.05){
                    KvalueMarkers.attach(ui->tegCurvePlot);
                    KValuePoint = m_VecTegPoint[i];
                    KvalueMarkers.setValue(KValuePoint);

                    QPointF startPoint(tempPoint.x(),tempPoint.y()-baseValue);
                    QPointF stopPoint(m_VecTegPoint[i+10].x(),baseValue - m_VecTegPoint[i+10].y());

                    QPointF lineParaPoint = linefunc(startPoint,stopPoint);
                    double xvalue[3] = {tempPoint.x(), m_VecTegPoint[i+10].x(),getlineX(lineParaPoint,0.3)};
                    double yvalue[3] = {tempPoint.y()-baseValue, baseValue - m_VecTegPoint[i+10].y(),0.3};

                    AngleCruve.setSamples(xvalue,yvalue,3);
                    double tanvalue = ((baseValue - m_VecTegPoint[i+10].y())*10)/(((m_VecTegPoint[i+10].x()-tempPoint.x()))/100);
                //    qDebug()<<tanvalue;
                    angleValue = (180*qAtan(tanvalue))/M_PI;

                }

            }

            m_ArcVecTegPoint[i+1].setX(m_VecTegPoint.at(i+1).x());
            m_ArcVecTegPoint[i+1].setY(2*baseValue - m_VecTegPoint.at(i+1).y());

            if(i<300 && m_ArcVecTegPoint[i].y() > MaxValue){
                MaxValue = m_ArcVecTegPoint[i].y();
                MAValuePoint = m_VecTegPoint[i];
                MAValuePoint.setY(MAValuePoint.y()-baseValue);
                MAvalueMarkers.setValue(MAValuePoint);
            }



        }
        m_VecTegPoint[numOfPoint-1].setY(m_VecTegPoint.at(numOfPoint-2).y());

        m_ArcVecTegPoint[numOfPoint-1].setX(m_VecTegPoint.at(numOfPoint-1).x());
        m_ArcVecTegPoint[numOfPoint-1].setY(2*baseValue - m_VecTegPoint.at(numOfPoint-1).y());

        for(unsigned i=0;i<numOfPoint;i++){
            m_VecTegPoint[i].setY(m_VecTegPoint.at(i).y()-baseValue);
            m_ArcVecTegPoint[i].setY(m_ArcVecTegPoint.at(i).y()-baseValue);
        }

        Arc_Teg_Time_Curve.setSamples(m_ArcVecTegPoint);
        Teg_Time_Curve.setSamples(m_VecTegPoint);
        ui->tegCurvePlot->replot();

    }


}

void PlotInfo::SetCurveDataView(int curvetype)
{
    if(curvetype==0){
        Arc_Teg_Time_Curve.hide();
        Teg_Time_Curve.hide();
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
        RvalueMarkers.show();
        KvalueMarkers.show();
        MAvalueMarkers.show();
        AngleCruve.show();
       // ui->tegCurvePlot->detachItems(QwtPlotItem::Rtti_PlotCurve);
       // ui->tegCurvePlot->detachItems(QwtPlotItem::Rtti_PlotMarker);
        m_qwtPhaseCurve.hide();
    }
    ui->tegCurvePlot->replot();
}


void PlotInfo::SetTimeLine(int timelinelenght)
{
    ui->tegCurvePlot->setXaxis(0,timelinelenght);
    m_timeline = timelinelenght;
}

void PlotInfo::SetAmplitudeRange(double minvalue, double maxvalue)
{
    ui->tegCurvePlot->setAxisScale(QwtPlot::yLeft,minvalue,maxvalue);
    ymaxvalue = maxvalue;
    yminvalue = minvalue;
}

void PlotInfo::onSetTimelineTriggered()
{
    bool ok;
    QString timeline=QInputDialog::getText(this,tr("修改坐标轴"),tr("请输入显示坐标轴长度："),
                                       QLineEdit::Normal,"",&ok);
    if(!ok&&timeline.isEmpty()){
      //  QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("输入长度有误！"));
        return;
    }
    ui->tegCurvePlot->setXaxis(0,timeline.toInt(0,10));
    m_timeline = timeline.toInt(0,10);
}


int PlotInfo::GetTimelineLenght()
{
    return m_timeline;
}

void PlotInfo::onExportOriginDataTriggered()
{
    QString test_case_id = GetTestSampleID();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export CSV File"),test_case_id+".csv",
                                                    tr("(*.txt *.csv)"));
    QFile file(fileName);
    QString teststr;
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);


        QString freString;   //数据库中存储频率的字符串链表
        QString ampString;   //存储幅值的字符串链表
        QString phaseString;  //存储相位的字符串链表

        freString = get_origin_fre_data(test_case_id);
        ampString = get_origin_amp_data(test_case_id);
        phaseString = get_origin_phase_data(test_case_id);


        QStringList fre_str_list = freString.split("\n");
        QStringList amp_str_list = ampString.split("\n");
        QStringList phase_str_list = phaseString.split("\n");

        unsigned int numofscan = amp_str_list.size();
        unsigned int numofFre = fre_str_list.size();


        QList< QVector<QPointF> > listAmpPoint;   //采集的幅值点信息
        QList< QVector<QPointF> > listPhasePoint;  //采集相位点信息

        for(int ci = 0; ci < numofscan; ci++){
            QVector<QPointF> ampPoint;
            QVector<QPointF> phasePoint;
            QStringList one_amp_str_list = amp_str_list.at(ci).split(",");
            QStringList one_phase_str_list = phase_str_list.at(ci).split(",");
            for(int i = 0; i < numofFre; i++){
                QPointF amptempPoint(fre_str_list.at(i).toDouble(),one_amp_str_list.at(i).toDouble());
                QPointF phasetempPoint(fre_str_list.at(i).toDouble(),one_phase_str_list.at(i).toDouble());;
                ampPoint.append(amptempPoint);
                phasePoint.append(phasetempPoint);
            }

            listAmpPoint.append(ampPoint);
            listPhasePoint.append(phasePoint);
        }


      //  QTextStream testFileStream(&teststr);

    //    unsigned numOfScan = listAmpPoint.at(currentOfCurveRow)->size();

//        unsigned numOfPerSample = listAmpPoint.at(currentOfCurveRow)->at(0).size();
    /*    fileStream << "Frequenry"<<",";
        for (unsigned int ci = 0; ci < numOfScan; ci++)
        {
            fileStream << "Amp"<<ci+1<<","
                        << "Phase"<<ci+1;
            if (ci != numOfScan-1) fileStream << ",";
        }
        fileStream << '\n';*/

        for (unsigned int i = 0; i < numofFre; i++)
        {
            fileStream << listAmpPoint.at(0).at(i).x()<<",";

         //   testFileStream << AmpPoint[0][i].x()<<",";
            for (unsigned int ci = 0; ci < numofscan; ci++)
            {
                fileStream <<int(listAmpPoint.at(ci).at(i).y()*32768)<<","
                           <<int(listPhasePoint.at(ci).at(i).y()*65536/PI);
            //    testFileStream <<int(AmpPoint[ci][i].y()*32768)<<","
            //               <<int(PhasePoint[ci][i].y()*65536);
                if (ci != numofscan-1) fileStream << ",";
             //   if (ci != numOfScan-1) testFileStream << ",";
            }
            fileStream << '\n';
          //  testFileStream << '\n';
        }
     /*   fileStream<<"ResFrePoint"<<",";
        for(unsigned int ci = 0; ci < numOfScan; ci++)
        {
            fileStream<<AmpResPoint[ci].x();
            if (ci != numOfScan-1) fileStream << ",";
        }
        fileStream << '\n';

        fileStream<<"ResAmpPoint"<<",";
        for(unsigned int ci = 0; ci < numOfScan; ci++)
        {
            fileStream<<AmpResPoint[ci].y();
            if (ci != numOfScan-1) fileStream << ",";
        }
        fileStream << '\n';*/

    /*    fileStream<<"startFre"<<","<<"stopFre"<<","<<"InterivalFre"
                 <<","<<"DCData"<<","<<"ACData"<<","<<"ScanNum"
                <<","<<"ScanInterival"<<","<<"MutilScanInterival"<<'\n';
        fileStream<<setPareStrList.at(0)<<","<<setPareStrList.at(1)<<","
                 <<setPareStrList.at(2)<<","<<setPareStrList.at(3)<<","<<
                   setPareStrList.at(4)<<","<<setPareStrList.at(5)<<","<<
                   setPareStrList.at(6)<<","<<setPareStrList.at(7)<<'\n';*/
        qDebug()<<teststr;



       /* QStringList paralist = get_para_info(currentOfCurveRow);
        fileStream<<paralist.at(0)<<","<<paralist.at(1)<<","
                 <<paralist.at(2)<<","<<paralist.at(3)<<","<<
                   paralist.at(4)<<","<<paralist.at(5)<<","<<
                   paralist.at(6)<<","<<paralist.at(7)<<'\n';*/

//        fileStream<<"startFre"<<","<<"stopFre"<<","<<"InterivalFre"<<","<<"ScanInterival"
//                         <<","<<"DCData"<<","<<"ACData"<<","<<"ScanNum"
//                        <<","<<"MutilScanInterival"<<'\n';

    }
    else
    {
        qCritical() << "File open error during export: " << file.error();
    }
}


void PlotInfo::onExportPhaseTimeDataTriggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export CSV File"),".csv",
                                                    tr("(*.txt *.csv)"));
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);
        unsigned int size = m_vecPhasePoint.size();
        for (unsigned int i = 0; i < size; i++)
        {
            fileStream << m_vecPhasePoint.at(i).x()<<","<< m_vecPhasePoint.at(i).y()<<'\n';

        }
    }else
    {
        qCritical() << "File open error during export: " << file.error();
    }
}

void PlotInfo::onSetAmplitudeRangeTriggered()
{
    bool ok;
    QString min=QInputDialog::getText(this,tr("设置Y轴范围"),tr("请输入Y轴最小值："),
                                       QLineEdit::Normal,"",&ok);
    if(!ok&&min.isEmpty()){
      //  QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("输入长度有误！"));
        return;
    }
    QString max=QInputDialog::getText(this,tr("设置Y轴范围"),tr("请输入Y轴最大值："),
                                       QLineEdit::Normal,"",&ok);
    if(!ok&&max.isEmpty()){
      //  QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("输入长度有误！"));
        return;
    }
    ui->tegCurvePlot->setYaxis(min.toDouble(),max.toDouble());
    ymaxvalue = max.toDouble();
    yminvalue = min.toDouble();

}

QPointF PlotInfo::GetRvaluePoint()
{
    return RValuePoint;
}


QPointF PlotInfo::GetKvaluePoint()
{
    return KValuePoint;
}

QPointF PlotInfo::GetMAvaluePoint()
{
    return MAValuePoint;
}

double PlotInfo::GetAngleValue()
{
    return angleValue;
}

Plot* PlotInfo::GetTegPlot()
{
    return ui->tegCurvePlot;
}


QPointF PlotInfo::linefunc(QPointF point1, QPointF point2)
{
    QPointF linepara;
    linepara.setX((point1.ry()-point2.ry())/(point1.rx()-point2.rx()));
    linepara.setY(point1.ry()-(linepara.rx()*point1.rx()));
    return linepara;
}


double PlotInfo::getlineX(QPointF lineSlope, double yValue)
{
    double  xValue;
    xValue = (yValue-lineSlope.ry())/lineSlope.rx();
    return xValue;
}
