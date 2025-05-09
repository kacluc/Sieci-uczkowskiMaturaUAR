#ifndef CONNECTION_H
#define CONNECTION_H

#include "client.h"
#include "server.h"
#include <QObject>

enum class Connected_as
{
    none,
    server,
    client,
};

class Connection : public QObject
{
    Q_OBJECT
public:

    void make_connection();
    void disconnect();
    bool is_connected() { return connection_status; }
    explicit Connection(QObject *parent = nullptr);

    bool values_avaiable();
    Connected_as get_connection_type();
    std::tuple<size_t,float> get_values();
    size_t get_last_readed_index();
    void send_msg(QByteArray msg);
signals:
    void newMsg();
    void disconnected();
private slots:
    void new_message(QByteArray msg);
    void slot_dissconect() {this->disconnect();}

private:
    Connected_as connected_as{Connected_as::none};
    QList<std::tuple<size_t,float>> gained_values;
    bool connection_status = false;
    SERVER* server = nullptr;
    CLIENT* client = nullptr;
};

#endif // CONNECTION_H
