#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QFileDialog>
#include <QtMath>
#include <synchapi.h>
#include <qwt_plot_renderer.h>
#include <QImage>

#include "newpatientdlg.h"
#include "printdialog.h"
#include "selectmergedlg.h"
#include "hdbexchangedlg.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_commConfigDlg(&m_serialPort),
    m_bPortIsOpened(false),
    m_bIsPreExperiment(false),
    platSensorSetForm(&m_serialPort),
    optimalACBiasForm(&m_serialPort),
    optimalDCBiasForm(&m_serialPort),
    m_bIsStartReceiveDataFlag(false),
    m_bIsStopReceiveDataFlag(false),
    m_iCurrentScanNum(0),
    m_viewtype(OriginPhaseType),
    isOptimalDCBiasFlag(false),
    isOptimalACBiasFlag(false),
    iDCScanNum(0),
    iACScanNum(0)
{
    ui->setupUi(this);

    ui->listPlotWidget->setSpacing(10);
//    ui->listSampleWidget->setSpacing(5);


    m_currentUserData = get_user_data();
    changeWindowTitle(m_currentUserData.userName,m_currentUserData.dbName);

    InitShowCurveWindow();
    InitDataBase();

    showTempLabel.setText("螺旋管目前的温度：00.0摄氏度");
    showTempLabel.setToolTip("但开始采集后来显示螺旋管的当前温度");
    ui->statusBar->addPermanentWidget(&showTempLabel);

    m_OriginPlot.setXButtonTitle(tr("频率[kHz]"));
    m_OriginPlot.setYLeftTitle(tr("幅值[V]"));
    m_viewOriginDataPlot.setXButtonTitle(tr("频率[kHz]"));
    m_viewOriginDataPlot.setYLeftTitle(tr("幅值[V]"));
    QObject::connect(ui->actionCase,SIGNAL(triggered()),
                     this,SLOT(on_NewSample_triggered()));
    QObject::connect(ui->actionStartDetect,SIGNAL(triggered()),
                    this,SLOT(StartDetectSignal()));
    QObject::connect(ui->actionStopDetect,SIGNAL(triggered()),
                     this,SLOT(StopDetectSignal()));
    QObject::connect(&m_serialPort,&QSerialPort::readyRead,
                    this,&MainWindow::onDataReady);
    QObject::connect(ui->actionPreExperiment,SIGNAL(triggered()),
                    this,SLOT(OnPreExperiment()));
    QObject::connect(ui->listPlotWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                    this,SLOT(selectListPlotOnDoubleClicked(QListWidgetItem*)));
    QObject::connect(ui->listPlotWidget,SIGNAL(itemClicked(QListWidgetItem*)),
                    this,SLOT(selectListPlotOnSignalClicked(QListWidgetItem*)));

    QObject::connect(ui->actionNewPatient,SIGNAL(triggered()),
                     this,SLOT(onNewPatienttriggered()));
    QObject::connect(&(this->m_serialPort), SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(onPortError(QSerialPort::SerialPortError)));

    QObject::connect(ui->actionOpenDB,SIGNAL(triggered()),this,SLOT(onOpenDBtriggered()));

    QObject::connect(ui->actionNewDB,SIGNAL(triggered()),this,SLOT(onNewCreateDBtriggered()));

    QObject::connect(ui->actionAuthorizeDB,SIGNAL(triggered()),this,SLOT(onAuthorizeDBtriggered()));

    QObject::connect(ui->action_Preset,SIGNAL(triggered()),this,SLOT(onSystemSetupTriggered()));

    QObject::connect(ui->actionOriginDataView,SIGNAL(triggered(bool)),this,SLOT(onOriginPhaseViewTriggered(bool)));

    QObject::connect(ui->actionTegDataView,SIGNAL(triggered(bool)),this,SLOT(onTegDataViewTriggered(bool)));

    QObject::connect(ui->actionTimeline,SIGNAL(triggered()),this,SLOT(onSetTimeLineTriggered()));

    QObject::connect(ui->actionExportDB,SIGNAL(triggered()),this,SLOT(onExportDatabaseTriggered()));

    QObject::connect(ui->actionImportDB,SIGNAL(triggered()),this,SLOT(onImportDatabaseTriggered()));


    QObject::connect(&ProcessNoDataTimer, SIGNAL(timeout()), this, SLOT(LongTimeNoData()));

    QObject::connect(ui->actionPrint,SIGNAL(triggered(bool)),this,SLOT(onPrintReportTriggered()));


    QObject::connect(&optimalDCBiasForm,&OptimalDCBiasForm::sendOptimalDCPara,
                     this,&MainWindow::OptimalDCBiasScan);

    QObject::connect(&optimalACBiasForm,&OptimalACBiasForm::sendOptimalACPara,
                     this,&MainWindow::OptimalACBiasScan);


}

MainWindow::~MainWindow()
{
    delete ui;
    QSqlDatabase::database(m_currentUserData.dbPathName).close();
    QSqlDatabase::removeDatabase(m_currentUserData.dbPathName);
}

void MainWindow::on_NewSample_triggered()
{
    NewSampleDlg newSampleDlg;
    if(newSampleDlg.exec()==QDialog::Accepted){       
        m_listPlotInfo.append(new PlotInfo);
        m_listPlotInfo.last()->SetTestSampleID(newSampleDlg.GetTestCaseID());
        m_listPlotInfo.last()->SetPatientName(newSampleDlg.GetPatientID());
        m_listPlotInfo.last()->SetCurveDataView(m_viewtype);
        QObject::connect(m_listPlotInfo.last(),SIGNAL(viewOriginData(QString)),this,SLOT(onViewOriginData(QString)));
        m_itemPlotInfo.append(new QListWidgetItem);
        m_itemPlotInfo.last()->setSizeHint(QSize(300,200));
        ui->listPlotWidget->addItem(m_itemPlotInfo.last());
        ui->listPlotWidget->setItemWidget(m_itemPlotInfo.last(),m_listPlotInfo.last());
        ui->listPlotWidget->setFocusProxy(m_listPlotInfo.last());

        m_listSampleInfo.append(new AuxiliaryInfoForm);
        m_listSampleInfo.last()->SetTestSampleID(newSampleDlg.GetTestCaseID());
        m_listSampleInfo.last()->SetPatientName(newSampleDlg.GetPatientID());
        m_itemSampleInfo.append(new QListWidgetItem);
        m_itemSampleInfo.last()->setSizeHint(QSize(300,80));
//        ui->listSampleWidget->addItem(m_itemSampleInfo.last());
//        ui->listSampleWidget->setItemWidget(m_itemSampleInfo.last(),m_listSampleInfo.last());
//        ui->listSampleWidget->setFocusProxy(m_listSampleInfo.last());

        ui->actionStartDetect->setEnabled(true);
        ui->actionStopDetect->setEnabled(false);
    }
}

void MainWindow::onNewPatienttriggered()
{
    NewPatientDlg newPatientDlg;
    if(newPatientDlg.exec()==QDialog::Accepted){
        if(newPatientDlg.m_bIsNewSample){
            on_NewSample_triggered();
        }
    }
}

void MainWindow::on_actionComSet_triggered()
{
   if(m_commConfigDlg.exec()==QDialog::Accepted){
        WriteComSetSetting();
   }
}

