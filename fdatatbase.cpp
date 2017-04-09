#include "fdatatbase.h"
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>
#include <QSqlError>


user_data data_u;

void set_user_data(user_data data)
{
    data_u.dbPathName = data.dbPathName;
    data_u.dbName = data.dbName;
    data_u.userName = data.userName;
}

user_data get_user_data()
{
    user_data ret_user_data;
    ret_user_data.dbPathName = data_u.dbPathName;
    ret_user_data.dbName = data_u.dbName;
    ret_user_data.userName = data_u.userName;
    return ret_user_data;
}

//连接并打开数据库
bool connectDB(const QString &dbName, const QString &user, const QString &password)
{
    QSqlDatabase database;
    qDebug()<<QSqlDatabase::database().databaseName();
    if( QSqlDatabase::database().databaseName() != dbName )	{
        database = QSqlDatabase::addDatabase("QSQLITE", dbName);
        database.setDatabaseName(dbName);
        database.setUserName(user);
        database.setPassword(password);
    } else {
        database = QSqlDatabase::database();
        if ( database.isOpen() ) {
            return true;
        }
    }

     if(!database.open()){
         QMessageBox::critical(0, "ThromboElastoGram",
                       QObject::tr("无法与数据库建立连接.")+"\n"+
                               QObject::tr("检测平台需要数据库的支持 "
                               "请检查数据库的驱动."), QMessageBox::Cancel,
         QMessageBox::NoButton);
         database = QSqlDatabase();
         database.removeDatabase(dbName);
         return false;
     }
     return true;
}



//获得数据的目录路径
QString documentsDir()     //返回建立的目录路径
{
    QString documents("ThromboElastoGram");
    QDir current=QDir::current();
    current.setFilter(QDir::Files);
    if(!current.exists(documents) && !current.mkdir(documents)) {
        QMessageBox::critical(0, "ThromboElastoGram",QObject::tr("Can't create documents directory"));
        return QString("");
    }
    current.cd(documents);
    qDebug()<< current.absolutePath();
    return current.absolutePath() + "/";
}


// 系统管理员数据库
bool createAdministratorDB(const QString &connectdbName)
{
    QString databaseName = connectdbName;
    QSqlQuery query(QSqlDatabase::database(databaseName));

    //创建操作人员表
    QString queryString = "create table operator("
                          "operator_id varchar(20) PRIMARY KEY NOT NULL,"
                          "operator_name varchar(20) NOT NULL,"
                          "operator_password varchar(20) NOT NULL,"
                          "creation_time varchar(25),"
                          "last_operate_time varchar(25),"
                          "TEL varchar(15),"
                          "Email varchar(25)"
                          ")";

    if(!query.exec(queryString)){
        qDebug() << "operator Failed to execute" << queryString;
        return false;
    }

    //创建实验数据库管理表
    queryString = "create table labDB("
                  "labDB_name varchar(20) PRIMARY KEY NOT NULL,"
                  "labDB_password varchar(20),"
                  "storage_path varchar(100)  NOT NULL,"
                  "creation_time varchar(25),"
                  "last_operate_time varchar(25),"
                  "last_operator_id varchar(20)"
                  ")";

    if(!query.exec(queryString)){
        qDebug() << "labDB Failed to execute" << queryString;
        return false;
    }

    //创建授权表
    queryString = "create table Authority("
                  "operator_id varchar(20) NOT NULL,"
                  "labDB_id varchar(20) NOT NULL,"
                  "award_time varchar(25),"
                  "primary key(operator_id,labDB_id)"
                  ")";

    if(!query.exec(queryString)){
        qDebug() << "Authority Failed to execute" << queryString;
        return false;
    }

    return true;

}

//新建操作人员
bool addOperatorRecord(const QString &connectdbName,const QString &operator_id, const QString &operator_name, const QString &operator_password,
                       const QString &Email, const QString &TEL)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));

    QDateTime currenttime = QDateTime::currentDateTime();
    QString creation_time = currenttime.toString("yyyy/MM/dd HH:mm:ss");
    QString last_operate_time = creation_time;

    query.prepare("INSERT INTO operator (operator_id,operator_name,operator_password,creation_time,last_operate_time,Email,TEL) "
                  "VALUES (:operator_id,:operator_name,:operator_password,:creation_time,:last_operate_time,:Email,:TEL)");

    query.bindValue(":operator_id",operator_id);
    query.bindValue(":operator_name",operator_name);
    query.bindValue(":operator_password",operator_password);
    query.bindValue(":creation_time",creation_time);
    query.bindValue(":last_operate_time",last_operate_time);
    query.bindValue(":Email",Email);
    query.bindValue(":TEL",TEL);


    if(!query.exec()){
        qDebug()<<query.lastError();
        return false;
    }

    return true;
}


