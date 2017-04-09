#ifndef SHOWLISTCURVEFORM_H
#define SHOWLISTCURVEFORM_H

#include <QWidget>
#include "qwt_plot_curve.h"
#include <qwt_plot_marker.h>


namespace Ui {
class ShowListCurveForm;
}

class ShowListCurveForm : public QWidget
{
    Q_OBJECT

public:
    explicit ShowListCurveForm(QWidget *parent = 0);
    ~ShowListCurveForm();
    void getOriginData(QList<QVector<QPointF >> *listAmpPoint,
                       QList<QVector<QPointF >> *listPhasePoint);
    void getDetectPara(QStringList listPara);
    void showClotingCurve();

private:
    Ui::ShowListCurveForm *ui;
    QStringList _listPara;
    int iStartFre,iStopFre,iInterivalFre,iDCData,iACData,
            iScanNum,iScanInterival,iMutilScanInterival;
    QList<QVector<QPointF >> _listAmpPoint;
    QList<QVector<QPointF >> _listPhasePoint;
    QList<QwtPlotCurve*> listShowAmpCurves;          //幅值的曲线
    QList<QwtPlotCurve*> listShowPhaseCurves;        //相位的曲线
    QList<QwtPlotMarker*> listAmpResHighMarkers;
    QList<QwtPlotMarker*> listAmpResLowMarkers;
    QList<QwtPlotMarker*> listPhaseResMarkers;
    QList<QPointF> AmpResPoint;            //存储共振点的幅值
    QList<QPointF> PhaseResPoint;           //存储共振点的相位

    QwtPlotCurve *AmpResCurve;
    QwtPlotCurve *PhaseResCurve;

    QVector<QPointF > AmpAndTimePoint;
    QVector<QPointF > PhaseAndTimePoint;

    QwtPlotCurve *FreResCurve;
    QVector<QPointF > FreAndTimePoint;


};

#endif // SHOWLISTCURVEFORM_H