//启动数据采集
void MainWindow::StartDetectSignal()
{
    if(!m_vecPhaseTimePoint.isEmpty()){
        QMessageBox button;
        button.setText("提示信息!");
        button.setInformativeText("确定：将清除所有采集数据，并重新开始\n\r取消：放弃本次操作，保留原来数据");
        button.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        button.setDefaultButton(QMessageBox::No);
        button.setButtonText(QMessageBox::Yes,QString("确定"));
        button.setButtonText(QMessageBox::No,QString("取消"));

        int ret = button.exec();

        if (ret == QMessageBox::No) {
            return;  //忽略退出信号，程序继续运行
        }
    }

    if(!m_bPortIsOpened){
        m_commConfigDlg.togglePort();
        //判断串口是否能使用
        if(m_serialPort.isOpen()){
            m_stParaInfo = m_commConfigDlg.GetParaInfo();
            m_testParaForm.setTestPara(m_stParaInfo);
            update_test_para(m_stParaInfo.iStartFre,m_stParaInfo.iStopFre,m_stParaInfo.iStepFre,m_stParaInfo.iStepInterivalTime,
                             m_stParaInfo.iOptionalDC,m_stParaInfo.iOptionalAC,m_stParaInfo.iScanNumbers,m_stParaInfo.iMutilScanInterivalTime,m_listPlotInfo.last()->GetTestSampleID());

            if(SendStartCommand(m_stParaInfo)){
                m_serialPort.waitForBytesWritten(10);
                m_dtStartTime = QDateTime::currentDateTime();
                ClearAllCurves();
                QDateTime currenttime = QDateTime::currentDateTime();
                QString strDate = currenttime.toString("yyyy/MM/dd HH:mm:ss");
                m_listPlotInfo.last()->SetStartTestTime(strDate);
                update_test_start_time_info(strDate,m_listPlotInfo.last()->GetTestSampleID());

                qDebug()<<m_stParaInfo.iStartFre;
                m_bPortIsOpened = true;
                ui->actionStartDetect->setEnabled(false);
                ui->actionStopDetect->setEnabled(true);
                ui->statusBar->showMessage(tr("已打开串口，并开始进行检测"),1000);
            }
        }
    }
}

void MainWindow::StopDetectSignal()
{
    if(m_serialPort.isOpen()){
        if(SendStopCommand()){
            m_bIsStartReceiveDataFlag = false;
            m_serialPort.waitForBytesWritten(1);
            m_commConfigDlg.togglePort();
            QDateTime currenttime = QDateTime::currentDateTime();
            QString strDate = currenttime.toString("yyyy/MM/dd HH:mm:ss");
            m_listPlotInfo.last()->SetEndTestTime(strDate);
            update_test_end_time_info(strDate,m_listPlotInfo.last()->GetTestSampleID());
            m_bPortIsOpened = false;
            update_test_para(m_stParaInfo.iStartFre,m_stParaInfo.iStopFre,m_stParaInfo.iStepFre,m_stParaInfo.iStepInterivalTime,
                             m_stParaInfo.iOptionalDC,m_stParaInfo.iOptionalAC,m_iCurrentScanNum,m_stParaInfo.iMutilScanInterivalTime,
                             m_listPlotInfo.last()->GetTestSampleID());

            m_iCurrentScanNum = 0;
            ui->actionStopDetect->setEnabled(false);
            ui->actionStartDetect->setEnabled(true);
            ui->actionStartDetect->setIconText(tr("启动"));
            ui->statusBar->showMessage(tr("已停止检测，并关闭串口！"),1000);
        }
    }
}

//串口准备接收数据
void MainWindow::onDataReady()
{
    int packageSize = ((((m_stParaInfo.iStopFre-m_stParaInfo.iStartFre))/m_stParaInfo.iStepFre)+1)*10+5;  //每次采集的数量点
    int bytesAvailable = m_serialPort.bytesAvailable();

    unsigned int numOfPackagesToRead =
            (bytesAvailable - (bytesAvailable % packageSize)) / packageSize;

    if(!m_bIsStartReceiveDataFlag){
        if(bytesAvailable%5==0){

            QByteArray tempData = m_serialPort.read(5);
            qDebug()<<"开始的字节："<<tempData;
            ProcessNoDataTimer.stop();
            if(DecideStartData(tempData)) m_bIsStartReceiveDataFlag = true;
            else {
                ui->statusBar->showMessage(tr("下位机传输的起始命令格式有错误！"),10);
               // StartDetectSignal();
                if(m_serialPort.isOpen()){
                    SendStopCommand();
                    m_serialPort.waitForBytesWritten(10);
                    m_commConfigDlg.togglePort();

                    m_commConfigDlg.togglePort();
                    m_serialPort.waitForBytesWritten(1000);
                    if(m_serialPort.isOpen()){
                        while(!SendStartCommand(m_stParaInfo)){
                            m_commConfigDlg.togglePort();
                            m_commConfigDlg.togglePort();
                        }
                    }
                    return;
                }
            }  
        }else{
            return;
        }
    }else{
        QString tempStr;
        tempStr.sprintf("正在接收第%d次数据...",m_iCurrentScanNum+1);
        ui->statusBar->showMessage(tempStr,1000);
        if(bytesAvailable < packageSize) return;
        for(unsigned int ci = 0; ci < numOfPackagesToRead; ci++)
        {
            QByteArray tempData = m_serialPort.read(packageSize);
            if(!addChannelData(tempData,packageSize)) {
                return;
            }
            m_iCurrentScanNum++;
        }

        if(m_iCurrentScanNum>=m_stParaInfo.iScanNumbers){
            int size = m_serialPort.bytesAvailable();
            m_serialPort.waitForReadyRead(10);
            QByteArray tempData = m_serialPort.read(5);
            m_bIsStartReceiveDataFlag = false;

            qDebug()<<"最后的字节:"<<tempData<<" "<<size;
            if(DecideStopData(tempData)){
                ui->statusBar->showMessage(tr("数据接收已正常结束！"),1000);
                StopDetectSignal();

            }else{
                ui->statusBar->showMessage(tr("数据接收异常结束！"),1000);
                StopDetectSignal();
            }           
        }

        //判断是否为最佳偏置扫描
        if(isOptimalACBiasFlag){
            iACScanNum++;
            if((iACScanNum*iStepAC)+iStartAC<=iStopAC){
                sendOptimalDCPara((iACScanNum*iStepAC)+iStartAC);
            }else{
                isOptimalACBiasFlag = false;
            }
        }
        if(isOptimalDCBiasFlag){
            iDCScanNum++;
            if((iDCScanNum*iStepDC)+iStartDC<=iStopDC){
                sendOptimalDCPara((iDCScanNum*iStepDC)+iStartDC);
            }else{
                isOptimalDCBiasFlag = false;
            }
        }


    }
}

