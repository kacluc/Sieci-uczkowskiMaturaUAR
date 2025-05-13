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
    void disconnect();
    bool isConnected() { return this->is_connected; }
    void sendMsg(QByteArray msg);

signals:
    void connected(QString adr, int port);
    void disconnected();
    void messageRecived(QByteArray msg);

private slots:
    void slot_connected();
    void slot_readyRead();
    void slod_disconnected();

private:
    bool is_connected = false;
    QTcpSocket socket;
    QString ip_address = "127.0.0.1";
    int port = 12345;
};

#endif // CLIENT_H
