#include "hdbexchangedlg.h"
#include "ui_hdbexchangedlg.h"

#include "fdatatbase.h"

HDBExchangeDlg::HDBExchangeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HDBExchangeDlg)
{
    ui->setupUi(this);

    QStringList databaseList;
    databaseList = getAllLabDB("administrator");
    ui->cbCreateDatabase->addItems(databaseList);
    QString currentDB = ui->cbCreateDatabase->currentText();
    OpenDatabase(currentDB);



}

HDBExchangeDlg::~HDBExchangeDlg()
{
    delete ui;
}


bool HDBExchangeDlg::OpenDatabase(const QString &dbname)
{
    QString path_db = documentsDir() + dbname + ".db";
    if(!connectDB(path_db,"","")){
        QSqlDatabase::database(path_db).close();
        QSqlDatabase::removeDatabase(path_db);
        return false;
    }
    QStringList list = QSqlDatabase::database(path_db).tables();
    ui->listWidget->addItems(list);
    return true;
}
void HDBExchangeDlg::on_tbadd_clicked()
{
    QString currentID = ui->listWidget->currentItem()->text();

    int resultNum = ui->ResultListWidget->count();

    for(int i = 0; i < resultNum; i++){
        if(ui->ResultListWidget->item(i)->text()==currentID) return;
    }

    ui->ResultListWidget->addItem(currentID);
}

void HDBExchangeDlg::on_tbdelete_clicked()
{
    ui->ResultListWidget->takeItem(ui->ResultListWidget->currentRow());
}