/*发送开始采集命令*/
bool MainWindow::SendStartCommand(stParaInfo &stparainfo)
{
    QByteArray  SendStartBuf;

    SendStartBuf[0] = 0x86;
    SendStartBuf[1] = 0x11;
    SendStartBuf[2] = 0x01;
    SendStartBuf[3] = 0x02;

    char cStartFreHigh,cStartFreLow,cStopFreHigh,cStopFreLow,
            cInterivalFre,cDCDataLow,cDCDataHigh,cACDataHigh,cACDataLow,
            cScanInterival,cScanNumHigh,cScanNumLow,cMutiScanInterivalHigh,
            cMutiScanInterivalLow;

    m_OriginPlot.setXaxis(stparainfo.iStartFre/1000,stparainfo.iStopFre/1000);
    m_OriginPlot.replot();

    if(stparainfo.iStartFre>=stparainfo.iStopFre){
        return false;
    }

    cStartFreLow = char(stparainfo.iStartFre/10);
    SendStartBuf.append(cStartFreLow);
    cStartFreHigh = char((stparainfo.iStartFre/10)>>8);
    SendStartBuf.append(cStartFreHigh);

    cStopFreLow = char(stparainfo.iStopFre/10);
    SendStartBuf.append(cStopFreLow);
    cStopFreHigh = char((stparainfo.iStopFre/10)>>8);
    SendStartBuf.append(cStopFreHigh);


    cInterivalFre = char(stparainfo.iStepFre);
    SendStartBuf.append(cInterivalFre);


    cScanInterival = char(stparainfo.iStepInterivalTime);
    SendStartBuf.append(cScanInterival);

    cDCDataLow = char(stparainfo.iOptionalDC);
    SendStartBuf.append(cDCDataLow);
    cDCDataHigh =char(stparainfo.iOptionalDC>>8);
    SendStartBuf.append(cDCDataHigh);

    cACDataLow = char(stparainfo.iOptionalAC);
    SendStartBuf.append(cACDataLow);
    cACDataHigh = char(stparainfo.iOptionalAC>>8);
    SendStartBuf.append(cACDataHigh);


    cScanNumLow = char(stparainfo.iScanNumbers);
    SendStartBuf.append(cScanNumLow);
    cScanNumHigh = char(stparainfo.iScanNumbers>>8);
    SendStartBuf.append(cScanNumHigh);

    cMutiScanInterivalLow = char(stparainfo.iMutilScanInterivalTime);
    SendStartBuf.append(cMutiScanInterivalLow);
    cMutiScanInterivalHigh = char(stparainfo.iMutilScanInterivalTime>>8);
    SendStartBuf.append(cMutiScanInterivalHigh);

    char endflagbyte = 0x68;
    SendStartBuf.append(endflagbyte);

    if(!m_serialPort.write(SendStartBuf)){
       return false;
    }

    m_serialPort.waitForBytesWritten(10);
    qDebug()<<"send data"<<SendStartBuf;

    int alltime = stparainfo.iMutilScanInterivalTime*2+
            ((stparainfo.iStopFre-stparainfo.iStartFre)/stparainfo.iStepFre)*stparainfo.iStepInterivalTime;
    ProcessNoDataTimer.start(alltime);
    return true;
}


void MainWindow::LongTimeNoData()
{
    if(ProcessNoDataTimer.isActive())
        ProcessNoDataTimer.stop();
    ProcessReceiveErrorData();
}

bool MainWindow::SendStopCommand()
{

    QByteArray SendStopBuf;
    SendStopBuf[0]=0x86;
    SendStopBuf[1]=0x03;
    SendStopBuf[2]=0x03;
    SendStopBuf[3]=0x01;
    SendStopBuf[4]=0x68;
    qDebug()<<SendStopBuf;
    if(!m_serialPort.write(SendStopBuf)){
        return false;
    }
    m_serialPort.waitForBytesWritten(10);
    return true;
}


void MainWindow::OnPreExperiment()
{
    IsShowPreExperiment();
}


void MainWindow::InitShowCurveWindow()
{

    setWindowState(Qt::WindowMaximized);
  //  m_twParaSet.insertTab(0,&platSensorSetForm,QIcon(":/mainfrmImage/hardware_set.png"),"硬件配置");
  //  m_twParaSet.insertTab(1,&paraSetForm,QIcon(":/mainfrmImage/setup1.png"),"参数配置");
    m_twParaSet.insertTab(0,&optimalDCBiasForm,QIcon(":/mainfrmImage/OptimalDC.png"),"最佳直流偏置");
    m_twParaSet.insertTab(1,&optimalACBiasForm,QIcon(":/mainfrmImage/OptimalAC.png"),"最佳交流偏置");
  //  m_twParaSet.insertTab(4,&m_commConfigDlg,QIcon(":/mainfrmImage/OptimalAC.png"),"通信配置");
    m_twParaSet.setFixedHeight(250);
    detailedSampleForm.hide();
    m_OriginPlot.hide();
    m_twParaSet.hide();
    m_testParaForm.hide();
    m_viewOriginDataPlot.hide();
    ui->gridLayout_4->addWidget(&m_OriginPlot,0,0);
    ui->gridLayout_4->addWidget(&m_twParaSet,1,0);
    ui->gridLayout_4->addWidget(&m_viewOriginDataPlot,0,0);
    ui->gridLayout_4->addWidget(&m_testParaForm,0,1,2,1);
    ui->gridLayout_4->addWidget(&detailedSampleForm);


}

void MainWindow::selectListPlotOnDoubleClicked(QListWidgetItem *item)
{
    IsShowDetailSample();
    ui->listPlotWidget->setCurrentItem(item);
    PlotInfo *plotinfo = (PlotInfo *)ui->listPlotWidget->itemWidget(item);
    detailedSampleForm.SetCurveDataView(m_viewtype);
    detailedSampleForm.SetCurveData(plotinfo->m_vecPhasePoint);
    detailedSampleForm.SetTegCurveData(plotinfo->m_vecPhasePoint);
    detailedSampleForm.SetPatientName(plotinfo->GetPatientName());
    detailedSampleForm.SetDetectionTime(plotinfo->GetDetectionTime());
    detailedSampleForm.SetTimelineLenght(plotinfo->GetTimelineLenght());
}

void MainWindow::selectListPlotOnSignalClicked(QListWidgetItem *item)
{
    ui->listPlotWidget->setCurrentItem(item);
    PlotInfo *plotinfo = (PlotInfo *)ui->listPlotWidget->itemWidget(item);
    ui->lbRvalue->setText(QString::number(plotinfo->GetRvaluePoint().x()/60,'f',2));
    ui->lbKvalue->setText(QString::number((plotinfo->GetKvaluePoint().x()- plotinfo->GetRvaluePoint().x())/60,'f',2));
    ui->lbMAValue->setText(QString::number(plotinfo->GetMAvaluePoint().y()*(-1),'f',3));
    ui->lbAngleValue->setText(QString::number(plotinfo->GetAngleValue(),'f',2));

    QString test_case_id = plotinfo->GetTestSampleID();

    stParaInfo stparainfo = get_para_info(test_case_id);

    ui->lbStartFre->setText(QString::number(stparainfo.iStartFre,10));
    ui->lbStopFre->setText(QString::number(stparainfo.iStopFre,10));
    ui->lbStepFre->setText(QString::number(stparainfo.iStepFre,10));
    ui->lbStepTime->setText(QString::number(stparainfo.iStepInterivalTime,10));
    ui->lbOptinalDC->setText(QString::number(stparainfo.iOptionalDC,10));
    ui->lbOptinalAC->setText(QString::number(stparainfo.iOptionalAC,10));
    ui->lbScanNumbers->setText(QString::number(stparainfo.iScanNumbers,10));
    ui->lbMutilTime->setText(QString::number(stparainfo.iMutilScanInterivalTime,10));
}

//判断是否为起始检测数据帧
bool MainWindow::DecideStartData(const QByteArray &startData)
{
    QByteArray startReceiveData;

    startReceiveData[0]=0x02;
    startReceiveData[1]=0x05;
    startReceiveData[2]=0xFF;
    startReceiveData[3]=0X00;
    startReceiveData[4]=0XFF;
    if(startData != startReceiveData){
         return false;
    }
    
    return true;
}

bool MainWindow::DecideStopData(const QByteArray &stopData)
{
     QByteArray stopReceiveData;
     stopReceiveData[0]=0x02;
     stopReceiveData[1]=0x05;
     stopReceiveData[2]=0xFF;
     stopReceiveData[3]=0X01;
     stopReceiveData[4]=0XFF;

     if(stopData!=stopReceiveData) return false;
     return true;
}

