#ifndef FDATATBASE_H
#define FDATATBASE_H
#include <QString>
#include <QSqlDatabase>
#include <QStringList>
#include <QSqlTableModel>

#include "utils.h"

struct user_data
{
   QString dbPathName;//path to base
   QString dbName;
   QString userName;
} ;


bool connectDB(QString const& dbName, const QString& user="",const QString& password="");
QString documentsDir();
bool createOperatorDB(const QString& connectdbName) ;
bool createAdministratorDB(const QString& connectdbName) ;

//系统管理员添加
bool addOperatorRecord(const QString &connectdbName,const QString &operator_id, const QString &operator_name, const QString &operator_password,
                       const QString &Email="", const QString &TEL="");

bool checkAdminPassword(const QString& connectdbName,const QString& password);
bool addLabDBRecord(const QString &connectdbName,const QString &labDB_name);
bool deleteLabDBRecord(const QString &connectdbName,const QString &labDB_name);
bool addAuthorizeRecord(const QString &connectdbName,const QString &operator_id, const QString &labDB_name);
QStringList getAllOperatorID(const QString& connectdbName);
bool checkHasAuthorize(const QString &connectdbName,const QString &operator_id,const QString &labDB_name);

QStringList getHasAuthorize(const QString &connectdbName,const QString &operator_id);
bool checkOperatorPassword(const QString &connectdbName,const QString &operator_id,const QString &password);
QStringList getAllLabDB(const QString& connectdbName);




//添加实验用例
bool add_test_case_info(QString test_case_id,QString operater_id,QString patient_id,QString sample_type,QString mag_chip_id,
                        QString instrument_id,QString test_date,QString test_time);

//添加实验信息
bool add_patient_info(QString patient_id,QString patient_name,QString birthday, QString sex,int age, float weight,float height,QString medical_info);

void set_user_data(user_data data);
user_data get_user_data();

bool add_sample_info(QString sample_type,QString sample_desc,QString sample_comment="");
QStringList get_all_patient_id();
QStringList get_all_sample_type_id();




stParaInfo get_para_info(QString test_case_id);
QStringList get_test_info(int id);
QStringList get_sensor_info(int id);

QStringList init_origin_fre_data();
QStringList init_origin_amp_data();
QStringList init_origin_phase_data();

//获得所有测试记录
QStringList get_all_testid();

bool add_teg_data_info(QString test_case_id);

bool update_teg_data_info(QString amplitude_value, QString phase_value,QString timestamp_value,QString test_id);
bool update_test_start_time_info(QString start_time,QString test_id);
bool update_test_end_time_info(QString end_time,QString test_id);

QString get_test_start_time(QString test_id);
QString get_test_end_time(QString test_id);


QString get_test_case_patient_info(QString test_id);

QString get_teg_data_amp(QString test_id);
QString get_teg_data_phase(QString test_id);
QString get_teg_data_time(QString test_id);

bool add_test_para_info(QString test_case_id);
bool update_test_para(int startfre,int stopfre,int stepfre,int scaninterival,
                   int dcdata,int acdata,int scannum,int mutilsacaninter,QString test_id);
//原始数据
bool add_origin_data_info(QString test_case_id);
bool update_origin_data_info(QString amplitude_value, QString phase_value,QString frequency_value,QString test_id);
QString get_origin_fre_data(QString test_id);
QString get_origin_amp_data(QString test_id);
QString get_origin_phase_data(QString test_id);


//添加传感器材料信息


//获取数据库所有表



#endif // FDATATBASE_H