//检查系统管理员密码
bool checkAdminPassword(const QString &connectdbName, const QString &password)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    if(QSqlDatabase::database(databaseName).password() != password) return false;

    return true;

}

//获取所以操作人员ID
QStringList getAllOperatorID(const QString &connectdbName)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));

    QStringList retstringlist;

    query.prepare("select operator_ID from operator");
    query.exec();

    while(query.next()){
        retstringlist<<query.value(0).toString();
    }

    return retstringlist;

}


//新增实验数据库
bool addLabDBRecord(const QString &connectdbName, const QString &labDB_name)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));

    QString storage_path = documentsDir() + labDB_name + ".db";

    QDateTime currenttime = QDateTime::currentDateTime();
    QString creation_time = currenttime.toString("yyyy/MM/dd HH:mm:ss");
    QString last_operate_time = creation_time;

    query.prepare("INSERT INTO labDB (labDB_name,storage_path,creation_time,last_operate_time) "
                  "VALUES (:labDB_name,:storage_path,:creation_time,:last_operate_time)");

    query.bindValue(":labDB_name",labDB_name);
    query.bindValue(":storage_path",storage_path);
    query.bindValue(":creation_time",creation_time);
    query.bindValue(":last_operate_time",last_operate_time);


    if(!query.exec()){
        qDebug()<<query.lastError();
        return false;
    }

    return true;

}


//添加权限记录

bool addAuthorizeRecord(const QString &connectdbName, const QString &operator_id, const QString &labDB_name)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));



    QDateTime currenttime = QDateTime::currentDateTime();
    QString award_time = currenttime.toString("yyyy/MM/dd HH:mm:ss");


    query.prepare("INSERT INTO Authority (operator_id,labDB_id,award_time) "
                  "VALUES (:operator_id,:labDB_id,:award_time)");

    query.bindValue(":operator_id",operator_id);
    query.bindValue(":labDB_id",labDB_name);
    query.bindValue(":award_time",award_time);


    if(!query.exec()){
        qDebug()<<query.lastError();
        return false;
    }

    return true;
}


//获取操作员可以操作的数据名称

QStringList getHasAuthorize(const QString &connectdbName,const QString &operator_id)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));

    QStringList retstringlist;

    query.prepare("select labDB_id from Authority where operator_id=:operator_id");
    query.bindValue(":operator_id",operator_id);
    query.exec();

    while(query.next()){
        retstringlist<<query.value(0).toString();
    }

    return retstringlist;
}

//检验操作员密码
bool checkOperatorPassword(const QString &connectdbName, const QString &operator_id,const QString &password)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));


    query.prepare("select operator_password from operator where operator_id=:operator_id");
    query.bindValue(":operator_id",operator_id);
    query.exec();
    query.next();
    if(query.value(0).toString() == password){
        return true;
    }
    return false;
}


//检测是否拥有权限
bool checkHasAuthorize(const QString &connectdbName, const QString &operator_id, const QString &labDB_name)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));



    query.prepare("select labDB_id from Authority where operator_id=:operator_id");
    query.bindValue(":operator_id",operator_id);
    query.exec();

    while(query.next()){
        if(query.value(0).toString()==labDB_name)
            return true;
    }

    return false;
}

//获取所以的数据库
QStringList getAllLabDB(const QString &connectdbName)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));

    QStringList retstringlist;

    query.prepare("select labDB_name from labDB");
    query.exec();

    while(query.next()){
        retstringlist<<query.value(0).toString();
    }

    return retstringlist;
}





/* ****************************************************************************
 * 以下代码主要是实现对实验数据库的操作
 * ***************************************************************************/