void MainWindow::ProcessReceiveErrorData()
{
    if(m_serialPort.isOpen()){
        if(SendStopCommand()){
            m_bIsStartReceiveDataFlag = false;
            m_serialPort.waitForBytesWritten(1);
            m_commConfigDlg.togglePort();
            m_bPortIsOpened = false;
            msleep(20);
            while(!m_serialPort.isOpen()){
                 m_commConfigDlg.togglePort();
            }

            //判断串口是否能使用
            if(m_serialPort.isOpen()){
                m_stParaInfo = m_commConfigDlg.GetParaInfo();
                int tempnum = m_stParaInfo.iScanNumbers;
                m_stParaInfo.iScanNumbers = m_stParaInfo.iScanNumbers-m_iCurrentScanNum;
                while(!SendStartCommand(m_stParaInfo)){
                    m_commConfigDlg.togglePort();
                    m_commConfigDlg.togglePort();
                }

                m_serialPort.waitForBytesWritten(10);
                qDebug()<<"currentnum:"<<m_iCurrentScanNum;
                qDebug()<<"send_nums"<<m_stParaInfo.iScanNumbers;
                m_stParaInfo.iScanNumbers = tempnum;
                qDebug()<<"scans:"<<m_stParaInfo.iScanNumbers;
                m_bPortIsOpened = true;
            }
        }
    }
}

/***********将串口读取的数据存取及在绘图区域显示***********/
bool MainWindow::addChannelData(QByteArray &data, unsigned size)
{   
    qDebug()<<data.toHex()<<"大小："<<size<<"\n";
    QVector<QPointF> ampPoint;
    QVector<QPointF> phasePoint;
    double maxAmpRes = 0;
    double maxPhaseRes = 0;
    QPointF ampresPoint;
    QPointF phaseresPoint;
    if(((unsigned char)(data.at(size-5))!=0x04)||((unsigned char)(data.at(size-1))!=0xFF)){
        ProcessReceiveErrorData();
        return false;

    }

    for(unsigned int i=0; i < size-5; i=i+10)
    {
        //判断接收数据是否发生错误，如果发生错误，将重新打开串口进行采集
        if(((unsigned char)(data.at(i))!=0x00)||((unsigned char)(data.at(i+9))!=0xFF)){
            ProcessReceiveErrorData();
            return false;

        }


        double xValue = ((double)(m_stParaInfo.iStartFre)+(double)(((int) i/10)*m_stParaInfo.iStepFre))/1000.0;
        double yAmpValue = ((double)(quint8)data.at(i+2)*256+(quint8)data.at(i+3))/32768;
        double yPhaseValue = (double)((quint8)data.at(i+7)*256+(quint8)data.at(i+8))*PI/65536;
        QPointF ampTemppoint(xValue,yAmpValue);
        QPointF phaseTemppoint(xValue,yPhaseValue);
        ampPoint.append(ampTemppoint);
        phasePoint.append(phaseTemppoint);
        if(ampTemppoint.y()> maxAmpRes)
        {
            maxAmpRes = ampTemppoint.y();
            ampresPoint = ampTemppoint;
        }
        if(phaseTemppoint.y()> maxPhaseRes)
        {
            maxPhaseRes = phaseTemppoint.y();
            phaseresPoint = phaseTemppoint;
        }
    }


    double currentTemp = ((double)(quint8)data.at(size-3)*256+(quint8)data.at(size-2))*1023.75/4096;
    showTempLabel.setText("螺旋管目前的温度"+QString::number(currentTemp,10,1)+"摄氏度");


    QDateTime currentTime = QDateTime::currentDateTime();
    uint difftime = currentTime.toTime_t()-m_dtStartTime.toTime_t();

    QPointF amptime(difftime,ampresPoint.y());
    QPointF phasetime(difftime,phaseresPoint.y());


    m_testParaForm.setTestResult(phaseresPoint.y(),phaseresPoint.x());

    m_vecAmpTimePoint.append(amptime);
    m_vecPhaseTimePoint.append(phasetime);


    m_strResAmpValue = m_strResAmpValue+QString("%1,").arg(ampresPoint.y());
    m_strResPhaseValue = m_strResPhaseValue+QString("%1,").arg(phaseresPoint.y());
    m_strResTimestampValue = m_strResTimestampValue+QString("%1,").arg(difftime);

    m_listAmpPoint.append(ampPoint);
    m_listPhasePoint.append(phasePoint);

    m_vecAmpResPoint.append(ampresPoint);
    m_vecPhaseResPoint.append(phaseresPoint);

    if(isOptimalDCBiasFlag){
        QPointF DCAmpPoint((iDCScanNum*iStepDC)+iStartDC,phaseresPoint.y());
        optimalDCBiasForm.drawOptimalDCBiasCurve(DCAmpPoint);
    }

    if(isOptimalACBiasFlag){
        QPointF ACAmpPoint((iACScanNum*iStepAC)+iStartAC,phaseresPoint.y());
        optimalACBiasForm.drawOptimalACBiasCurve(ACAmpPoint);
    }

    //存储原始数据
    if(!m_listAmpPoint.isEmpty()){
        QString test_id = m_listPlotInfo.last()->GetTestSampleID();

        QString strFrequency;
        QString strAmplitude;
        QString strPhase;
        QTextStream freFileStream(&strFrequency);
        QTextStream ampFlieStream(&strAmplitude);
        QTextStream phaseFileStream(&strPhase);

        int numOfScan = m_listAmpPoint.size();
        int numOfFre = m_listAmpPoint.at(0).size();

        for(int ci = 0; ci < numOfFre; ci++){
            freFileStream<<m_listAmpPoint.at(0).at(ci).x();
            if(ci != numOfFre-1) freFileStream<<"\n";
        }

        for(int ci = 0; ci < numOfScan; ci++){

            for(int i = 0; i < numOfFre; i++){
                ampFlieStream<<m_listAmpPoint.at(ci).at(i).y()<<",";
                phaseFileStream<<m_listPhasePoint.at(ci).at(i).y()<<",";
            }
            if(ci != numOfScan-1){
                ampFlieStream<<"\n";
                phaseFileStream<<"\n";
            }

        }
    //    qDebug()<<strFrequency<<strAmplitude<<strPhase;
        update_origin_data_info(strAmplitude,strPhase,strFrequency,test_id);
    }


    //更新TEG数据
    m_listPlotInfo.last()->SetCurveData(m_vecPhaseTimePoint);
    m_listPlotInfo.last()->SetTegCurveData(m_vecPhaseTimePoint);

    if(ui->listPlotWidget->currentItem()==m_itemPlotInfo.last()){
        detailedSampleForm.SetCurveData(m_vecPhaseTimePoint);
        detailedSampleForm.SetTegCurveData(m_vecPhaseTimePoint);
    }


    update_teg_data_info(m_strResAmpValue,m_strResPhaseValue,m_strResTimestampValue,m_listPlotInfo.last()->GetTestSampleID());

    m_listAmpCurves.append(new QwtPlotCurve());
    m_listAmpCurves.last()->setSamples(ampPoint);
    m_listAmpCurves.last()->setPen(Qt::red,1.0,Qt::SolidLine);
    m_listAmpCurves.last()->setCurveAttribute(QwtPlotCurve::Fitted);
    m_listAmpCurves.last()->attach(&m_OriginPlot);

    m_listPhaseCurves.append(new QwtPlotCurve());
    m_listPhaseCurves.last()->setSamples(phasePoint);
    m_listPhaseCurves.last()->setPen(Qt::green,1.0,Qt::SolidLine);
    m_listPhaseCurves.last()->setCurveAttribute(QwtPlotCurve::Fitted);
    m_listPhaseCurves.last()->attach(&m_OriginPlot);

    m_OriginPlot.replot();

    return true;

}

void MainWindow::ClearAllCurves()
{
    //清除预实验的曲线
    int CurvesNum = m_listAmpPoint.size();
    for (int ci = 0; ci < CurvesNum; ci++)
    {
        delete m_listAmpCurves.takeLast();
        delete m_listPhaseCurves.takeLast();

        m_listAmpPoint.removeLast();
        m_listPhasePoint.removeLast();
        m_vecAmpResPoint.removeLast();
        m_vecPhaseResPoint.removeLast();
    }

    m_OriginPlot.replot();

    //清除正在测试的list曲线
    m_vecPhaseTimePoint.clear();
    m_strResAmpValue="";
    m_strResPhaseValue="";
    m_strResTimestampValue="";

}

