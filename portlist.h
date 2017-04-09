#ifndef PORTLIST_H
#define PORTLIST_H

#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QSerialPortInfo>

enum PortListRoles
{
    PortNameRole = Qt::UserRole+1  // portName as QString
};

class PortListItem : public QStandardItem
{
public:
    PortListItem(QString name, QString description="", quint16 vid=0, quint16 pid=0);
    PortListItem(QSerialPortInfo* portInfo);

    QString portName(); // returns only the port name

private:
    // common constructor
    void construct(QString name, QString description="", quint16 vid=0, quint16 pid=0);
};

class PortList : public QStandardItemModel
{
    Q_OBJECT
public:
    PortList(QObject* parent=0);

    void loadPortList();
    int indexOf(QString portText); // return -1 if not found

private:
    QStringList userEnteredPorts;

private slots:
    void onRowsInserted(QModelIndex parent, int start, int end);
};

#endif // PORTLIST_H
