#ifndef PARASETFORM_H
#define PARASETFORM_H

#include <QWidget>

namespace Ui {
class ParaSetForm;
}

class ParaSetForm : public QWidget
{
    Q_OBJECT

public:
    explicit ParaSetForm(QWidget *parent = 0);
    ~ParaSetForm();
    int getStartFre();
    int getStopFre();
    int getStepFre();
    int getScanInterival();

    int getScanNum();  //多次扫描次数
    int getMutilScanInterival(); //多次扫描的时间间隔

    int getOptimalDC();   //最佳直流
    int getOptimalAC();    //最佳交流


private:
    Ui::ParaSetForm *ui;

    int iStartFre;    //起始频率
    int iStopFre;     //终止频率
    int iStepFre;     //步进频率
    int iScanInterival;  //步进时间间隔

    int iScanNum;  //多次扫描次数
    int iMutilScanInterival; //多次扫描的时间间隔

    int iOptimalDC;   //最佳直流
    int iOptimalAC;    //最佳交流


};

#endif // PARASETFORM_H
