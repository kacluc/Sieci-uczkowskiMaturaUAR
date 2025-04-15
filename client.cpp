#include "client.h"

CLIENT::CLIENT(QObject *parent)
    : QObject{parent}
    , m_socket(this)
{
    connect(&m_socket, SIGNAL(connected()), this, SLOT(slot_connected()));
    connect(&m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(slot_readyRead()));
}

void CLIENT::connectTo(QString address, int port)
{
    m_ipAddress = address;
    m_port = port;
    m_socket.connectToHost(m_ipAddress, port);
}

void CLIENT::disconnectFrom()
{
    m_socket.close();
}

void CLIENT::sendMsg(QByteArray msg)
{
    m_socket.write(msg);
}

void CLIENT::slot_connected()
{
    emit connected(m_ipAddress, m_port);
}

void CLIENT::slot_readyRead()
{
    auto message = m_socket.readAll();
    emit messageRecived(message);
}