bool createOperatorDB(const QString &connectdbName )
{
    QString databaseName = documentsDir()+connectdbName+".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));

    //创建操作人员表
    QString queryString = "create table operater_info("
                          "operator_id varchar(20) primary key,"
                          "operator_name varchar(20),"
                          "Email varchar(20),"
                          "last_login_time varchar(20)"
                          ")";
    if (!query.exec(queryString)) {
        qDebug() << "operater_info Failed to execute" << queryString;
        return false;
    }

    //试剂样品信息表
    queryString = "create table sample_info("
                          "sample_type varchar(40) primary key,"
                          "sample_desc varchar(100),"
                          "sample_comment text,"
                          "normal_R_value float,"
                          "normal_K_value float,"
                          "normal_Angle_value float,"
                          "normal_MA_value float"
                          ")";
    if (!query.exec(queryString)) {
        qDebug() << "sample_info Failed to execute" << queryString;
        return false;
    }



    //创建磁弹性传感器芯片信息表
    queryString = "create table mag_chip_info("
                          "mag_chip_id varchar(20) primary key,"
                          "lenght float,"
                          "width float,"
                          "thick float,"
                          "type varchar(20),"
                          "optinal_dc int,"
                          "optinal_ac int"
                          ")";
    if (!query.exec(queryString)) {
        qDebug() << "mag_chip_info Failed to execute" << queryString;
        return false;
    }


    //创建病人信息表
    queryString = "create table patient_info("
                          "patient_id varchar(20) primary key NOT NULL,"
                          "patient_name varchar(20),"
                          "birthday  varchar(20),"
                          "age int,"
                          "sex varchar(2),"
                          "weight float,"
                          "height float,"
                          "creation_time varchar(25),"
                          "medical_info text"
                          ")";
    if (!query.exec(queryString)) {
        qDebug() << "patient_info Failed to execute" << queryString;
        return false;
    }


    //创建实验用例表
    queryString = "create table test_case_info ("
                  "test_case_id varchar(20) primary key,"
                  "operater_id varchar(20),"
                  "patient_id varchar(20),"
                  "sample_type varchar(40),"
                  "mag_chip_id varchar(20),"
                  "instrument_id varchar(20),"
                  "test_date varchar(20),"
                  "test_time varchar(20),"
                  "foreign key (operater_id) references test_case_info(test_case_id) on delete cascade on update cascade,"
                  "foreign key (sample_type) references sample_info(sample_type) on delete cascade on update cascade,"
                  "foreign key (mag_chip_id) references mag_chip_info(mag_chip_id) on delete cascade on update cascade,"
                  "foreign key (patient_id) references patient_info(patient_id) on delete cascade on update cascade"
                  ")";

    if (!query.exec(queryString)) {
        qDebug() << "test_case_info Failed to execute" << queryString;
        return false;
    }


    //创建参数信息表
    queryString = "create table test_para_info ("
                  "test_case_id varchar(20) primary key,"
                  "start_fre int,"
                  "stop_fre int,"
                  "step_fre int,"
                  "step_interival_time int,"
                  "optinal_dc int,"
                  "optinal_ac int,"
                  "scan_numbers int,"
                  "multi_scan_interival int,"
                  "foreign key (test_case_id) references test_case_info(test_case_id) on delete cascade on update cascade"

                  ")";
    if (!query.exec(queryString)) {
        qDebug() << "test_para_info Failed to execute" << queryString;
        return false;
    }


    //创建原始数据信息表
    queryString = "create table origin_data_info("
                          "test_case_id varchar(20) primary key,"
                          "amplitude text,"
                          "phase text,"
                          "frequency text,"
                          "foreign key (test_case_id) references test_case_info(test_case_id) on delete cascade on update cascade"
                          ")";
    if (!query.exec(queryString)) {
        qDebug() << "origin_data_info Failed to execute" << queryString;
        return false;
    }


    queryString = "create table teg_data_info("
                          "test_case_id varchar(20) primary key,"
                          "res_amplitude text,"
                          "res_phase text,"
                          "timeline text,"
                          "test_start_time varchar(25),"
                          "test_end_time varchar(25),"
                          "R_value float,"
                          "K_value float,"
                          "Angle_value float,"
                          "MA_value float,"
                          "foreign key (test_case_id) references test_case_info(test_case_id) on delete cascade on update cascade"
                          ")";
    if (!query.exec(queryString)) {
        qDebug() << "teg_data_info Failed to execute" << queryString;
        return false;
    }


/*
    //创建操作人员表
    QString queryString1 = "create table operater("
                          "username varchar,"
                          "password varchar,"
                          "Email varchar"
                          ")";
    if (!query.exec(queryString)) {
        QMessageBox::critical(0, "ThromboElastoGram",QObject::tr("该用户名已经被注册，请重新输入"));
        return false;
    }

    query.prepare("INSERT INTO operater (username,password)"
                  "VALUES(:sername,:password)");
    query.bindValue(":username",connectdbName);
    query.bindValue(":password",password);


    if (!query.exec()) {
        qDebug() << "无法在操作员表插入数据！" ;
        return false;
    }

    //创建实验测试表
    queryString = "create table testInfo ("
                   "id integer primary key,"
                    "operator_id varchar,"
                    "operate_time varchar)";

    if (!query.exec(queryString)) {
        qDebug() << "testInfo Failed to execute" << queryString;
        return false;
    }

    //创建传感器信息表
    queryString = "create table sensorinfo ("
                  "test_id integer primary key,"
                  "sensor_type varchar,"
                  "length varchar,"
                  "width varchar,"
                  "thickness varchar"
                  "optimal_dc_bias varchar,"
                  "optimal_ac_bias varchar,"
                  "foreign key (test_id) references testinfo(id) on delete cascade on update cascade)";
    if (!query.exec(queryString)) {
        qDebug() << "sensorinfo Failed to execute" << queryString;
        return false;
    }

    //创建参数信息表
    queryString = "create table parainfo ("
                  "test_id integer primary key,"
                  "start_fre int,"
                  "stop_fre int,"
                  "step_fre int,"
                  "interival int,"
                  "dc_data int,"
                  "ac_data int,"
                  "scan_num int,"
                  "multi_scan_interival int,"
                  "test_describe varchar,"
                  "foreign key (test_id) references testinfo(id) on delete cascade on update cascade)";
    if (!query.exec(queryString)) {
        qDebug() << "parainfo Failed to execute" << queryString;
        return false;
    }



    //创建实验结果信息表
    queryString = "create table resultinfo ("
                  "test_id integer primary key,"
                  "res_amp text,"
                  "res_phase text,"
                  "foreign key (test_id) references testinfo(id) on delete cascade on update cascade)";

    if (!query.exec(queryString)) {
        qDebug() << "resultinfo Failed to execute" << queryString;
        return false;
    }

    //原始数据的信息表
    queryString = "create table origin_data_info ("
                  "test_id integer primary key,"
                  "frequency text,"
                  "amplitude text,"
                  "phase text,"
                  "foreign key (test_id) references testinfo(id) on delete cascade on update cascade)";

    if (!query.exec(queryString)) {
        qDebug() << "origin_data_info Failed to execute" << queryString;
        return false;
    }




    //创建样本的数据库
    queryString = "create table test_case_info ("
                  "test_id varchar(20) primary key,"
                  "patient_id varchar(20),"
                  "sample_id varchar(20),"
                  "test_date varchar(20),"
                  "test_time varchar(20),"
                  "instrument_id varchar(20),"
                  "operater_id varchar(20))";

    if (!query.exec(queryString)) {
        qDebug() << "test_case_info Failed to execute" << queryString;
        return false;
    }

    //创建样本实验结果的数据库
    queryString = "create table test_result_info ("
                  "test_result_id varchar(20) primary key,"
                  "amplitude_value text,"
                  "phase_value text,"
                  "timestamp_value text,"
                  "test_start_time varchar(25),"
                  "test_end_time varchar(25),"
                  "R_value integer,"
                  "K_value integer,"                  
                  "Angle_value integer,"
                  "MA_value integer,"
                  "test_id varchar(20))";

    if (!query.exec(queryString)) {
        qDebug() << "test_result_info Failed to execute" << queryString;
        return false;
    }

    //创建病人信息表
    queryString = "create table test_patient_info ("
                  "patient_id varchar(20) primary key,"
                  "patient_name varchar(20),"
                  "sex varchar(2),"
                  "age integer,"
                  "weight integer,"
                  "height integer)";

    if (!query.exec(queryString)) {
        qDebug() << "test_patient_info Failed to execute create" << queryString;
        return false;
    }*/

    return true;

}