void MainWindow::InitDataBase()
{
    QStringList testidstringlist;
    testidstringlist = get_all_testid();

    int recordNum = testidstringlist.size();
    if(recordNum<=0){
        return;
    }

    for(int i = 0; i < recordNum; i++){
        m_listPlotInfo.append(new PlotInfo);
        m_listPlotInfo.last()->SetTestSampleID(testidstringlist[i]);
        QObject::connect(m_listPlotInfo.last(),SIGNAL(viewOriginData(QString)),this,SLOT(onViewOriginData(QString)));
        QString stramp = get_teg_data_amp(testidstringlist[i]);
        QString strphase = get_teg_data_phase(testidstringlist[i]);
        QString strtime = get_teg_data_time(testidstringlist[i]);
        QStringList amp_str_list = stramp.split(",");
        QStringList phase_str_list = strphase.split(",");
        QStringList time_str_list = strtime.split(",");

        int size = time_str_list.size();

        QVector<QPointF> ampPoint;
        QVector<QPointF> phasePoint;

        for(int i=0; i<size-1; i++){

            QPointF amptempPoint(time_str_list.at(i).toInt(),amp_str_list.at(i).toDouble());
            QPointF phasetempPoint(time_str_list.at(i).toInt(),phase_str_list.at(i).toDouble());
            ampPoint.append(amptempPoint);
            phasePoint.append(phasetempPoint);
        }

        m_listPlotInfo[i]->SetCurveData(phasePoint);
        m_listPlotInfo[i]->SetTegCurveData(phasePoint);
        m_listPlotInfo[i]->SetCurveDataView(m_viewtype);
        m_listPlotInfo.last()->SetPatientName(get_test_case_patient_info(testidstringlist[i]));
        m_listPlotInfo.last()->SetStartTestTime(get_test_start_time(testidstringlist[i]));
        m_listPlotInfo.last()->SetEndTestTime(get_test_end_time(testidstringlist[i]));
        m_itemPlotInfo.append(new QListWidgetItem);
        m_itemPlotInfo.last()->setSizeHint(QSize(300,200));
        ui->listPlotWidget->addItem(m_itemPlotInfo.last());
        ui->listPlotWidget->setItemWidget(m_itemPlotInfo.last(),m_listPlotInfo.last());
        ui->listPlotWidget->setFocusProxy(m_listPlotInfo.last());

        m_listSampleInfo.append(new AuxiliaryInfoForm);
        m_listSampleInfo.last()->SetTestSampleID(testidstringlist[i]);
        m_listSampleInfo.last()->SetPatientName(get_test_case_patient_info(testidstringlist[i]));
        m_itemSampleInfo.append(new QListWidgetItem);
        m_itemSampleInfo.last()->setSizeHint(QSize(300,80));
//        ui->listSampleWidget->addItem(m_itemSampleInfo.last());
//        ui->listSampleWidget->setItemWidget(m_itemSampleInfo.last(),m_listSampleInfo.last());
//        ui->listSampleWidget->setFocusProxy(m_listSampleInfo.last());
    }


}

void MainWindow::on_actionMainWindows_triggered()
{
    IsShowMainWindowed();
}


void MainWindow::IsShowMainWindowed()
{
    detailedSampleForm.hide();
    if(ui->actionPreExperiment->isChecked()){
        OnPreExperiment();
        ui->actionPreExperiment->setChecked(false);
    }
    m_testParaForm.hide();
    m_viewOriginDataPlot.hide();
    ui->listPlotWidget->show();
    ui->frame_12->show();
    setWindowState(Qt::WindowMaximized);
}

void MainWindow::IsShowPreExperiment()
{
    if(!m_bIsPreExperiment){
        ui->listPlotWidget->hide();
        ui->scrollArea->hide();
        detailedSampleForm.hide();
        ui->frame_12->hide();
        m_viewOriginDataPlot.hide();
        m_OriginPlot.show();
        m_twParaSet.show();
        m_testParaForm.show();
        m_bIsPreExperiment = true;
    }else{
        m_OriginPlot.hide();
        m_twParaSet.hide();
        detailedSampleForm.hide();
        m_testParaForm.hide();
        m_viewOriginDataPlot.hide();
        ui->listPlotWidget->show();
        ui->scrollArea->show();
        ui->frame_12->show();
        m_bIsPreExperiment = false;
        setWindowState(Qt::WindowMaximized);
    }
}

void MainWindow::IsShowDetailSample()
{
    ui->listPlotWidget->hide();
    ui->frame_12->hide();
    m_OriginPlot.hide();
    m_twParaSet.hide();
    m_testParaForm.hide();

    detailedSampleForm.show();

}

void MainWindow::IsShowViewOriginData()
{
    ui->listPlotWidget->hide();
    ui->frame_12->hide();
    m_OriginPlot.hide();
    m_twParaSet.hide();
    detailedSampleForm.hide();

    m_viewOriginDataPlot.show();
    m_testParaForm.show();
}

void MainWindow::onPortError(QSerialPort::SerialPortError error)
{
    switch(error)
    {
        case QSerialPort::NoError :
            break;
        case QSerialPort::ResourceError :
            qWarning() << "Port error: resource unavaliable; most likely device removed.";
            if (m_serialPort.isOpen())
            {
                qWarning() << "Closing port on resource error: " << m_serialPort.portName();
                m_commConfigDlg.togglePort();
            }
            m_commConfigDlg.loadPortList();
            ui->actionStartDetect->setEnabled(true);
            ui->actionStopDetect->setEnabled(false);
            m_bPortIsOpened = false;
            break;
        case QSerialPort::DeviceNotFoundError:
            qCritical() << "Device doesn't exists: " << m_serialPort.portName();
            ui->actionStartDetect->setEnabled(true);
            ui->actionStopDetect->setEnabled(false);
            m_bPortIsOpened = false;
            break;
        case QSerialPort::PermissionError:
            qCritical() << m_serialPort.portName()<< "Permission denied. Either you don't have \
required privileges or device is already opened by another process.";
            break;
        case QSerialPort::OpenError:
            qWarning() << m_serialPort.portName()<< "Device is already opened!";
            break;
        case QSerialPort::NotOpenError:
            qCritical()<< m_serialPort.portName() << "Device is not open!";
            break;
        case QSerialPort::ParityError:
            qCritical() << "Parity error detected.";
            break;
        case QSerialPort::FramingError:
            qCritical() << "Framing error detected.";
            break;
        case QSerialPort::BreakConditionError:
            qCritical() << "Break condition is detected.";
            break;
        case QSerialPort::WriteError:
            qCritical() << "An error occurred while writing data.";
            break;
        case QSerialPort::ReadError:
            qCritical() << "An error occurred while reading data.";
            break;
        case QSerialPort::UnsupportedOperationError:
            qCritical() << "Operation is not supported.";
            break;
        case QSerialPort::TimeoutError:
            qCritical() << "A timeout error occurred.";
            break;
        case QSerialPort::UnknownError:
            qCritical() << "Unknown error!";
            break;
        default:
            qCritical() << "Unhandled port error: " << error;
            break;
    }
}

void MainWindow::messageHandler(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msg)
{
    QString logString;

    switch (type)
    {
        case QtDebugMsg:
            logString = "[Debug] " + msg;
            break;
        case QtWarningMsg:
            logString = "[Warning] " + msg;
            break;
        case QtCriticalMsg:
            logString = "[Error] " + msg;
            break;
        case QtFatalMsg:
            logString = "[Fatal] " + msg;
            break;
        default:
            logString = "[UnError] " + msg;
            break;

    }
   std::cerr << logString.toStdString() << std::endl;
    if (type != QtDebugMsg && ui != NULL)
    {
        ui->statusBar->showMessage(msg, 5000);
    }
}

