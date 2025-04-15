#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class CLIENT : public QObject
{
    Q_OBJECT
public:
    explicit CLIENT(QObject *parent = nullptr);
    void connectTo(QString address, int port);
    void disconnectFrom();
    bool isConnected() { return m_socket.isOpen(); }
    void sendMsg(QByteArray msg);

signals:
    void connected(QString adr, int port);
    void disconnected();
    void messageRecived(QByteArray msg);

private slots:
    void slot_connected();
    void slot_readyRead();

private:
    QTcpSocket m_socket;
    QString m_ipAddress = "127.0.0.1";
    int m_port = 12345;
};

#endif // CLIENT_H