/*
bool checkPassword(const QString& connectdbName,const QString& password)
{
    QString databaseName = documentsDir() + connectdbName + ".db";
    QSqlQuery query(QSqlDatabase::database(databaseName));

    query.prepare("select * from operater");
    query.exec();
    QString pass;

    while (query.next()) {
        pass = query.value(1).toString();
    }

    if (password == pass) {
        return true;
    }

    return false;
}


bool addNewTest(int id)
{
   // QSqlDatabase database;

    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));

    QDateTime nowtime = QDateTime::currentDateTime();
    QString timestr = nowtime.toString("yyyy-MM-dd hh:mm");

    query.prepare("INSERT INTO testinfo (id,operator_id,operate_time)"
                  "VALUES(:id,:operator_id,:operate_time)");
    query.bindValue(":id",id);
    query.bindValue(":operator_id",data_u.userName);
    query.bindValue(":operate_time",timestr);

    if (!query.exec()) {
        qDebug() << "Failed to execute insert operator to testinfo table" ;
        return false;
    }

    return true;
}


bool init_test_para(int startfre, int stopfre, int stepfre, int scaninterival,
                   int dcdata, int acdata, int scannum, int mutilsacaninter,QString test_desc,int test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("INSERT INTO parainfo (test_id,start_fre,stop_fre,step_fre,interival,dc_data,ac_data,scan_num,multi_scan_interival,test_describe)"
                  "VALUES(:test_id,:start_fre,:stop_fre,:step_fre,:interival,:dc_data,:ac_data,:scan_num,:multi_scan_interival,:test_describe)");
    query.bindValue(":test_id",test_id);
    query.bindValue(":start_fre",startfre);
    query.bindValue(":stop_fre",stopfre);
    query.bindValue(":step_fre",stepfre);
    query.bindValue(":interival",scaninterival);
    query.bindValue(":dc_data",dcdata);
    query.bindValue(":ac_data",acdata);
    query.bindValue(":scan_num",scannum);
    query.bindValue(":multi_scan_interival",mutilsacaninter);
    query.bindValue(":test_describe",test_desc);

    if (!query.exec()) {
        qDebug() << "Failed to execute insert operator to paraInfo table" ;
        return false;
    }

    return true;

}

bool set_test_para(int startfre, int stopfre, int stepfre, int scaninterival,
                   int dcdata, int acdata, int scannum, int mutilsacaninter,QString test_desc,int test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("UPDATE parainfo SET start_fre=:start_fre,stop_fre=:stop_fre,step_fre=:step_fre,interival=:interival,"
                  "dc_data=:dc_data,ac_data=:ac_data,scan_num=:scan_num,multi_scan_interival=:multi_scan_interival,test_describe=:test_describe WHERE test_id=:test_id");

    query.bindValue(":start_fre",startfre);
    query.bindValue(":stop_fre",stopfre);
    query.bindValue(":step_fre",stepfre);
    query.bindValue(":interival",scaninterival);
    query.bindValue(":dc_data",dcdata);
    query.bindValue(":ac_data",acdata);
    query.bindValue(":scan_num",scannum);
    query.bindValue(":multi_scan_interival",mutilsacaninter);
    query.bindValue(":test_describe",test_desc);
    query.bindValue(":test_id",test_id);

    if (!query.exec()) {
        qDebug() << "Failed to execute update operator to paraInfo table" ;
        return false;
    }

    return true;

}

QStringList get_para_info(int id)
{
    QStringList stringList;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select * from parainfo");
    query.exec();

    if(query.seek(id)){
        stringList<<query.value(1).toString()<<query.value(2).toString()
                 <<query.value(3).toString()<<query.value(4).toString()
                <<query.value(5).toString()<<query.value(6).toString()
               <<query.value(7).toString()<<query.value(8).toString()
              <<query.value(9).toString();
    }

    return stringList;
}

QStringList get_test_info(int id)
{
    QStringList stringList;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select * from testinfo");
    query.exec();

    if(query.seek(id)){
        stringList<<query.value(1).toString()<<query.value(2).toString();
    }
    return stringList;
}


bool init_sensor_info(QString type, QString length, QString width, QString thick,int test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("INSERT INTO sensorinfo (test_id,sensor_type,length,width,thickness)"
                  "VALUES(:test_id,:sensor_type,:length,:width,:thickness)");
    query.bindValue(":test_id",test_id);
    query.bindValue(":sensor_type",type);
    query.bindValue(":length",length);
    query.bindValue(":width",width);
    query.bindValue(":thickness",thick);

    if (!query.exec()) {
        qDebug() << "Failed to execute insert operator to sensorInfo table" ;
        return false;
    }


    return true;

}

bool set_sensor_info(QString type, QString length, QString width, QString thick,int test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("UPDATE sensorinfo SET sensor_type=:sensor_type,length=:length,"
                  "width=:width,thickness=:thickness WHERE test_id=:test_id");
    query.bindValue(":test_id",test_id);
    query.bindValue(":sensor_type",type);
    query.bindValue(":length",length);
    query.bindValue(":width",width);
    query.bindValue(":thickness",thick);

    if (!query.exec()) {
        qDebug() << "Failed to execute update sensorInfo table" ;
        return false;
    }


    return true;

}

QStringList get_sensor_info(int id)
{
    QStringList stringList;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select * from sensorinfo");
    query.exec();

    if(query.seek(id)){
        stringList<<query.value(1).toString()<<query.value(2).toString()
                 <<query.value(3).toString()<<query.value(4).toString();
        qDebug()<<query.value(1).toString()<<query.value(2).toString()
               <<query.value(3).toString()<<query.value(4).toString();
    }
    return stringList;
}

bool init_origin_info(QString frequency, QString amplitude, QString phase, int test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("INSERT INTO origin_data_info (test_id,frequency,amplitude,phase)"
                  "VALUES(:test_id,:frequency,:amplitude,:phase)");
    query.bindValue(":test_id",test_id);
    query.bindValue(":frequency",frequency);
    query.bindValue(":amplitude",amplitude);
    query.bindValue(":phase",phase);

    if (!query.exec()) {
        qDebug() << "Failed to execute insert operator to origin_data_info table" ;
        return false;
    }


    return true;

}

bool set_origin_info(QString frequency, QString amplitude, QString phase, int test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("UPDATE origin_data_info SET frequency=:frequency,amplitude=:amplitude,"
                  "phase=:phase WHERE test_id=:test_id");
    query.bindValue(":test_id",test_id);
    query.bindValue(":frequency",frequency);
    query.bindValue(":amplitude",amplitude);
    query.bindValue(":phase",phase);

    if (!query.exec()) {
        qDebug() << "Failed to execute update operator to origin_data_info table" ;
        return false;
    }


    return true;

}

QStringList init_origin_fre_data()
{
    QStringList stringList;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select * from origin_data_info");
    query.exec();
    int recordNum = query.size();
    if(recordNum){
        while (query.next()) {
            stringList<<query.value(1).toString();
        }
    }
    return stringList;
}

QStringList init_origin_amp_data()
{
    QStringList stringList;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select * from origin_data_info");
    query.exec();
    int recordNum = query.size();
    if(recordNum){
        while (query.next()) {
            stringList<<query.value(2).toString();
        }
    }
    return stringList;
}

QStringList init_origin_phase_data()
{
    QStringList stringList;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select * from origin_data_info");
    query.exec();
    int recordNum = query.size();
    if(recordNum){
        while (query.next()) {
            stringList<<query.value(3).toString();
        }
    }
    return stringList;
}


bool delete_one_record(int test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("DELETE FROM testinfo WHERE id=:test_id");
    query.bindValue(":test_id",test_id);
    if (!query.exec()) {
        qDebug() << "Failed to execute delete operator to origin_data_info table" ;
        return false;
    }
    query.prepare("DELETE FROM sensorinfo WHERE test_id=:test_id");
    query.bindValue(":test_id",test_id);

    if (!query.exec()) {
        qDebug() << "Failed to execute delete operator to sensorinfo table" ;
        return false;
    }
    query.prepare("DELETE FROM parainfo WHERE test_id=:test_id");
    query.bindValue(":test_id",test_id);

    if (!query.exec()) {
        qDebug() << "Failed to execute delete operator to parainfo table" ;
        return false;
    }

    query.prepare("DELETE FROM resultinfo WHERE test_id=:test_id");
    query.bindValue(":test_id",test_id);

    if (!query.exec()) {
        qDebug() << "Failed to execute delete operator to resultinfo table" ;
        return false;
    }
    query.prepare("DELETE FROM origin_data_info WHERE test_id=:test_id");
    query.bindValue(":test_id",test_id);

    if (!query.exec()) {
        qDebug() << "Failed to execute delete operator to origin_data_info table" ;
        return false;
    }

    query.prepare("UPDATE testinfo SET id=id-1 where id > :test_id");
    query.bindValue(":test_id",test_id);
    if (!query.exec()) {
        qDebug() << "Failed to execute update_sort  operator to testinfo table" ;
        return false;
    }

    query.exec("select * from sensorinfo");
    while(query.next()){
       qDebug()<<"id"<<query.value(0).toInt();
    }

    query.prepare("UPDATE sensorinfo SET test_id=test_id-1 where test_id > :test_id");
    query.bindValue(":test_id",test_id);
    if (!query.exec()) {
        qDebug() << "Failed to execute update_sort  operator to sensorinfo table" ;
        return false;
    }

    query.prepare("UPDATE parainfo SET test_id=test_id-1 where test_id > :test_id");
    query.bindValue(":test_id",test_id);
    if (!query.exec()) {
        qDebug() << "Failed to execute update_sort  operator to sensorinfo table" ;
        return false;
    }

    query.prepare("UPDATE resultinfo SET test_id=test_id-1 where test_id > :test_id");
    query.bindValue(":test_id",test_id);
    if (!query.exec()) {
        qDebug() << "Failed to execute update_sort  operator to sensorinfo table" ;
        return false;
    }

    query.prepare("UPDATE origin_data_info SET test_id=test_id-1 where test_id > :test_id");
    query.bindValue(":test_id",test_id);
    if (!query.exec()) {
        qDebug() << "Failed to execute update_sort  operator to sensorinfo table" ;
        return false;
    }



    query.exec("select * from sensorinfo");
    while(query.next()){
       qDebug()<<"id"<<query.value(0).toInt();
    }



    return true;

}*/

