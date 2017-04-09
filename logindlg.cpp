#include "logindlg.h"
#include "ui_logindlg.h"
#include "fdatatbase.h"
#include <QMessageBox>
#include <QDir>
#include <QInputDialog>
#include <QSettings>

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint);
    initShow();

    QObject::connect(ui->cbUserType,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChanged(int)));
    QObject::connect(ui->pbExit,SIGNAL(clicked()),this,SLOT(close()));
    QObject::connect(ui->pbAuthorize,SIGNAL(clicked(bool)),this,SLOT(onAuthorizeClicked(bool)));
    QObject::connect(ui->pbCreateNewDB,SIGNAL(clicked()),this,SLOT(onCreateNewDB()));
    QObject::connect(ui->pbAuthorizeOK,SIGNAL(clicked()),this,SLOT(onAuthorizeOKClicked()));
    QObject::connect(ui->cbUser,SIGNAL(currentTextChanged(QString)),this,SLOT(onUserIndexChanged(QString)));



    /*获得数据库存储地址*/
    QDir dir(documentsDir());

    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList () << "*.db");
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
       QFileInfo fileInfo = list.at(i);
       if(fileInfo.baseName() != "administrator"){
           m_listLabDB += fileInfo.baseName();
       }
    }
    ui->cbUser->addItem("administrator");
    ui->cbLabDatabase->addItems(m_listLabDB);
    ui->cbAuthorizeDB->addItem("none");
    ui->cbAuthorizeDB->addItems(m_listLabDB);

    QStringList stringlist;
    stringlist = getAllOperatorID("administrator");
    ui->cbOperatorID->addItems(stringlist);

    readLoginSetting();

}

LoginDlg::~LoginDlg()
{
    delete ui;
}

void LoginDlg::on_pbRegister_clicked()
{
    QString str_admin = ui->cbUser->currentText();
    QString str_password = ui->lePassword->text();

    if(!checkAdminPassword(str_admin,str_password)){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("请输入正确的系统管理员与密码！"));
        return;
    }
    if(ui->gbRegister->isVisible())
        ui->gbRegister->hide();
    else ui->gbRegister->show();
}

void LoginDlg::on_pbRegisterOK_clicked()
{

    QString str_admin = ui->cbUser->currentText();
    QString str_operator_id = ui->leOperatorID->text();
    QString str_operator_name = ui->leOperatorName->text();
    QString passWord = ui->leNewPassword->text();
    QString confirmPassword = ui->leConfirmPassword->text();
    QString Email = ui->leEmail->text();
    QString Tel = ui->leTelphone->text();


    if(str_operator_id.isEmpty()){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("操作员ID不能为空！"));
        return;
    }
    if(str_operator_name.isEmpty()){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("操作员姓名不能为空"));
        return;
    }
    if(passWord.isEmpty()){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("请输入密码！"));
        return;
    }
    if(confirmPassword.isEmpty()){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("请输入确认密码！"));
        return;
    }
    if(confirmPassword != passWord){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("两次输入的密码不一样，请重新输入！"));
        return;
    }

    if(!addOperatorRecord(str_admin,str_operator_id,str_operator_name,passWord,Email,Tel)){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("添加用户失败，该用户已经存在！"));
        return;
    }else{
        QString str_labname;
        str_labname = ui->cbAuthorizeDB->currentText();
        if(str_labname != "none"){
            if(addAuthorizeRecord(str_admin,str_operator_id,str_labname)){
                QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("恭喜你，已经成功添加操作员和完成授权！"));

            }else{
                QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("恭喜你，已经成功添加操作员，但是未完成授权！"));
            }
        }else{
             QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("恭喜你，已经成功添加操作员！"));
        }

         ui->cbOperatorID->addItem(str_operator_id);

    }


}

void LoginDlg::initShow()
{
    ui->gbRegister->hide();
    ui->cbUser->clear();
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->leNewPassword->setEchoMode(QLineEdit::Password);
    ui->leConfirmPassword->setEchoMode(QLineEdit::Password);
    ui->lePassword->setEchoMode(QLineEdit::Password);
    ui->lbOperatorID2->hide();
    ui->cbOperatorID->hide();
    ui->pbAuthorizeOK->hide();
}

void LoginDlg::setInitData(const QStringList &value)
{
    ui->cbLabDatabase->addItems(value);
}

void LoginDlg::on_pbLogin_clicked()
{
    m_enUserType =(enUserType) ui->cbUserType->currentIndex();
    QString username = ui->cbUser->currentText();
    QString password = ui->lePassword->text();
    QString dbname = ui->cbLabDatabase->currentText();
    QString path_db = documentsDir()+dbname+".db";
    if(m_enUserType == Administrator){
        if(!checkAdminPassword(username,password)){
            QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("请输入正确的系统管理员与密码！"));
            return;
        }
        if(!connectDB(path_db,"","")){
            QSqlDatabase::database(path_db).close();
            QSqlDatabase::removeDatabase(path_db);
            return;
        }

    }else{

        if(username.isEmpty()){
            QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("请输入用户名！"));
            return;
        }
        if(password.isEmpty()){
            QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("请输入密码！"));
            return;
        }

        if(!checkOperatorPassword("administrator",username,password)){
            QMessageBox::critical(0, "磁弹性传感器检测平台",QObject::tr("没有该操作员或者密码错误"));
                          return;
        }

        if(!connectDB(path_db,"","")){
            QSqlDatabase::database(path_db).close();
            QSqlDatabase::removeDatabase(path_db);
            return;
        }

        if(!checkHasAuthorize("administrator",username,dbname)){
              QMessageBox::critical(0, "磁弹性传感器检测平台",QObject::tr("没有该操作员没有操作该数据库权限"));
                            return;
        }
    }

    user_data data;
    data.dbPathName = path_db;
    data.dbName = dbname;
    data.userName = username;
    set_user_data(data);
    accept();

}

