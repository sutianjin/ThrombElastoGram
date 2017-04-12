#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QListWidget>
#include <QLabel>
#include "plotinfo.h"
#include "commconfigdlg.h"
#include "platsensorsetform.h"
#include "optimaldcbiasform.h"
#include "optimalacbiasform.h"
#include "parasetform.h"
#include "plot.h"
#include "detailedsampleform.h"
#include "newsampledlg.h"
#include "auxiliaryinfoform.h"
#include "fdatatbase.h"
#include "systemsetupdlg.h"
#include "testparaform.h"
#include "selectmergedlg.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void messageHandler(QtMsgType type, const QMessageLogContext &context,
                        const QString &msg);

private slots:
    void on_NewSample_triggered();
    void onNewPatienttriggered();
    void on_actionComSet_triggered();
    void StartDetectSignal();
    void StopDetectSignal();
    void onDataReady();
    void OnPreExperiment();
    void selectListPlotOnDoubleClicked(QListWidgetItem* item);
    void selectListPlotOnSignalClicked(QListWidgetItem* item);

    void on_actionMainWindows_triggered();

    void onPortError(QSerialPort::SerialPortError error);
    void onOpenDBtriggered();
    void onNewCreateDBtriggered();
    void onAuthorizeDBtriggered();

    void onSystemSetupTriggered();

    void onViewOriginData(QString test_case_id);
    void onOriginPhaseViewTriggered(bool checked);
    void onTegDataViewTriggered(bool checked);

    void onSetTimeLineTriggered();
    void onImportDatabaseTriggered();
    void onExportDatabaseTriggered();

    void LongTimeNoData();

    //打印
    void onPrintReportTriggered();

    void on_action_MergeData_triggered();



    void on_action_Sensor_Property_triggered();

    void on_action_OptionalDC_triggered();

    void on_actionHDBExchange_triggered();

    //最佳偏置设置
    void OptimalDCBiasScan(int istartDC,int istopDC,int istepDC);
    void OptimalACBiasScan(int istartAC,int istopAC,int istepAC);

private:
    Ui::MainWindow *ui;


    //系统初始化
    void changeWindowTitle(const QString &username,const QString &dbname);
    void InitShowCurveWindow();
    void InitDataBase();

    /*listPlotWidget的曲线信息*/
    QList<PlotInfo*> m_listPlotInfo;
    QList<QListWidgetItem*> m_itemPlotInfo;

    QList<AuxiliaryInfoForm*> m_listSampleInfo;
    QList<QListWidgetItem*> m_itemSampleInfo;


    /*串口配置*/
    QSerialPort m_serialPort;
    CommConfigDlg m_commConfigDlg;
    bool m_bPortIsOpened;

    stParaInfo m_stParaInfo;

    bool SendStartCommand(stParaInfo &stparainfo);
    bool SendStopCommand();
    void WriteComSetSetting();

    //预实验曲线

    bool m_bIsPreExperiment;
    Plot m_OriginPlot;
    QTabWidget m_twParaSet;
    PlatSensorSetForm  platSensorSetForm;
    OptimalACBiasForm  optimalACBiasForm;
    OptimalDCBiasForm optimalDCBiasForm;
    ParaSetForm paraSetForm;
    TestParaForm m_testParaForm;

    QList< QVector<QPointF> > m_listAmpPoint;   //采集的幅值点信息
    QList< QVector<QPointF> > m_listPhasePoint;  //采集相位点信息


    QList<QwtPlotCurve*> m_listAmpCurves;          //幅值的曲线
    QList<QwtPlotCurve*> m_listPhaseCurves;        //相位的曲线

    void ClearAllCurves();

    //血栓弹力图曲线

    DetailedSampleForm detailedSampleForm;

    QVector<QPointF> m_vecAmpResPoint;            //存储共振点的幅值
    QVector<QPointF> m_vecPhaseResPoint;           //存储共振点的相位
    QDateTime m_dtStartTime;

    QVector<QPointF> m_vecAmpTimePoint;
    QVector<QPointF> m_vecPhaseTimePoint;

    QString m_strResAmpValue;
    QString m_strResPhaseValue;
    QString m_strResTimestampValue;

    //接收数据处理
    bool m_bIsStartReceiveDataFlag;
    bool m_bIsStopReceiveDataFlag;
    
    bool DecideStartData(const QByteArray &startData);
    bool DecideStopData(const QByteArray &stopData);
    bool addChannelData(QByteArray &data, unsigned size);
    void ProcessReceiveErrorData();
    int m_iCurrentScanNum;

    QLabel showTempLabel;

    //界面显示设置
    void IsShowMainWindowed();
    void IsShowPreExperiment();
    void IsShowDetailSample();
    void IsShowViewOriginData();

    //数据库处理
    user_data m_currentUserData;
    bool OpenDatabase(const QString &dbname);
    void reloadDatabase();
    Plot m_viewOriginDataPlot;


    //系统文件设置
    SystemSetupDlg m_systemSetupDlg;

    //视图选择
    enum enViewType{OriginPhaseType=0,TegDataType} m_viewtype;
    void SetCruvesView(int viewtype);

    //延时函数
    void msleep(unsigned int msec);
    QTimer ProcessNoDataTimer;

    //打印功能

   QString FormatDocToPrint();

   //平滑滤波函数
   QVector<QPointF>  SmoothFilter(QVector<QPointF> originData,int N);

   //选择对比数据库窗口
  SelectMergeDlg m_selectMergeDlg;


  //最佳偏置设置
  void sendOptimalDCPara(int DCValue);
  void sendOptimalACPara(int ACValue);

  bool isOptimalDCBiasFlag;
  bool isOptimalACBiasFlag;

  int iStartDC;    //起始直流激励
  int iStopDC;      //终止的直流激励
  int iStepDC;      //步进的直流激励间隔

  int iStartAC;
  int iStopAC;
  int iStepAC;

  int iDCScanNum;
  int iACScanNum;





};

#endif // MAINWINDOW_H
