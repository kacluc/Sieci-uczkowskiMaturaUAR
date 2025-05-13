#include "client.h"

CLIENT::CLIENT(QObject *parent)
    : QObject{parent}
    , socket(this)
{
    connect(&socket, SIGNAL(connected()), this, SLOT(slot_connected()));
    connect(&socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(slot_readyRead()));
}

void CLIENT::connectTo(QString address, int port)
{
    ip_address = address;
    port = port;
    this->is_connected = true;
    socket.connectToHost(ip_address, port);
}

void CLIENT::slod_disconnected()
{
    this->disconnect();
}

void CLIENT::disconnect()
{
    if(this->is_connected)
    {
        is_connected = false;
        try
        {
            socket.disconnectFromHost();
            socket.close();
        }
        catch (...) {}
        qInfo() << "Rozłączenie wewnątrz CLIENTA!!!!!!!!!!!!";
        emit this->disconnected();
    }
}

void CLIENT::sendMsg(QByteArray msg)
{
    socket.write(msg);
}

void CLIENT::slot_connected()
{
    emit connected("Clinet "+ip_address, port);
}

void CLIENT::slot_readyRead()
{
    auto message = socket.readAll();
    emit messageRecived(message);
}