void LoginDlg::onCurrentIndexChanged(int index)
{
    m_enUserType = (enUserType)index;
    ui->lePassword->clear();
    if(m_enUserType == Administrator){
        ui->pbRegister->show();
        ui->pbAuthorize->show();
        ui->pbCreateNewDB->show();
        ui->cbUser->clear();
        ui->cbUser->addItem("administrator");
        ui->cbLabDatabase->addItems(m_listLabDB);
    }else{
        ui->gbRegister->hide();
        ui->pbRegister->hide();
        ui->pbAuthorize->hide();
        ui->lbOperatorID2->hide();
        ui->cbOperatorID->hide();
        ui->pbAuthorizeOK->hide();
        ui->pbCreateNewDB->hide();
        ui->cbUser->clear();
        ui->cbOperatorID->clear();
        QStringList list;
        list = getAllOperatorID("administrator");
        ui->cbUser->addItems(list);
        ui->cbOperatorID->addItems(list);
    }
}


void LoginDlg::onAuthorizeClicked(bool checked)
{
    QString str_admin = ui->cbUser->currentText();
    QString str_password = ui->lePassword->text();

    if(!checkAdminPassword(str_admin,str_password)){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("请输入正确的系统管理员与密码！"));
        return;
    }

    if(checked){
        ui->pbCreateNewDB->hide();
        ui->lbOperatorID2->show();
        ui->cbOperatorID->show();
        ui->pbAuthorizeOK->show();
    }else{

        ui->lbOperatorID2->hide();
        ui->cbOperatorID->hide();
        ui->pbAuthorizeOK->hide();
        ui->pbCreateNewDB->show();
    }
}


void LoginDlg::onCreateNewDB()
{
    QString str_admin = ui->cbUser->currentText();
    QString str_password = ui->lePassword->text();

    if(!checkAdminPassword(str_admin,str_password)){
        QMessageBox::critical(0,"磁弹性传感器检测平台",QObject::tr("请输入正确的系统管理员与密码！"));
        return;
    }

    bool ok;
    QString dbname=QInputDialog::getText(this,tr("新建数据库"),tr("请输入数据库名称："),
                                       QLineEdit::Normal,"",&ok);
    if(ok&&!dbname.isEmpty()){
        QString path_db = documentsDir()+dbname+".db";
        if(!connectDB(path_db,"","")){
            QSqlDatabase::database(path_db).close();
            QSqlDatabase::removeDatabase(path_db);
            QMessageBox::information(0, "磁弹性传感器检测平台",QObject::tr("很遗憾，无法创建该数据库"));
            return;
        }

        if(!addLabDBRecord(str_admin,dbname)||!createOperatorDB(dbname)){
            QMessageBox::information(0, "磁弹性传感器检测平台",QObject::tr("很遗憾，无法创建该数据库"));
            return;
        }else{
            QMessageBox::information(0, "磁弹性传感器检测平台",QObject::tr("恭喜你，已经成功创建数据库！"));
            m_listLabDB<<dbname;
            ui->cbLabDatabase->addItem(dbname);

        }
    }

}

void LoginDlg::onAuthorizeOKClicked()
{
  QString str_admin = ui->cbUser->currentText();
  QString str_operator_id = ui->cbOperatorID->currentText();
  QString str_labdb_name = ui->cbLabDatabase->currentText();

  if(addAuthorizeRecord(str_admin,str_operator_id,str_labdb_name)){
      QMessageBox::information(0, "磁弹性传感器检测平台",QObject::tr("恭喜你，已经成功完成授权！"));
      ui->pbAuthorize->setChecked(false);
      onAuthorizeClicked(false);
  }else{
      QMessageBox::information(0, "磁弹性传感器检测平台",QObject::tr("很遗憾，未能完成授权！"));
  }
}


void LoginDlg::onUserIndexChanged(QString str_user)
{
    QStringList strlist;
    strlist = getHasAuthorize("administrator",str_user);
    ui->cbLabDatabase->clear();
    ui->cbLabDatabase->addItems(strlist);
}


void LoginDlg::readLoginSetting()
{
    QSettings loginSetting("ThromboElastoGram","LoginDlg");
    QString str_user_type = loginSetting.value("user_type").toString();
    QString str_user_name = loginSetting.value("user_name").toString();
    QString str_lab_db = loginSetting.value("lab_db").toString();
    QString str_operator_id = loginSetting.value("operator_id").toString();

    ui->cbUserType->setCurrentText(str_user_type);
    ui->cbUser->setCurrentText(str_user_name);
    ui->cbLabDatabase->setCurrentText(str_lab_db);
    ui->cbOperatorID->setCurrentText(str_operator_id);

}

void LoginDlg::writeLoginSetting()
{
    QSettings loginSetting("ThromboElastoGram","LoginDlg");
    loginSetting.setValue("user_type",ui->cbUserType->currentText());
    loginSetting.setValue("user_name",ui->cbUser->currentText());
    loginSetting.setValue("lab_db",ui->cbLabDatabase->currentText());
    loginSetting.setValue("operator_id",ui->cbOperatorID->currentText());
}

void LoginDlg::closeEvent(QCloseEvent *e)
{
    writeLoginSetting();
}





