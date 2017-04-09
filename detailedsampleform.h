#ifndef DETAILEDSAMPLEFORM_H
#define DETAILEDSAMPLEFORM_H

#include <QWidget>
#include "qwt_plot_curve.h"
#include "qwt_plot_marker.h"
#include <qwt_curve_fitter.h>
#include <qwt_legend.h>
#include <qwt_plot_textlabel.h>
#include <qwt_legend_label.h>

namespace Ui {
class DetailedSampleForm;
}

class DetailedSampleForm : public QWidget
{
    Q_OBJECT

public:
    explicit DetailedSampleForm(QWidget *parent = 0);
    ~DetailedSampleForm();

    void SetCurveData(QVector<QPointF> phasepoint);
    void SetTegCurveData(QVector<QPointF> phaseResPoint);
    void SetPatientName(QString name);
    void SetSampleType(QString type);
    void SetDetectionTime(QString time);

    void SetCurveDataView(int curvetype);

    void SetTimelineLenght(int lenght);
    
    void SetMergeCurveData(QStringList list);
    void SetTegMerge(QVector<QPointF> phaseResPoint);

    //求差分最大值点
    int MaxOfDiff(QVector<QPointF> phasepoint);

    //求斜率
    QPointF linefunc(QPointF point1, QPointF point2);

    double getlineX(QPointF lineSlope, double yValue);

private:
    Ui::DetailedSampleForm *ui;
    QwtPlotCurve m_qwtPhaseCurve;

    QwtSplineCurveFitter *curveFitter;
    QwtWeedingCurveFitter *weedFitter;

    QPointF RValuePoint;
    QPointF KValuePoint;
    QPointF MAValuePoint;
    QPointF AngleValuePoint;

    QwtPlotCurve Teg_Time_Curve;
    QwtPlotCurve Arc_Teg_Time_Curve;
    QwtPlotCurve Teg_Fitter_Time_Curve;

    QwtPlotMarker RvalueMarkers;
    QwtPlotMarker MAvalueMarkers;
    QwtPlotMarker KvalueMarkers;
    QwtPlotCurve AngleCruve;
    
    
    QList< QVector<QPointF> > m_listMergePoint;   //采集的幅值点信息
    QList< QVector<QPointF> > m_listArcMergeePoint;  //采集相位点信息
    
    QList<QwtPlotCurve*> list_Teg_Time_Merge_Curve;
    QList<QwtPlotCurve*> list_Arc_Teg_Time_Curve;
    QList<QwtLegend*> list_legend;
    QList<QwtLegendLabel*> list_label;



private Q_SLOTS:
    void showItem(const QVariant &itemInfo, bool on);//点击图例，显示相应的曲线
};

#endif // DETAILEDSAMPLEFORM_H