bool add_test_case_info(QString test_case_id,QString operater_id,QString patient_id,QString sample_type,QString mag_chip_id,
                        QString instrument_id,QString test_date,QString test_time)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("INSERT INTO test_case_info (test_case_id,operater_id,patient_id,sample_type,mag_chip_id,instrument_id,test_date,test_time)"
                  "VALUES(:test_case_id,:operater_id,:patient_id,:sample_type,:mag_chip_id,:instrument_id,:test_date,:test_time)");
    query.bindValue(":test_case_id",test_case_id);
    query.bindValue(":operater_id",operater_id);
    query.bindValue(":patient_id",patient_id);
    query.bindValue(":sample_type",sample_type);
    query.bindValue(":mag_chip_id",mag_chip_id);
    query.bindValue(":instrument_id",instrument_id);
    query.bindValue(":test_date",test_date);
    query.bindValue(":test_time",test_time);


    if (!query.exec()) {
        qDebug() << "Failed to execute insert operator to test_case_info table" <<query.lastError();
        return false;
    }

    return true;
}


bool add_patient_info(QString patient_id,QString patient_name,QString birthday, QString sex,int age, float weight,float height,QString medical_info)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));

    QDateTime currenttime = QDateTime::currentDateTime();
    QString creation_time = currenttime.toString("yyyy/MM/dd HH:mm:ss");

    query.prepare("INSERT INTO patient_info (patient_id,patient_name,birthday,age,sex,weight,height,creation_time,medical_info)"
                  "VALUES(:patient_id,:patient_name,:birthday,:age,:sex,:weight,:height,:creation_time,:medical_info)");


    query.bindValue(":patient_id",patient_id);
    query.bindValue(":patient_name",patient_name);
    query.bindValue(":birthday",birthday);
    query.bindValue(":age",age);
    query.bindValue(":sex",sex);

    query.bindValue(":weight",weight);
    query.bindValue(":height",height);

    query.bindValue(":creation_time",creation_time);

    query.bindValue(":medical_info",medical_info);

    if (!query.exec()) {
        qDebug() << "Failed to execute insert patient information to test_patient_info table"<<query.lastError();
        return false;
    }

    return true;
}



