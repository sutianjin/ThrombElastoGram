#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QtWidgets/QDialog>
#include <QMainWindow>

class QTextBrowser;
class QPrinter;
class QCheckBox;

class PrintDialog : public QDialog
{
    Q_OBJECT
    public:
        PrintDialog(const QString& src, QWidget* parent=0);
        ~PrintDialog();
    public slots:
        void slot_print();
        void slot_print_preview();
        void printPreview(QPrinter *printer);
        void slot_print_to_file();
        void slot_close();
    private:
        QTextBrowser *ptxtBrowser;
        QString srcTxt;
        QCheckBox *bchek;
};

#endif // PRINTDIALOG_H
