#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class SERVER : public QObject
{
    Q_OBJECT
public:
    explicit SERVER(QObject *parent = nullptr);
    bool startListening(int port);
    bool isListening() { return is_Listening; }
    void stopListening();
    int getNumClients();
    void sendMsg(QByteArray msg);
    void disconnect();
signals:
    void newClientConnected(QString adr, int port);
    void disconnected();
    void newMsg(QByteArray msg);

private slots:
    void slot_new_client();
    void slot_disconnected();
    void slot_newMsg();

private:
    bool is_connected = false;
    bool is_Listening = false;
    int port = 12345;
    QTcpServer server;
    QTcpSocket* client = nullptr;
};

#endif // SERVER_H