bool add_teg_data_info(QString test_case_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));

    query.prepare("INSERT INTO teg_data_info (test_case_id)"
                  "VALUES(:test_case_id)");
    query.bindValue(":test_case_id",test_case_id);



    if (!query.exec()) {
        qDebug() << "Failed to execute insert operator to teg_data_info table" <<query.lastError();
        return false;
    }

    return true;
}


bool update_teg_data_info(QString res_amplitude_value, QString res_phase_value,QString res_timesline_value,QString test_case_id)
{

    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("UPDATE teg_data_info SET res_amplitude=:res_amplitude,res_phase=:res_phase,timeline=:timeline "
                  "WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_case_id);
    query.bindValue(":res_amplitude",res_amplitude_value);
    query.bindValue(":res_phase",res_phase_value);
    query.bindValue(":timeline",res_timesline_value);

    if (!query.exec()) {
        qDebug() << "Failed to execute update teg_data_info table" <<query.lastError();
        return false;
    }
    return true;
}

bool update_test_start_time_info(QString start_time,QString test_case_id)
{

    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("UPDATE teg_data_info SET test_start_time=:test_start_time "
                  "WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_case_id);
    query.bindValue(":test_start_time",start_time);

    if (!query.exec()) {
        qDebug() << "Failed to execute update test_start_time_info table" ;
        return false;
    }
    return true;
}

bool update_test_end_time_info(QString end_time,QString test_case_id)
{

    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("UPDATE teg_data_info SET test_end_time=:test_end_time "
                  "WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_case_id);
    query.bindValue(":test_end_time",end_time);

    if (!query.exec()) {
        qDebug() << "Failed to execute update test_end_time_info table " ;
        return false;
    }
    return true;
}


QStringList get_all_testid()
{
    QStringList stringList;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select * from test_case_info");
    query.exec();
    int recordNum = query.size();
    if(recordNum){
        while (query.next()) {
            stringList<<query.value(0).toString();
        }
    }
    return stringList;
}



QString get_teg_data_amp(QString test_id)
{
    QString restr;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select res_amplitude from teg_data_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    query.next();
    restr = query.value(0).toString();
    return restr;
}


QString get_teg_data_phase(QString test_id)
{
    QString restr;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select res_phase from teg_data_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    query.next();
    restr = query.value(0).toString();
    return restr;
}

QString get_teg_data_time(QString test_id)
{
    QString restr;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select timeline from teg_data_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    query.next();
    restr = query.value(0).toString();
    return restr;
}

QString get_test_start_time(QString test_id)
{
    QString restr;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select test_start_time from teg_data_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    query.next();
    restr = query.value(0).toString();
    return restr;
}


QString get_test_end_time(QString test_id)
{
    QString restr;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select test_end_time from teg_data_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    query.next();
    restr = query.value(0).toString();
    return restr;
}

QString get_test_case_patient_info(QString test_id)
{
    QString restr;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select patient_id from test_case_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    query.next();
    restr = query.value(0).toString();
    return restr;
}

bool add_sample_info(QString sample_type, QString sample_desc, QString sample_comment)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("INSERT INTO sample_info (sample_type,sample_desc,sample_comment)"
                  "VALUES(:sample_type,:sample_desc,:sample_comment)");
    query.bindValue(":sample_type",sample_type);
    query.bindValue(":sample_desc",sample_desc);
    query.bindValue(":sample_comment",sample_comment);

    if (!query.exec()) {
        qDebug() << "Failed to execute insert to sample_info table" ;
        return false;
    }

    return true;
}

QStringList get_all_patient_id()
{
    QStringList restrlist;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select patient_id from patient_info");
    query.exec();
    while(query.next()){
        restrlist<<query.value(0).toString();
    }
    return restrlist;
}

QStringList get_all_sample_type_id()
{
    QStringList restrlist;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select sample_type from sample_info");
    query.exec();
    while(query.next()){
        restrlist<<query.value(0).toString();
    }
    return restrlist;
}

bool add_test_para_info(QString test_case_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));

    query.prepare("INSERT INTO test_para_info (test_case_id)"
                  "VALUES(:test_case_id)");
    query.bindValue(":test_case_id",test_case_id);



    if (!query.exec()) {
        qDebug() << "Failed to execute insert operator to test_para_info table" <<query.lastError();
        return false;
    }

    return true;
}