void MainWindow::onOpenDBtriggered()
{
    QStringList databaseList;
    user_data userdata = get_user_data();
    if(userdata.userName=="administrator"){
        databaseList = getAllLabDB(userdata.userName);
    }else{

        databaseList = getHasAuthorize("administrator",userdata.userName);
    }

    bool ok;
    QString databaseItem=QInputDialog::getItem(this,tr("打开数据库"),tr("请选择打开数据库："),
                                         databaseList,0,false,&ok);
    if(ok&&!databaseItem.isEmpty()){
        if(userdata.dbName != databaseItem){
            QSqlDatabase::database(userdata.dbPathName).close();
            QSqlDatabase::removeDatabase(userdata.dbPathName);
            if(OpenDatabase(databaseItem)){
                reloadDatabase();
            }

        }
    }
}


bool MainWindow::OpenDatabase(const QString &dbname)
{
    QString path_db = documentsDir() + dbname + ".db";
    if(!connectDB(path_db,"","")){
        QSqlDatabase::database(path_db).close();
        QSqlDatabase::removeDatabase(path_db);
        return false;
    }
    m_currentUserData.dbPathName = path_db;
    m_currentUserData.dbName = dbname;
    set_user_data(m_currentUserData);
    changeWindowTitle(m_currentUserData.userName,m_currentUserData.dbName);
    return true;
}


void MainWindow::reloadDatabase()
{
    m_listPlotInfo.clear();
    m_listSampleInfo.clear();
    m_itemPlotInfo.clear();
    m_itemSampleInfo.clear();
    ui->listPlotWidget->clear();
//    ui->listSampleWidget->clear();

    InitDataBase();

}


void MainWindow::changeWindowTitle(const QString &username, const QString &dbname)
{
    QString strwindowtitle;
    strwindowtitle = "基于磁弹性传感器凝血功能检测平台(重大)V1.2.3   当前操作员："+username+"  当前数据库："+dbname;
    setWindowTitle(strwindowtitle);
}

void MainWindow::onNewCreateDBtriggered()
{
    QString str_admin = "administrator";
    if(m_currentUserData.userName!=str_admin){
        bool ok;
        QString adminpassword=QInputDialog::getText(this,tr("新建数据库"),tr("请输入管理员密码："),
                                           QLineEdit::Password,"",&ok);
        if(ok&&!adminpassword.isEmpty()){
            if(!checkAdminPassword(str_admin,adminpassword)){
                QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("系统管理员密码错误！"));
                return;
            }
        }else{
            return ;
        }
    }
    bool ok;
    QString dbname=QInputDialog::getText(this,tr("新建数据库"),tr("请输入数据库名称："),
                                       QLineEdit::Normal,"",&ok);
    if(ok&&!dbname.isEmpty()){
        QString path_db = documentsDir()+dbname+".db";
        if(!connectDB(path_db,"","")){
            QSqlDatabase::database(path_db).close();
            QSqlDatabase::removeDatabase(path_db);
            QMessageBox::information(0, "重庆大学血栓弹力图仪",QObject::tr("很遗憾，无法创建该数据库"));
            return;
        }

        if(!addLabDBRecord(str_admin,dbname)||!createOperatorDB(dbname)){
            QMessageBox::information(0, "重庆大学血栓弹力图仪",QObject::tr("很遗憾，无法创建该数据库"));
            return;
        }else{
            QMessageBox::information(0, "重庆大学血栓弹力图仪",QObject::tr("恭喜你，已经成功创建数据库！"));
        }
    }
}


void MainWindow::onAuthorizeDBtriggered()
{
    QString str_admin = "administrator";
    QString str_operator_id=m_currentUserData.userName;
    QString str_labdb_name;
    if(m_currentUserData.userName!=str_admin){
        bool ok;
        QString adminpassword=QInputDialog::getText(this,tr("授权数据库"),tr("请输入管理员密码："),
                                           QLineEdit::Password,"",&ok);
        if(ok&&!adminpassword.isEmpty()){
            if(!checkAdminPassword(str_admin,adminpassword)){
                QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("系统管理员密码错误"));
                return;
            }
        }else{
            return;
        }
    }


    if(m_currentUserData.userName==str_admin){
        QStringList usernameList;
        usernameList = getAllOperatorID(str_admin);
        bool ok;
        QString username=QInputDialog::getItem(this,tr("授权操作员"),tr("请选择要授权操作员："),
                                             usernameList,0,false,&ok);
        if(ok&&!username.isEmpty()){
            str_operator_id = username;
        }else{
            return;
        }
    }

    QStringList databaseList;
    databaseList = getAllLabDB(str_admin);



    bool ok;
   str_labdb_name = QInputDialog::getItem(this,tr("授权数据库"),tr("请选择要授权数据库："),
                                         databaseList,0,false,&ok);
    if(ok&&!str_labdb_name.isEmpty()){
        if(addAuthorizeRecord(str_admin,str_operator_id,str_labdb_name)){
            QMessageBox::information(0, "磁弹性传感器检测平台",QObject::tr("恭喜你，已经成功完成授权！"));
        }else{
            QMessageBox::information(0, "磁弹性传感器检测平台",QObject::tr("很遗憾，未能完成授权！"));
        }
    }else{
        QMessageBox::information(0, "磁弹性传感器检测平台",QObject::tr("很遗憾，未能完成授权！"));
    }

}


void MainWindow::onSystemSetupTriggered()
{
    if(m_systemSetupDlg.exec() == QDialog::Accepted){

    }
}


void MainWindow::WriteComSetSetting()
{
    stParaInfo stpara;
    stpara = m_commConfigDlg.GetParaInfo();
    QSettings setting("ThromboElastoGram",m_currentUserData.userName);
    setting.setValue("start_fre",stpara.iStartFre);
    setting.setValue("stop_fre",stpara.iStopFre);
    setting.setValue("step_fre",stpara.iStepFre);
    setting.setValue("step_interival_time",stpara.iStepInterivalTime);
    setting.setValue("scan_numbers",stpara.iScanNumbers);
    setting.setValue("mutil_scan_interival_time",stpara.iMutilScanInterivalTime);
    setting.setValue("optinal_dc",stpara.iOptionalDC);
    setting.setValue("optinal_ac",stpara.iOptionalAC);
}


