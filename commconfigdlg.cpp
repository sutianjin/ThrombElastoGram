#include "commconfigdlg.h"
#include "ui_commconfigdlg.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QSettings>
#include "fdatatbase.h"


CommConfigDlg::CommConfigDlg(QSerialPort *port,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommConfigDlg)
{
    ui->setupUi(this);
    setWindowTitle(tr("通信配置"));
    m_serialPort = port;
    ui->cbPortList->setModel(&m_portList);

    QObject::connect(ui->pbReloadPort, &QToolButton::clicked,
                     this, &CommConfigDlg::loadPortList);


    QObject::connect(ui->cbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this,&CommConfigDlg::loadPortList);

    QObject::connect(ui->cbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &CommConfigDlg::selectPort);

    QObject::connect(ui->cbBaudRate,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &CommConfigDlg::selectBaudRate);

    loadPortList();
    loadBaudRateList();
    ui->cbBaudRate->setCurrentIndex(ui->cbBaudRate->findText("57600"));
    readComsetSettings();

}

CommConfigDlg::~CommConfigDlg()
{
    delete ui;
}

void CommConfigDlg::loadPortList()
{
    QString currentSelection = ui->cbPortList->currentData(PortNameRole).toString();
    m_portList.loadPortList();
    int index = m_portList.indexOf(currentSelection);
    if (index >= 0)
    {
        ui->cbPortList->setCurrentIndex(index);
    }
}


void CommConfigDlg::selectPort(QString portName)
{
    // 获得端口号
    portName = portName.split(" ")[0];
    // 如果串口发送变化
    if (portName != m_serialPort->portName())
    {
        // 就将原来的串口关闭，打开重新选择的串口
        if (m_serialPort->isOpen())
        {
            togglePort();
            // 打开新的串口
            togglePort();
        }
    }
}


void CommConfigDlg::loadBaudRateList()
{
    ui->cbBaudRate->clear();

    for (auto baudRate : QSerialPortInfo::standardBaudRates())
    {
        ui->cbBaudRate->addItem(QString::number(baudRate));
    }
}

void CommConfigDlg::selectBaudRate(QString baudRate)
{
    if (m_serialPort->isOpen())
    {
        if (!m_serialPort->setBaudRate(baudRate.toInt()))
        {
            qCritical() << "无法设置该波特率!";
        }
    }
}

void CommConfigDlg::togglePort()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
        qDebug() << "Closed port:" << m_serialPort->portName();
        emit portToggled(false);
    }
    else
    {

        //从编辑框选择出串口号，如果有该串口则选中
        //如果没有该串口号，则添加串口号
        QString portText = ui->cbPortList->currentText();
        QString portName;
        int portIndex = m_portList.indexOf(portText);
        if (portIndex < 0) //不在编辑框中
        {
            m_portList.appendRow(new PortListItem(portText));
            ui->cbPortList->setCurrentIndex(m_portList.rowCount()-1);
            portName = portText;
        }
        else
        {
           //获取串口的信息
            portName = static_cast<PortListItem*>(m_portList.item(portIndex))->portName();
        }

        m_serialPort->setPortName(ui->cbPortList->currentData(PortNameRole).toString());

        // 打开串口
        if (m_serialPort->open(QIODevice::ReadWrite))
        {
            // 串口配置
            selectBaudRate(ui->cbBaudRate->currentText());
            m_serialPort->setDataBits(QSerialPort::Data8);
            m_serialPort->setParity(QSerialPort::NoParity);
            m_serialPort->setStopBits(QSerialPort::OneStop);
            m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

            qDebug() << "Opened port:" << m_serialPort->portName();
            emit portToggled(true);
        }
    }
}


stParaInfo CommConfigDlg::GetParaInfo()
{

    m_stParaInfo.iStartFre = ui->StartFreqlineEdit->displayText().toInt(0,10);
    m_stParaInfo.iStopFre = ui->stopFreqlineEdit->displayText().toInt(0,10);
    m_stParaInfo.iStepFre = ui->StepFreqlineEdit->displayText().toInt(0,10);
    m_stParaInfo.iStepInterivalTime = ui->oneStepTimelineEdit->displayText().toInt(0,10);
    m_stParaInfo.iScanNumbers = ui->scanNumlineEdit->displayText().toInt(0,10);
    m_stParaInfo.iMutilScanInterivalTime = ui->multiScanlineEdit->displayText().toInt(0,10);
    m_stParaInfo.iOptionalDC = ui->leOptimalDC->displayText().toInt(0,10);
    m_stParaInfo.iOptionalAC = ui->leOptimalAC->displayText().toInt(0,10);

    return m_stParaInfo;
}

void CommConfigDlg::closeEvent(QCloseEvent *e)
{
    readComsetSettings();
}

void CommConfigDlg::readComsetSettings()
{
    user_data currentuser;
    currentuser= get_user_data();
    QSettings setting("ThromboElastoGram",currentuser.userName);
    QString str_start_fre = setting.value("start_fre").toString();
    QString str_stop_fre = setting.value("stop_fre").toString();
    QString str_step_fre = setting.value("step_fre").toString();
    QString str_step_interival_time = setting.value("step_interival_time").toString();
    QString str_scan_numbers = setting.value("scan_numbers").toString();
    QString str_mutil_scan_interival_time = setting.value("mutil_scan_interival_time").toString();
    QString str_optinal_dc = setting.value("optinal_dc").toString();
    QString str_optinal_ac = setting.value("optinal_ac").toString();

    ui->StartFreqlineEdit->setText(str_start_fre);
    ui->stopFreqlineEdit->setText(str_stop_fre);
    ui->StepFreqlineEdit->setText(str_step_fre);
    ui->oneStepTimelineEdit->setText(str_step_interival_time);
    ui->scanNumlineEdit->setText(str_scan_numbers);
    ui->multiScanlineEdit->setText(str_mutil_scan_interival_time);
    ui->leOptimalDC->setText(str_optinal_dc);
    ui->leOptimalAC->setText(str_optinal_ac);



}
