#include "mainwindow.h"
#include <QApplication>
#include "logindlg.h"
#include "fdatatbase.h"


MainWindow *pMainWindow;
void messageHandler(QtMsgType type, const QMessageLogContext &context,
                   const QString &msg)
{
   // TODO: don't call MainWindow::messageHandler if window is destroyed
   pMainWindow->messageHandler(type, context, msg);
}
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int return_code = -1;


    //连接到系统管理员数据库
    QString str_admin_db = documentsDir()+"administrator.db";
    QString str_user_name = "Administrator";
    QString str_password = "bme";
    if(!connectDB(str_admin_db,str_user_name,str_password)){
        QSqlDatabase::database(str_admin_db).close();
        QSqlDatabase::removeDatabase(str_admin_db);
        return return_code;
    }


    //创建系统管理员数据表
    if(QSqlDatabase::database(str_admin_db).tables().isEmpty()){
        if(!createAdministratorDB(str_admin_db)){
            QSqlDatabase::database(str_admin_db).close();
            QSqlDatabase::removeDatabase(str_admin_db);
            return return_code;
        }

    }


    LoginDlg loginDlg;

    if(loginDlg.exec()==QDialog::Accepted)
    {
        MainWindow w;
        pMainWindow = &w;
        qInstallMessageHandler(messageHandler);
        w.show();
        return_code = app.exec();
    }

    QSqlDatabase::database(str_admin_db).close();
    QSqlDatabase::removeDatabase(str_admin_db);
    return return_code;

}