void MainWindow::onViewOriginData(QString test_case_id)
{
    IsShowViewOriginData();

    m_viewOriginDataPlot.detachItems(QwtPlotItem::Rtti_PlotCurve);   
    stParaInfo stparainfo = get_para_info(test_case_id);
    m_testParaForm.setTestPara(stparainfo);

    QString freString;   //数据库中存储频率的字符串链表
    QString ampString;   //存储幅值的字符串链表
    QString phaseString;  //存储相位的字符串链表

    freString = get_origin_fre_data(test_case_id);
    ampString = get_origin_amp_data(test_case_id);
    phaseString = get_origin_phase_data(test_case_id);


    QStringList fre_str_list = freString.split("\n");
    QStringList amp_str_list = ampString.split("\n");
    QStringList phase_str_list = phaseString.split("\n");


    int numofscan = amp_str_list.size();
    int numofFre = fre_str_list.size();

    m_viewOriginDataPlot.setXaxis(fre_str_list.at(0).toDouble(),fre_str_list.at(numofFre-1).toDouble());

    QList< QVector<QPointF> > listAmpPoint;   //采集的幅值点信息
    QList< QVector<QPointF> > listPhasePoint;  //采集相位点信息
    QList< QVector<QPointF> > listRealPoint;  //采集阻抗实部信息

    QList<QwtPlotCurve*> listAmpCurves;          //幅值的曲线
    QList<QwtPlotCurve*> listPhaseCurves;        //相位的曲线
    QList<QwtPlotCurve*> listRealCurves;        //阻抗实部的曲线
    for(int ci = 0; ci < numofscan; ci++){
        QVector<QPointF> ampPoint;
        QVector<QPointF> phasePoint;
        QVector<QPointF> realpoint;
        QStringList one_amp_str_list = amp_str_list.at(ci).split(",");
        QStringList one_phase_str_list = phase_str_list.at(ci).split(",");
        for(int i = 0; i < numofFre; i++){
            QPointF amptempPoint(fre_str_list.at(i).toDouble(),one_amp_str_list.at(i).toDouble());
            QPointF phasetempPoint(fre_str_list.at(i).toDouble(),one_phase_str_list.at(i).toDouble());;
            ampPoint.append(amptempPoint);
            phasePoint.append(phasetempPoint);

            double realvalue = one_amp_str_list.at(i).toDouble()*cos(one_phase_str_list.at(i).toDouble());
            QPointF amprealPoint(fre_str_list.at(i).toDouble(),realvalue);
            realpoint.append(amprealPoint);
        }

        listAmpPoint.append(ampPoint);
        listPhasePoint.append(phasePoint);
        listRealPoint.append(realpoint);


        listAmpCurves.append(new QwtPlotCurve());
        listAmpCurves.last()->setSamples(ampPoint);
        listAmpCurves.last()->setPen(Qt::red,1.0,Qt::SolidLine);
        listAmpCurves.last()->attach(&m_viewOriginDataPlot);

        listPhaseCurves.append(new QwtPlotCurve());
        listPhaseCurves.last()->setSamples(phasePoint);
        listPhaseCurves.last()->setPen(Qt::green,1.0,Qt::SolidLine);
        listPhaseCurves.last()->attach(&m_viewOriginDataPlot);

        listRealCurves.append(new QwtPlotCurve());
        listRealCurves.last()->setSamples(realpoint);
        listRealCurves.last()->setPen(Qt::yellow,1.0,Qt::SolidLine);
        listRealCurves.last()->attach(&m_viewOriginDataPlot);

        m_viewOriginDataPlot.replot();;
    }
}

void MainWindow::onOriginPhaseViewTriggered(bool checked)
{
    if(checked){
        ui->actionTegDataView->setChecked(false);
        m_viewtype = OriginPhaseType;
    }else{
        ui->actionTegDataView->setChecked(true);
        m_viewtype = TegDataType;
    }
    SetCruvesView(m_viewtype);
}


void MainWindow::onTegDataViewTriggered(bool checked)
{
    if(checked){
        ui->actionOriginDataView->setChecked(false);
        m_viewtype = TegDataType;

    }else{
        ui->actionOriginDataView->setChecked(true);
        m_viewtype = OriginPhaseType;

    }
    SetCruvesView(m_viewtype);
}

void MainWindow::SetCruvesView(int viewtype)
{
    unsigned int recordNum = m_listPlotInfo.size();
    for(unsigned int i=0; i<recordNum; i++){
        m_listPlotInfo[i]->SetCurveDataView(viewtype);
    }
}

void MainWindow::onSetTimeLineTriggered()
{
    bool ok;
    QString timeline=QInputDialog::getText(this,tr("修改坐标轴"),tr("请输入显示坐标轴长度："),
                                       QLineEdit::Normal,"",&ok);
    if(!ok&&timeline.isEmpty()){
        QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("输入长度有误！"));
        return;
    }

    unsigned int recordNum = m_listPlotInfo.size();
    for(unsigned int i=0; i<recordNum; i++){
        m_listPlotInfo[i]->SetTimeLine(timeline.toInt(0,10));
    }

}

void MainWindow::onImportDatabaseTriggered()
{
    QString str_admin = "administrator";
    QString str_operator_id=m_currentUserData.userName;
    QString str_labdb_name;
    if(m_currentUserData.userName!=str_admin){
        bool ok;
        QString adminpassword=QInputDialog::getText(this,tr("导入数据库"),tr("请输入管理员密码："),
                                           QLineEdit::Password,"",&ok);
        if(ok&&!adminpassword.isEmpty()){
            if(!checkAdminPassword(str_admin,adminpassword)){
                QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("系统管理员密码错误"));
                return;
            }
        }else{
            return;
        }
    }

    QStringList listfiles = QFileDialog::getOpenFileNames(this,tr("load database"),QDir::currentPath(),"*.db");

    for(QString filename : listfiles){
        qDebug()<<"filename:"<<filename<<QDir::currentPath();
        QString strnewfile = QDir::currentPath()+"/ThromboElastoGram/"+QFileInfo(filename).fileName();
        if(QFile(strnewfile).exists()){
            QMessageBox msgBox;
            msgBox.setText("'"+QFileInfo(filename).fileName()+"' 系统已经存在该数据库"
                                                              "\n请修改数据库名称再导入");
            msgBox.exec();
            return ;

        }
        if(!QFile::copy(filename,QDir::currentPath()+"/ThromboElastoGram/"+QFileInfo(filename).fileName()))
        {
            QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("导入数据库失败！"));
            return;
        }

        QString dbname = QFileInfo(filename).baseName();
        if(!addLabDBRecord(str_admin,dbname)){
            QMessageBox::information(0, "重庆大学血栓弹力图仪",QObject::tr("很遗憾，无法创建该数据库"));
            return;
        }else{
            QMessageBox::information(0, "重庆大学血栓弹力图仪",QObject::tr("恭喜你，已经成功创建数据库！"));
        }

    }


}

void MainWindow::onExportDatabaseTriggered()
{
    QString str_admin = "administrator";
    if(m_currentUserData.userName!=str_admin){
        bool ok;
        QString adminpassword=QInputDialog::getText(this,tr("导入数据库"),tr("请输入管理员密码："),
                                           QLineEdit::Password,"",&ok);
        if(ok&&!adminpassword.isEmpty()){
            if(!checkAdminPassword(str_admin,adminpassword)){
                QMessageBox::critical(0,"重庆大学血栓弹力图仪",QObject::tr("系统管理员密码错误"));
                return;
            }
        }else{
            return;
        }
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export database File"),m_currentUserData.dbName+".db",
                                                    tr("(*.db)"));

    QFile::copy(m_currentUserData.dbPathName,fileName);

}