bool update_test_para(int startfre,int stopfre,int stepfre,int scaninterival,
                   int dcdata,int acdata,int scannum,int mutilsacaninter,QString test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("UPDATE test_para_info SET start_fre=:start_fre,stop_fre=:stop_fre,step_fre=:step_fre,step_interival_time=:step_interival_time,"
                  "optinal_dc=:optinal_dc,optinal_ac=:optinal_ac,scan_numbers=:scan_numbers,multi_scan_interival=:multi_scan_interival WHERE test_case_id=:test_case_id");

    query.bindValue(":start_fre",startfre);
    query.bindValue(":stop_fre",stopfre);
    query.bindValue(":step_fre",stepfre);
    query.bindValue(":step_interival_time",scaninterival);
    query.bindValue(":optinal_dc",dcdata);
    query.bindValue(":optinal_ac",acdata);
    query.bindValue(":scan_numbers",scannum);
    query.bindValue(":multi_scan_interival",mutilsacaninter);
    query.bindValue(":test_case_id",test_id);

    if (!query.exec()) {
        qDebug() << "Failed to execute update operator to test_para_Info table"<<query.lastError() ;
        return false;
    }

    return true;
}

bool add_origin_data_info(QString test_case_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));

    query.prepare("INSERT INTO origin_data_info (test_case_id)"
                  "VALUES(:test_case_id)");
    query.bindValue(":test_case_id",test_case_id);



    if (!query.exec()) {
        qDebug() << "Failed to execute insert operator to origin_data_info table" <<query.lastError();
        return false;
    }

    return true;
}

