#include <QTextBrowser>
#include <QDate>
#include <QCheckBox>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QSettings>
#include <QTranslator>
#include <QDockWidget>
#include <QToolButton>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QShowEvent>
#include <QPushButton>
#include <QBoxLayout>


#include "printdialog.h"

/////////////////////////////////////////////////////

PrintDialog::PrintDialog(const QString& src, QWidget* parent)
    :QDialog(parent)
{
    QPushButton *printB = new QPushButton(tr("打印.."));
    QPushButton *printPrv = new QPushButton(tr("打印预览."));
    QPushButton *saveB = new QPushButton(tr("保存为文件.."));
    QPushButton *closeB = new QPushButton(tr("关闭"));
    ptxtBrowser = new QTextBrowser;
    bchek = new QCheckBox(tr("保存为图像"));
    bchek->setChecked(true);

    ptxtBrowser->setText(src);
    srcTxt = src;

    connect(printPrv, SIGNAL(clicked()),this, SLOT(slot_print_preview()));
    connect(printB,SIGNAL(clicked()),this,SLOT(slot_print()));
    connect(saveB,SIGNAL(clicked()),this,SLOT(slot_print_to_file()));
    connect(closeB,SIGNAL(clicked()),this,SLOT(slot_close()));

    QVBoxLayout *pvbxLayout = new QVBoxLayout;
    QHBoxLayout *phbxLayout = new QHBoxLayout;
    phbxLayout->addWidget(printB);
    phbxLayout->addWidget(printPrv);
    phbxLayout->addStretch(7);
    phbxLayout->addWidget(saveB);
    phbxLayout->addWidget(bchek);
    phbxLayout->addStretch(100);
    phbxLayout->addWidget(closeB);
    pvbxLayout->addWidget(ptxtBrowser);
    pvbxLayout->addLayout(phbxLayout);
    setLayout(pvbxLayout);

    setWindowTitle(tr("打印预览"));

    setMinimumSize(750,500);
}

PrintDialog::~PrintDialog()
{
    delete ptxtBrowser;
}

void PrintDialog::slot_print_preview()
{
    #ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
        preview.exec();
    #endif
        accept();

}

void PrintDialog::printPreview(QPrinter *printer)
{
    ptxtBrowser->print(printer);
}

void PrintDialog::slot_print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print report"));

    if (dialog->exec() != QDialog::Accepted) {
        return;
    }

    ptxtBrowser->print(&printer);
#endif

    accept();
}

void PrintDialog::slot_print_to_file()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save report..."),
                                              QString(), tr("HTML-Files (*.htm *.html)"));
    if (fn.isEmpty()) {
        return;
    }

    if (fn.indexOf(".") == -1) {
        fn+=".html";
    }

    if (bchek->isChecked())	{
        QPixmap pix1(":/img/cash-add.png");
        QPixmap pix2(":/img/cash-delete.png");

        QString path = fn.mid(0,fn.lastIndexOf("/"));
        QString name = fn.section("/",-1);
        name.remove(".");
        QDir dir(path);
        dir.mkdir(name+"_files");

        pix1.save(dir.path()+"/"+name+"_files/add.png","PNG");
        pix2.save(dir.path()+"/"+name+"_files/rem.png","PNG");

        srcTxt.replace(":/img/cash-add.png",name+"_files/add.png");
        srcTxt.replace(":/img/cash-delete.png",name+"_files/rem.png");
    } else {
        srcTxt.remove("<img src=\":/img/cash-add.png\">");
        srcTxt.remove("<img src=\":/img/cash-delete.png\">");
    }

    QFile file(fn);
    if (!file.open(QFile::WriteOnly)) {
        return;
    }

    QTextStream ts(&file);
    ts.setCodec(QTextCodec::codecForName("UTF-8"));
    ts << srcTxt;
    file.close();

    accept();
}

void PrintDialog::slot_close()
{
    reject();
}

