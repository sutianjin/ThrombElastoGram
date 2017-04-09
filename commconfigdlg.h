#ifndef COMMCONFIGDLG_H
#define COMMCONFIGDLG_H

#include <QDialog>
#include <QSerialPort>
#include "portlist.h"
#include "utils.h"



namespace Ui {
class CommConfigDlg;
}

class CommConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CommConfigDlg(QSerialPort *port,QWidget *parent = 0);
    ~CommConfigDlg();

    stParaInfo GetParaInfo();


public slots:
    void loadPortList();                    //导入系统的端口号
    void loadBaudRateList();                //导入波特率
    void togglePort();                      //打开串口
    void selectPort(QString portName);      //设置串口
    void selectBaudRate(QString baudRate);  //设置波特率
protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::CommConfigDlg *ui;

    QSerialPort* m_serialPort;
    PortList m_portList;


    stParaInfo m_stParaInfo;
    void readComsetSettings();

signals:
   void portToggled(bool open);
};

#endif // COMMCONFIGDLG_H
