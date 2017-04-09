
#include "systemsetupdlg.h"
#include "ui_systemsetupdlg.h"
#include "addsampletypedlg.h"



SystemSetupDlg::SystemSetupDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemSetupDlg)
{
    ui->setupUi(this);
    currentUserData = get_user_data();
    m_pSampleModel = new QSqlTableModel(this,QSqlDatabase::database(currentUserData.dbPathName));
    m_pSampleModel->setTable("sample_info");
    m_pSampleModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_pSampleModel->select();
    m_pSampleModel->setHeaderData(0, Qt::Horizontal, tr("样本类型"));
    m_pSampleModel->setHeaderData(1, Qt::Horizontal, tr("样本描述"));
    m_pSampleModel->setHeaderData(2, Qt::Horizontal, tr("备注"));
    ui->tvSampleType->setModel(m_pSampleModel);
    ui->tvSampleType->hideColumn(3);
    ui->tvSampleType->hideColumn(4);
    ui->tvSampleType->hideColumn(5);
    ui->tvSampleType->hideColumn(6);
    ui->tvSampleType->setColumnWidth(1,150);
    ui->tvSampleType->setColumnWidth(2,250);
    QObject::connect(ui->pbAddSampleType,SIGNAL(clicked()),this,SLOT(onAddSampleTypeClicked()));
    ui->tvSampleType->horizontalHeader()->resize(0,50);
    ui->tvSampleType->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
}

SystemSetupDlg::~SystemSetupDlg()
{

    delete ui;
    delete m_pSampleModel;
}

void SystemSetupDlg::onAddSampleTypeClicked()
{
    AddSampleTypeDlg addSampleTypedlg;
    if(addSampleTypedlg.exec()==QDialog::Accepted){
        m_pSampleModel->select();
        ui->tvSampleType->setModel(m_pSampleModel);

    }
}
