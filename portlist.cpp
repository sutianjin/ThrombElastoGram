#include <QVariant>
#include <QIcon>
#include <QtDebug>

#include "portlist.h"

PortListItem::PortListItem(QString name, QString description, quint16 vid, quint16 pid)
{
    construct(name, description, vid, pid);
}

PortListItem::PortListItem(QSerialPortInfo* portInfo)
{
    if (portInfo->hasProductIdentifier())
    {
        construct(portInfo->portName(),
                  portInfo->description(),
                  portInfo->vendorIdentifier(),
                  portInfo->productIdentifier());
    }
    else
    {
        construct(portInfo->portName());
    }
}

void PortListItem::construct(QString name, QString description, quint16 vid, quint16 pid)
{
    QString text = name;
    if (!description.isEmpty())
    {
       // text += QString(" ") + description;
    }
    if (vid && pid)
    {
        setIcon(QIcon(":/CurveImage/usb_icon.png"));
    }
    else if (name.contains("rfcomm"))
    {
        setIcon(QIcon(":/CurveImage/bluetooth_icon.png"));
    }else
    {
        setIcon(QIcon(":/new/toolbar/toolbarImage/rs232_icon.png"));
    }
    setText(text);
    setData(name, PortNameRole);
}

QString PortListItem::portName()
{
    return data(PortNameRole).toString();
}

PortList::PortList(QObject* parent) :
    QStandardItemModel(parent)
{
    loadPortList();

    // we have to use macro based notation to be able to disconnect
    QObject::connect(this, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                     this, SLOT(onRowsInserted(QModelIndex, int, int)));
}

void PortList::loadPortList()
{
    clear();

    disconnect(this, SLOT(onRowsInserted(QModelIndex,int,int)));
    for (auto portInfo : QSerialPortInfo::availablePorts())
    {
        appendRow(new PortListItem(&portInfo));
    }
    for (auto portName : userEnteredPorts)
    {
        appendRow(new PortListItem(portName));
    }
    QObject::connect(this, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                     this, SLOT(onRowsInserted(QModelIndex, int, int)));
}

int PortList::indexOf(QString portText)
{
    for (int i = 0; i < rowCount(); i++)
    {
        if (item(i)->text() == portText)
        {
            return i;
        }
    }
    return -1; // not found
}

void PortList::onRowsInserted(QModelIndex parent, int start, int end)
{
    PortListItem* newItem = static_cast<PortListItem*>(item(start));
    QString portName = newItem->text();
    newItem->setData(portName, PortNameRole);
    userEnteredPorts << portName;
}
