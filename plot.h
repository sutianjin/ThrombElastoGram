#ifndef PLOT_H
#define PLOT_H
#include <qwt_plot.h>
#include <qwt_interval.h>

#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_panner.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>

#include "zoomer.h"
#include "scalezoomer.h"





class QwtPlotMarker;
class QwtPlotDirectPainter;

class Plot : public QwtPlot
{
    Q_OBJECT
public:
    Plot(QWidget * = NULL);
    virtual ~Plot();
    void insertCurve();
    void setAxis(bool autoScaled, double yMin = 0, double yMax = 1);
    void setXaxis(double xMin = 0,double XMax = 100);
    void setYaxis(double yMin = 0,double yMax = 100);
    void setXButtonTitle(QString titlename);
    void setYLeftTitle(QString titlename);

private:
    bool isAutoScaled;
    QwtInterval d_interval;
    QwtPlotMarker *d_origin;
    QwtPlotDirectPainter *d_directPainter;

    double yMin, yMax,xMin,xMax;

    Zoomer *zoomer;
    ScaleZoomer *sZoomer;

    void initZoomer();
    void resetAxes();

public slots:
    void unzoom();

private slots:
    void unzoomed();


};

#endif // PLOT_H
