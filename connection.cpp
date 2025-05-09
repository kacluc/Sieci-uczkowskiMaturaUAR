#include "connection.h"
#include "connectdialog.h"
#include "simulation.h"
#include "mainwindow.h"
#include "algorithm"

Connection::Connection(QObject *parent)
    : QObject{parent}
{}

void Connection::disconnect()
{
    if(this->is_connected())
    {
        this->connection_status = false;
        if(this->server != nullptr)
        {
            server->disconnect();
            delete this->server;
            this->server = nullptr;
        }
        if(this->client != nullptr)
        {
            client->disconnectFrom();
            delete this->client;
            this->client = nullptr;
        }
    }
    this->connected_as = Connected_as::none;
    emit this->disconnected();
}

void Connection::send_msg(QByteArray msg)
{
    switch(connected_as)
    {
    default:
    case Connected_as::none: break;
    case Connected_as::server:
    {
        this->server->sendMsg(msg);
        // działa qInfo() << "wysłanie wiadomości switchcase regulator";
        break;
    }
    case Connected_as::client:
    {
        this->client->sendMsg(msg);
        //qInfo() << "wysłanie wiadomości switchcase obiekt";
        break;
    }
    }
}

void Connection::make_connection()
{
    auto& simulation = Simulation::get_instance();
    bool simulation_is_running{simulation.is_running};
    if(simulation.is_running) simulation.stop();
    ConnectDialog* window = new ConnectDialog();
    if(!!window->exec())
    {
        this->disconnect();
        if(window->get_connection_type())
        {
            this->server = new SERVER(this);
            bool tst = false;
            tst = this->server->startListening(window->get_Port());
            this->connected_as = Connected_as::server;
            connect(this->server, SIGNAL(newClientConnected(QString,int)), &MainWindow::get_instance(), SLOT(slot_connected(QString,int)));
            connect(this->server, SIGNAL(newMsg(QByteArray)), this, SLOT(new_message(QByteArray)));
            connect(this->server, SIGNAL(disconnected()), this, SLOT(slot_dissconect()));
            qInfo() << "server listening: " << tst;
        }
        else
        {
            this->client = new CLIENT(this);
            connect(this->client, SIGNAL(connected(QString,int)), &MainWindow::get_instance(), SLOT(slot_connected(QString,int)));
            connect(this->client, SIGNAL(messageRecived(QByteArray)), this, SLOT(new_message(QByteArray)));
            connect(this->client, SIGNAL(disconnected()), this, SLOT(slot_dissconect()));
            bool tst = true, tst2 = true;

            QHostAddress ipAdr(window->get_IP());
            if (ipAdr.protocol() != QAbstractSocket::IPv4Protocol) {
                tst = false;
            }
            if (window->get_Port() < 0 || 65535 < window->get_Port()) {
                tst2 = false;
            }

            this->connected_as = Connected_as::client;
            this->client->connectTo(window->get_IP(), window->get_Port());
            qInfo() << "ip: " << tst << " port: " << tst2;
        }
        connection_status = true;
    }
    else if(simulation_is_running) simulation.start();
    delete window;
}

void Connection::new_message(QByteArray msg)
{
    if(connected_as == Connected_as::client) qInfo() << "Obiekt Otrzymał wiadomość";
    if(connected_as == Connected_as::server) qInfo() << "Regulator Otrzymał wiadomość";
    QDataStream stream(&msg, QIODevice::ReadOnly);
    size_t tick; float value;
    stream >> tick >> value;
    this->gained_values.push_back(std::make_tuple(tick,value));
    if (this->values_avaiable())
    {
        std::sort(this->gained_values.begin(), this->gained_values.end(), [](const std::tuple<size_t, float>& fst, const std::tuple<size_t, float>& sec) {return std::get<0>(fst) < std::get<0>(sec); });
    }
    qInfo() << "wysłanie wiadomości z connection" << std::get<0>(gained_values[0]) << " " << std::get<1>(gained_values[0]);
    Simulation::get_instance().recived_online_simulation();
}

std::tuple<size_t,float> Connection::get_values()
{
    auto l1 = gained_values.length();
    auto values = this->gained_values[0];
    this->gained_values.pop_front();
    auto l2 = gained_values.length();;
    qInfo() << "Zwrócona wartość: " << std::get<1>(values) << " " << l1-l2;
    return values;
}

size_t Connection::get_last_readed_index()
{
    return (this->values_avaiable()) ? std::get<0>(this->gained_values[0]) : 0;
}

bool Connection::values_avaiable()
{
    return 0 < this->gained_values.length();
}

Connected_as Connection::get_connection_type()
{
    return this->connected_as;
}
