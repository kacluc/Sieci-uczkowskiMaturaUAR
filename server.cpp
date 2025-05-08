#include "server.h"

SERVER::SERVER(QObject *parent)
    : QObject{parent}
    , server(this)
{
    connect(&server, SIGNAL(newConnection()), this, SLOT(slot_new_client()));
}

bool SERVER::startListening(int port)
{
    if(!this->is_Listening)
    {
        if(this->port != port)
        {
            this->stopListening();
            this->port = port;
        }
        is_Listening = server.listen(QHostAddress::Any, port);
    }
    return is_Listening;
}

void SERVER::stopListening()
{
    if(this->isListening())
    {
        server.close();
        is_Listening = false;
    }
}

void SERVER::sendMsg(QByteArray msg)
{
    if(this->client != nullptr) this->client->write(msg);
}

void SERVER::slot_new_client()
{
    if(this->client == nullptr)
    {
        client = server.nextPendingConnection();
        connect(client, SIGNAL(disconnected()), this, SLOT(slot_client_disconnetcted()));
        connect(client, SIGNAL(readyRead()), this, SLOT(slot_newMsg()));
        this->startListening(this->port);
        emit newClientConnected("Server " + client->peerAddress().toString(), client->peerPort());
    }
}

void SERVER::slot_client_disconnetcted()
{
    client->disconnectFromHost();
    emit clientDisconnetced();
}

void SERVER::slot_newMsg()
{
    emit newMsg(this->client->readAll());
}

