#ifndef PLOTINFO_H
#define PLOTINFO_H

#include <QWidget>
#include "qwt_plot_curve.h"
#include "qwt_plot_marker.h"
#include "plot.h"

namespace Ui {
class PlotInfo;
}

class PlotInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PlotInfo(QWidget *parent = 0);
    ~PlotInfo();
    void SetTestSampleID(QString strTestSampleID);
    void SetPatientName(QString strPatientName);
    void SetStartTestTime(QString strStartTime);
    void SetEndTestTime(QString strEndTime);
    void SetCurveDataView(int curvetype);
    void SetCurveData(QVector<QPointF> phaseResPoint);
    void SetTegCurveData(QVector<QPointF> phaseResPoint);
    void SetTimeLine(int timelinelenght);
    void SetAmplitudeRange(double minvalue,double maxvalue);
    QString GetTestSampleID();
    QString GetPatientName();
    QString GetSampleType();
    QString GetDetectionTime();
    int GetTimelineLenght();
    Plot* GetTegPlot();

    QVector<QPointF> m_vecPhasePoint;
    QVector<QPointF> m_VecTegPoint;
    QVector<QPointF> m_ArcVecTegPoint;

    QPointF GetRvaluePoint();
    QPointF GetKvaluePoint();
    QPointF GetMAvaluePoint();
    double GetAngleValue();

    //求斜率
    QPointF linefunc(QPointF point1, QPointF point2);
    double getlineX(QPointF lineSlope, double yValue);


public slots:
    void emitViewOriginDataSignal();
    void onSetTimelineTriggered();
    void onExportOriginDataTriggered();
    void onExportPhaseTimeDataTriggered();
    void onSetAmplitudeRangeTriggered();


private:
    Ui::PlotInfo *ui;
    QwtPlotCurve m_qwtPhaseCurve;
    QAction *actionViewOriginData;
    QAction *actionSetTimeline;
    QAction *actionExportOriginData;
    QAction *actionExportPhaseTimeData;
    QAction *actionSetAmplitudeRange;

    QPointF RValuePoint;
    QPointF KValuePoint;
    QPointF MAValuePoint;
    QPointF AngleValuePoint;
    double angleValue;

    QwtPlotCurve Teg_Time_Curve;
    QwtPlotCurve Arc_Teg_Time_Curve;

    QwtPlotMarker RvalueMarkers;
    QwtPlotMarker MAvalueMarkers;
    QwtPlotMarker KvalueMarkers;
    QwtPlotCurve AngleCruve;

    int m_timeline;
    double ymaxvalue;
    double yminvalue;

signals:
    void viewOriginData(QString test_case_id);

};

#endif // PLOTINFO_H
