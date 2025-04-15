#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "ui_mainwindow.h"

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
    void set_ui(Ui::MainWindow *Ui) {this->ui = Ui;}

signals:
    void newClientConnected(QString adr);
    void clientDisconnetced();
    void newMsg(QByteArray msg);

private slots:
    void slot_new_client();
    void slot_client_disconnetcted();
    void slot_newMsg();

private:

    bool is_Listening = false;
    int port = 12345;
    QTcpServer server;
    QTcpSocket* client = nullptr;
    Ui::MainWindow *ui;
};

#endif // SERVER_H
