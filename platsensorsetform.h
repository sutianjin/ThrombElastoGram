#ifndef PLATSENSORSETFORM_H
#define PLATSENSORSETFORM_H

#include <QWidget>
#include <QtSerialPort>
#include "portlist.h"

namespace Ui {
class PlatSensorSetForm;
}

class PlatSensorSetForm : public QWidget
{
    Q_OBJECT

public:
    explicit PlatSensorSetForm(QSerialPort* port,QWidget *parent = 0);
    ~PlatSensorSetForm();

    QSerialPort* serialPort;
    QString getSensorType();
    QString getSensorLength();
    QString getSensorWidth();
    QString getSensorThick();

public slots:
    void loadPortList();
    void loadBaudRateList();
    void togglePort();
    void selectPort(QString portName);
    void selectBaudRate(QString baudRate);

private:
    Ui::PlatSensorSetForm *ui;
    PortList portList;


private slots:

signals:
   void portToggled(bool open);

};

#endif // PLATSENSORSETFORM_H
