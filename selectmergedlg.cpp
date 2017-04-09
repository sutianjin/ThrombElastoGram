#include "selectmergedlg.h"
#include "ui_selectmergedlg.h"

SelectMergeDlg::SelectMergeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectMergeDlg)
{
    ui->setupUi(this);
    QStringList testidstringlist;
    testidstringlist = get_all_testid();

    int recordNum = testidstringlist.size();
    if(recordNum<=0){
        return;
    }

     for(int i = 0; i < recordNum; i++){
         ui->listWidget->addItem(testidstringlist[i]);
     }


}

SelectMergeDlg::~SelectMergeDlg()
{
    delete ui;
}

void SelectMergeDlg::on_tbadd_clicked()
{
    QString currentID = ui->listWidget->currentItem()->text();

    int resultNum = ui->ResultListWidget->count();

    for(int i = 0; i < resultNum; i++){
        if(ui->ResultListWidget->item(i)->text()==currentID) return;
    }

    ui->ResultListWidget->addItem(currentID);


}

void SelectMergeDlg::on_tbdelete_clicked()
{
    ui->ResultListWidget->takeItem(ui->ResultListWidget->currentRow());
}

QStringList SelectMergeDlg::GetMergeString()
{
    QStringList list;
    
    int resultNum = ui->ResultListWidget->count();
    
    for(int i = 0; i < resultNum; i++){
        list.append(ui->ResultListWidget->item(i)->text());
    }
    
    return list;
    
}
