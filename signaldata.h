#ifndef SIGNALDATA_H
#define SIGNALDATA_H

#include <qwt_series_data.h>
#include <QPointF>
#include <QRectF>

class SignalData : public QwtSeriesData<QPointF>
{
public:
    SignalData();
    SignalData( const SignalData & );
    SignalData &operator=( const SignalData & );
    ~SignalData();

    QPointF sample( size_t i ) const;
    size_t size() const;
    QRectF boundingRect() const;

    void append( const QPointF &pos );
    void clearStaleValues( double min );
    void lock();
    void unlock();
    void clear();
    void resize(size_t size);

private:

    class PrivateData;
    PrivateData *d_data;

};

#endif // SIGNALDATA_H
