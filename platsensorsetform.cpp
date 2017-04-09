#include "platsensorsetform.h"
#include "ui_platsensorsetform.h"


#include <QSerialPortInfo>
#include "utils.h"

PlatSensorSetForm::PlatSensorSetForm(QSerialPort* port,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlatSensorSetForm)
{
    ui->setupUi(this);
    serialPort = port;
    ui->cbPortList->setModel(&portList);
    QObject::connect(ui->pbReloadPort, &QToolButton::clicked,
                     this, &PlatSensorSetForm::loadPortList);


    QObject::connect(ui->cbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PlatSensorSetForm::selectPort);
    QObject::connect(ui->cbBaudRate,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PlatSensorSetForm::selectBaudRate);
    loadPortList();
    loadBaudRateList();
    ui->cbBaudRate->setCurrentIndex(ui->cbBaudRate->findText("57600"));
}

PlatSensorSetForm::~PlatSensorSetForm()
{
    delete ui;
}

void PlatSensorSetForm::loadPortList()
{
    QString currentSelection = ui->cbPortList->currentData(PortNameRole).toString();
    portList.loadPortList();
    int index = portList.indexOf(currentSelection);
    if (index >= 0)
    {
        ui->cbPortList->setCurrentIndex(index);
    }
}

void PlatSensorSetForm::loadBaudRateList()
{
    ui->cbBaudRate->clear();

    for (auto baudRate : QSerialPortInfo::standardBaudRates())
    {
        ui->cbBaudRate->addItem(QString::number(baudRate));
    }
}

void PlatSensorSetForm::selectPort(QString portName)
{
    // 获得端口号
    portName = portName.split(" ")[0];
    // 如果串口发送变化
    if (portName != serialPort->portName())
    {
        // 就将原来的串口关闭，打开重新选择的串口
        if (serialPort->isOpen())
        {
            togglePort();
            // 打开新的串口
            togglePort();
        }
    }
}

void PlatSensorSetForm::selectBaudRate(QString baudRate)
{
    if (serialPort->isOpen())
    {
        if (!serialPort->setBaudRate(baudRate.toInt()))
        {
            qCritical() << "无法设置该波特率!";
        }
    }
}

void PlatSensorSetForm::togglePort()
{
    if (serialPort->isOpen())
    {
        serialPort->close();
        qDebug() << "Closed port:" << serialPort->portName();
        emit portToggled(false);
    }
    else
    {

        //从编辑框选择出串口号，如果有该串口则选中
        //如果没有该串口号，则添加串口号
        QString portText = ui->cbPortList->currentText();
        QString portName;
        int portIndex = portList.indexOf(portText);
        if (portIndex < 0) //不在编辑框中
        {
            portList.appendRow(new PortListItem(portText));
            ui->cbPortList->setCurrentIndex(portList.rowCount()-1);
            portName = portText;
        }
        else
        {
           //获取串口的信息
            portName = static_cast<PortListItem*>(portList.item(portIndex))->portName();
        }

        serialPort->setPortName(ui->cbPortList->currentData(PortNameRole).toString());

        // 打开串口
        if (serialPort->open(QIODevice::ReadWrite))
        {
            // 串口配置
            selectBaudRate(ui->cbBaudRate->currentText());
            serialPort->setDataBits(QSerialPort::Data8);
            serialPort->setParity(QSerialPort::NoParity);
            serialPort->setStopBits(QSerialPort::OneStop);
            serialPort->setFlowControl(QSerialPort::NoFlowControl);

            qDebug() << "Opened port:" << serialPort->portName();
            emit portToggled(true);
        }
    }
}

QString PlatSensorSetForm::getSensorType()
{
    return ui->cbSensorType->currentText();
}

QString PlatSensorSetForm::getSensorLength()
{

    return ui->leSensorLength->text();
}

QString PlatSensorSetForm::getSensorWidth()
{
    return ui->leSensorWidth->text();
}

QString PlatSensorSetForm::getSensorThick()
{
    return ui->leSensorThick->text();
}

