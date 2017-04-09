#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>


namespace Ui {
class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

private slots:
    void on_pbRegister_clicked();
    void on_pbRegisterOK_clicked();

    void on_pbLogin_clicked();

    void onCurrentIndexChanged(int index);
    void onAuthorizeClicked(bool checked);
    void onCreateNewDB();
    void onAuthorizeOKClicked();
    void onUserIndexChanged(QString str_user);

protected:
    void closeEvent(QCloseEvent*);

private:
    Ui::LoginDlg *ui;

    enum enUserType{Administrator,Operator} m_enUserType;
    void initShow();  //初始化界面显示
    void setInitData(const QStringList& value);  //初始化用户名

    QStringList m_listLabDB;
    void writeLoginSetting();
    void readLoginSetting();
};

#endif // LOGINDLG_H