bool update_origin_data_info(QString amplitude_value, QString phase_value, QString frequency_value, QString test_id)
{
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("UPDATE origin_data_info SET frequency=:frequency,amplitude=:amplitude,"
                  "phase=:phase WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.bindValue(":frequency",frequency_value);
    query.bindValue(":amplitude",amplitude_value);
    query.bindValue(":phase",phase_value);

    if (!query.exec()) {
        qDebug() << "Failed to execute update operator to origin_data_info table" <<query.lastError();
        return false;
    }


    return true;
}


QString get_origin_fre_data(QString test_id)
{
    QString resstring;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select frequency from origin_data_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    while (query.next()) {
        resstring = query.value(0).toString();
    }
    return resstring;
}

QString get_origin_amp_data(QString test_id)
{
    QString resstring;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select amplitude from origin_data_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    while (query.next()) {
        resstring = query.value(0).toString();
    }
    return resstring;
}

QString get_origin_phase_data(QString test_id)
{
    QString resstring;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select phase from origin_data_info WHERE test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_id);
    query.exec();
    while (query.next()) {
        resstring = query.value(0).toString();
    }
    return resstring;
}

stParaInfo get_para_info(QString test_case_id)
{
    stParaInfo stparaList;
    QSqlQuery query(QSqlDatabase::database(data_u.dbPathName));
    query.prepare("select * from test_para_info where test_case_id=:test_case_id");
    query.bindValue(":test_case_id",test_case_id);

    query.exec();
    query.next();

    stparaList.iStartFre=query.value(1).toInt();
    stparaList.iStopFre=query.value(2).toInt();
    stparaList.iStepFre=query.value(3).toInt();
    stparaList.iStepInterivalTime=query.value(4).toInt();
    stparaList.iOptionalDC=query.value(5).toInt();
    stparaList.iOptionalAC=query.value(6).toInt();
    stparaList.iScanNumbers=query.value(7).toInt();
    stparaList.iMutilScanInterivalTime=query.value(8).toInt();

    return stparaList;
}