void MainWindow::msleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QString MainWindow::FormatDocToPrint()
{
    QString sret = "<html><head>";
    sret += "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">";
    sret +=	"</head><title>";
//    QString titlename = tr("磁弹性传感器检测装置报告单");
//    sret += titlename;
    sret += "</title>\n</head>\n";


    sret += "<body>\n<center>\n";
    sret += "<table width=\"800px\" border=\"0px\">\n<tr><td align=\"left\"><h3>基于磁弹性传感器凝血功能检测分析报告</h3></td>\n<td rowspan=\"2\" align=\"right\"><b></b></td>\n</tr>\n";
    sret += "<tr><td align=\"left\">报告生成：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>2017/3/3&nbsp;21:48:17</b></td>\n</tr>\n</table>\n";
    sret += "<table width=\"800px\" BORDER=1>\n";
    sret += "<tr><td width=\"400px\" colspan=\"2\"><span>患者：张三;<b></b></span><br/>\n<span>ID:&nbsp;H1888888888&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span><br/>\n";
    sret += "<span>生日:1990.12.23;年龄：26;性别：男</span><br/></td>\n";
    sret += "<td align=\"left\" colspan=\"2\"><span>样本日期：&nbsp;<b>2016/12/14</b></span><br/><span>开始时间：&nbsp;<b>14:27</b></span><br/><span>结束时间：&nbsp;<b>14:47</b></span><br/><span>总共用时：&nbsp;<b>20min</b></span><br/></td>";
    sret += "</tr><tr><td colspan=\"2\"><span >样本类型： <b>Citrated kaolin</b></span><br/><span>描述：</span><br/></td><td rowspan=\"2\" colspan=\"2\"><span>地点<b>ID:</b></span><br/><span>操作者<b>ID:LI19930102</b></span><br/></td></tr>\n";
    sret += "<tr><td colspan=\"2\"><span>备注：</span><br/></td></tr>\n";
    QString filename = QDir::currentPath()+"originimage.png";
    QwtPlotRenderer renderer;
    QListWidgetItem *currentitem = ui->listPlotWidget->currentItem();
    PlotInfo *plotinfo = (PlotInfo *)ui->listPlotWidget->itemWidget(currentitem);
    renderer.renderDocument(plotinfo->GetTegPlot(),filename,QSizeF(180,80),85);

    sret += "<tr><td colspan=\"4\" align=\"center\"> <img src="+filename+" alt=\"TEG\"/>\n<center>\n";

    sret += "<tr><td BGCOLOR=\"#ffaa00\">"+tr("R值")+"</td><td BGCOLOR=\"#55ff7f\">"+tr("K值")+"</td><td BGCOLOR=\"#0055ff\">"
                +tr("Angle值")+"</td><td BGCOLOR=\"#aa007f\">"+tr("MA值")+"</td></tr>";
    sret += "<tr><td BGCOLOR=\"#ffaa00\">"+tr("min")+"</td><td BGCOLOR=\"#55ff7f\">"+tr("min")+"</td><td BGCOLOR=\"#0055ff\">"
                +tr("deg")+"</td><td BGCOLOR=\"#aa007f\">"+tr("V")+"</td></tr>";
    QString Rvalue = QString::number(plotinfo->GetRvaluePoint().x()/60,'f',2);
    QString Kvalue = QString::number(plotinfo->GetKvaluePoint().x()/60-plotinfo->GetRvaluePoint().x()/60,'f',2);
    QString MAvalue = QString::number(plotinfo->GetMAvaluePoint().y(),'f',3);
    QString Anglevalue = QString::number(plotinfo->GetAngleValue(),'f',2);
    sret += "<tr><td BGCOLOR=\"#ffaa00\">"+Rvalue+"</td><td BGCOLOR=\"#55ff7f\" >"+Kvalue+"</td><td  BGCOLOR=\"#0055ff\">"
            +Anglevalue+"</td><td BGCOLOR=\"#aa007f\">"+MAvalue+"</td></tr>";



    sret += " <tr> <td border-right=\"0px\" heigth=\"300px\" colspan=\"2\" >报告结果：</td> <td colspan=\"2\"> <span>报告日期：</span><br/><span>报告者：</span><br/><b>Physician interpretation</b></td></tr>\n</table></center>\n";
    sret += "</body></html>";

    return sret;

}

void MainWindow::onPrintReportTriggered()
{
    if(!ui->listPlotWidget->currentItem()) return;
    QString s = FormatDocToPrint();
    PrintDialog *dlg = new PrintDialog(s,this);
    dlg->exec();
    delete dlg;
}

/*
参数说明
originData表示要进行平滑滤波的原始数据
N表示平滑的窗口大小
返回值为经过平滑滤波处理后的数据
*/

QVector<QPointF> MainWindow::SmoothFilter(QVector<QPointF> originData, int N)
{
    QVector<QPointF> resultData;
    int origin_data_size = originData.size();
    int temp;
    for(unsigned int ci = 0; ci < origin_data_size-N; ci=ci+N){
        double sum = 0 ;
        for(int j = 0; j < N-1; j++)
            for(int i = 0; i< N-j;i++){
                if(originData[ci+i].y()>originData[ci+i+1].y()){
                    temp = originData[ci+i].y();
                    originData[ci+i].setY(originData[ci+i+1].y());
                    originData[ci+i+1].setY(temp);
                }
            }
        for(int count = ci ; count<ci+N-1; count++){
            sum+=originData[count].y();
        }
        double ave =(double) sum/(N-2);

        resultData.append(QPointF(originData[ci].x(),ave));
    }
    resultData.append(originData[origin_data_size-1]);
    return resultData;
}

void MainWindow::on_action_MergeData_triggered()
{
    
       if(m_selectMergeDlg.exec()==QDialog::Accepted){
           QStringList list = m_selectMergeDlg.GetMergeString();
           if(!list.isEmpty()){
               detailedSampleForm.SetMergeCurveData(list);
               IsShowDetailSample();
           }
       }

}



void MainWindow::on_action_Sensor_Property_triggered()
{
    if(ui->action_Sensor_Property->isChecked()){
        m_twParaSet.hide();
    }else{
        m_twParaSet.show();
    }
}

void MainWindow::on_action_OptionalDC_triggered()
{

}

void MainWindow::on_actionHDBExchange_triggered()
{
    HDBExchangeDlg *hdbExchangedlg = new HDBExchangeDlg();

    hdbExchangedlg->show();
}


void MainWindow::OptimalDCBiasScan(int istartDC, int istopDC, int istepDC)
{
    if(!m_listAmpPoint.isEmpty())
    {
        ClearAllCurves();
        m_OriginPlot.replot();
    }
    iStartDC = istartDC;
    iStopDC = istopDC;
    iStepDC = istepDC;
    iDCScanNum = 0;

    isOptimalDCBiasFlag = true;
    sendOptimalDCPara(istartDC);
    ui->actionStopDetect->setEnabled(true);
    ui->actionStartDetect->setEnabled(false);

//    for(unsigned int currentDCvalue = istartDC; currentDCvalue<=istopDC; currentDCvalue = currentDCvalue+istepDC){
//        sendOptimalDCPara(currentDCvalue);
//        //以相位的共振幅值来判断其
//        QPointF DCAmpPoint(currentDCvalue,m_vecPhaseResPoint.last().ry());
//        optimalDCBiasForm.drawOptimalDCBiasCurve(DCAmpPoint);

//    }
//    ui->actionStopDetect->setEnabled(false);
//    ui->actionStartDetect->setEnabled(true);

}


void MainWindow::sendOptimalDCPara(int DCValue)
{

    qDebug()<<m_bPortIsOpened;

    if(!m_bPortIsOpened){
        m_commConfigDlg.togglePort();
        //判断串口是否能使用
        if(m_serialPort.isOpen()){
            m_stParaInfo = m_commConfigDlg.GetParaInfo();
            m_stParaInfo.iOptionalDC = DCValue;
            m_stParaInfo.iScanNumbers = 1;
            SendStartCommand(m_stParaInfo);
            m_bPortIsOpened = true;
        }
        m_serialPort.waitForReadyRead(1000);
    }

}

void MainWindow::OptimalACBiasScan(int istartAC, int istopAC, int istepAC)
{
    if(!m_listAmpPoint.isEmpty())
    {
        ClearAllCurves();
        m_OriginPlot.replot();
    }
    iStartAC = istartAC;
    iStopAC = istopAC;
    iStepAC = istepAC;
    iACScanNum = 0;

    isOptimalACBiasFlag = true;
    sendOptimalACPara(istartAC);
    ui->actionStopDetect->setEnabled(true);
    ui->actionStartDetect->setEnabled(false);
}

void MainWindow::sendOptimalACPara(int ACValue)
{

    qDebug()<<m_bPortIsOpened;

    if(!m_bPortIsOpened){
        m_commConfigDlg.togglePort();
        //判断串口是否能使用
        if(m_serialPort.isOpen()){
            m_stParaInfo = m_commConfigDlg.GetParaInfo();
            m_stParaInfo.iOptionalAC = ACValue;
            m_stParaInfo.iScanNumbers = 1;
            SendStartCommand(m_stParaInfo);
            m_bPortIsOpened = true;
        }
        m_serialPort.waitForReadyRead(1000);
    }

}
