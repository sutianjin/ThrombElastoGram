#include "plot.h"
#include <qwt_plot_canvas.h>
#include <qwt_painter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_directpainter.h>
#include <qwt_plot_curve.h>
#include <qwt_picker_machine.h>

class Canvas: public QwtPlotCanvas             //设置画布
{
public:
    Canvas( QwtPlot *plot = NULL ):
        QwtPlotCanvas( plot )
    {
        // The backing store is important, when working with widget
        // overlays ( f.e rubberbands for zooming ).
        // Here we don't have them and the internal
        // backing store of QWidget is good enough.

        setPaintAttribute( QwtPlotCanvas::BackingStore, false );
        setBorderRadius( 10 );

        if ( QwtPainter::isX11GraphicsSystem() )
        {
#if QT_VERSION < 0x050000
            // Even if not liked by the Qt development, Qt::WA_PaintOutsidePaintEvent
            // works on X11. This has a nice effect on the performance.

            setAttribute( Qt::WA_PaintOutsidePaintEvent, true );
#endif

            // Disabling the backing store of Qt improves the performance
            // for the direct painter even more, but the canvas becomes
            // a native window of the window system, receiving paint events
            // for resize and expose operations. Those might be expensive
            // when there are many points and the backing store of
            // the canvas is disabled. So in this application
            // we better don't disable both backing stores.

            if ( testPaintAttribute( QwtPlotCanvas::BackingStore ) )
            {
                setAttribute( Qt::WA_PaintOnScreen, true );
                setAttribute( Qt::WA_NoSystemBackground, true );
            }
        }

        setupPalette();
    }

private:
    void setupPalette()
    {
        QPalette pal = palette();

#if QT_VERSION >= 0x040400
        //改变背景颜色
        QLinearGradient gradient;
        gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
        gradient.setColorAt( 0.0, QColor( 0, 0, 0 ) );
        gradient.setColorAt( 1.0, QColor( 0, 0, 0 ) );

        pal.setBrush( QPalette::Window, QBrush( gradient ) );
#else
        pal.setBrush( QPalette::Window, QBrush( color ) );
#endif

        // QPalette::WindowText is used for the curve color
        pal.setColor( QPalette::WindowText, Qt::green );

        setPalette( pal );
    }
};



Plot::Plot(QWidget *parent):
    QwtPlot(parent)
{
    isAutoScaled = true;

    d_directPainter = new QwtPlotDirectPainter();
    setAutoReplot( false );
    setCanvas(new Canvas());
    plotLayout()->setAlignCanvasToScales( true );

    zoomer = new Zoomer(this->canvas(),false);
    sZoomer = new ScaleZoomer(this,zoomer);


    QObject::connect(zoomer, &Zoomer::unzoomed, this, &Plot::unzoomed);
    zoomer->setZoomBase();

 //   setAxisTitle( QwtPlot::xBottom, tr("频率[kHz]") );
   // setAxisScale( QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue(),d_interval.width()/10 );
 //   setAxisTitle( QwtPlot::yLeft, tr("幅值[mV]") );
  //  setAxisScale( QwtPlot::yLeft, 0, 5.0 );

   // setTitle(tr("共振曲线"));
 //   enableAxis(QwtPlot::yLeft,false);
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( false );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );

    d_origin = new QwtPlotMarker();
    d_origin->setLineStyle( QwtPlotMarker::Cross );
    d_origin->setValue( d_interval.minValue() + d_interval.width() / 2.0, 2000.0 );
    d_origin->setLinePen( Qt::gray, 0.0, Qt::DashLine );
    d_origin->attach( this );
    initZoomer();


}

Plot::~Plot()
{
    delete d_directPainter;
    delete zoomer;
    delete sZoomer;
}


void Plot::insertCurve()
{
    static int counter = 1;

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
    const int numColors = sizeof( colors ) / sizeof( colors[0] );

    QwtPlotCurve *Ampcurve = new QwtPlotCurve();
    QwtPlotCurve *Phasecurve = new QwtPlotCurve();
    Ampcurve->setPen( QColor( colors[ counter % numColors ] ), 2 );
    Ampcurve->attach( this );
    Phasecurve->setPen( QColor( colors[ counter % numColors ] ), 2 );
    Phasecurve->attach( this );
}

void Plot::initZoomer()
{

    zoomer->setRubberBand( QwtPicker::RectRubberBand );
    zoomer->setRubberBandPen( QColor( Qt::green ) );
    zoomer->setTrackerMode( QwtPicker::ActiveOnly );
    zoomer->setTrackerPen( QColor( Qt::white ) );
    replot();
  /*  d_zoomer[1] = new Zoomer( QwtPlot::xTop, QwtPlot::yRight,
         this->canvas() );

    d_panner = new QwtPlotPanner( this->canvas() );
    d_panner->setMouseButton( Qt::MidButton );

    d_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        this->canvas() );
    d_picker->setStateMachine( new QwtPickerDragPointMachine() );
    d_picker->setRubberBandPen( QColor( Qt::green ) );
    d_picker->setRubberBand( QwtPicker::CrossRubberBand );
    d_picker->setTrackerPen( QColor( Qt::white ) );

    d_panner->setEnabled( true );

    d_zoomer[0]->setEnabled( true );
    d_zoomer[0]->zoom( 0 );

    d_zoomer[1]->setEnabled( true );
    d_zoomer[1]->zoom( 0 );

    d_picker->setEnabled( !true );

    zoomer.setRubberBandPen(QwtPicker::RectRubberBand);
    zoomer.setTrackerPen(QPen(Qt::white));
    sZoomer.setPickerPen(QPen(Qt::white));*/

}

void Plot::unzoomed()
{
    setAxisAutoScale(QwtPlot::xBottom);
  //  setAxisScaleDiv(QwtPlot::xBottom,);
    setAxisScale(QwtPlot::xBottom, xMin,xMax,(xMax-xMin)/5);
    resetAxes();
}


void Plot::unzoom()
{
    zoomer->zoom(0);
}


void Plot::setAxis(bool autoScaled, double yAxisMin, double yAxisMax)
{
    this->isAutoScaled = autoScaled;

    if (!autoScaled)
    {
        yMin = yAxisMin;
        yMax = yAxisMax;
    }

    zoomer->zoom(0);
    resetAxes();
}

void Plot::setXaxis(double xAxisMin, double xAxisMax)
{
    xMin = xAxisMin;
    xMax = xAxisMax;
    zoomer->zoom(0);
    setAxisScale(QwtPlot::xBottom, xMin,xMax,(xMax-xMin)/5);
    replot();
}

void Plot::setYaxis(double yAxisMin, double yAxisMax)
{
    yMin = yAxisMin;
    yMax = yAxisMax;
    zoomer->zoom(0);

    setAxisScale(QwtPlot::yLeft, yMin,yMax,(yMax-yMin)/5);
    replot();

}

void Plot::resetAxes()
{
    if (isAutoScaled)
    {
        setAxisAutoScale(QwtPlot::yLeft);
    }
    else
    {
        setAxisScale(QwtPlot::yLeft, yMin, yMax);
    }

    replot();
}


void Plot::setXButtonTitle(QString titlename)
{
    setAxisTitle(QwtPlot::xBottom,titlename);
}

void Plot::setYLeftTitle(QString titlename)
{
    setAxisTitle(QwtPlot::yLeft,titlename);
}



