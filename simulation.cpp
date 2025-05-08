#include "simulation.h"
#include "connection.h"

Simulation::Simulation(QObject *parent)
    : QObject{parent}
{
    this->pid = std::make_unique<PID>();
    this->generator = std::make_unique<Generator>();
    this->arx = std::make_unique<ARX>();
    this->connection = new Connection{this};

    //connect(connection, SIGNAL(newMsg()), this, SLOT(recived_online_simulation()));
}

Simulation &Simulation::get_instance()
{
    static Simulation instance;
    return instance;
}

Simulation::~Simulation()
{
    this->killTimer(this->timer_id);
}

float Simulation::get_ticks_per_second() const
{
    return this->ticks_per_second;
}

void Simulation::increment_tick()
{
    this->tick++;
}

float Simulation::get_current_time() const
{
    return this->current_time;
}

size_t Simulation::get_tick() const
{
    return this->tick;
}

template<typename T>
void memcopy_s(void *dest, const T &src, size_t size = 1)
{
    std::memcpy(dest, &src, sizeof(T) * size);
}

void Simulation::simulate_local()
{
    this->tick++;
    const size_t tick = this->get_tick();

    // const float time = interval / 1000.0f ;
    this->current_time += interval / 1000.0f;

    generator_out = this->generator->run(current_time);

    error_output = generator_out - arx_output;

    pid_output = this->pid->run(error_output);

    arx_output = this->arx->run(pid_output);

    SimulationFrame frame{
        .tick = tick,
        .geneartor_output = generator_out,
        .p = this->pid->proportional_part,
        .i = this->pid->integral_part,
        .d = this->pid->derivative_part,
        .pid_output = pid_output,
        .error = error_output,
        .arx_output = arx_output,
        .noise = this->arx->noise_part,
    };

    this->frames.push_back(frame);
    this->draw_simulation();
}

void Simulation::simulate_online()
{
    if(!connection->values_avaiable())
    {
        //const size_t tick = this->get_tick();
        this->current_time += interval / 1000.0f;
        generator_out = this->generator->run(current_time);
        error_output = generator_out - arx_output;
        pid_output = this->pid->run(error_output);
        QByteArray msg;
        QDataStream stream(&msg, QIODevice::ReadWrite);
        stream << this->get_tick() << pid_output;
        connection->send_msg(msg);
        this->tick++;
        // działa qInfo() << "wysłanie wiadomości z symulacji";
    }
}

void Simulation::recived_online_simulation()
{
    qInfo() << "ASDASDASDASDASDASDASDASDASD";
    if(connection->get_connection_type() == Connected_as::client) qInfo() << "Obiekt Otrzymał wiadomość ZWROTNĄ";
    if(connection->get_connection_type() == Connected_as::server) qInfo() << "Regulator Otrzymał wiadomość ZWROTNĄ";
    switch(connection->get_connection_type())
    {
    default:
    case Connected_as::none: break;
    case Connected_as::server:
    {
        qInfo() << "tick: " << this->get_tick();
        qInfo() << "Ilość ramek: " << frames.size() << "     index: " << connection->get_last_readed_index();
        if(connection->values_avaiable())
        {
            auto msg = connection->get_values();
            arx_output = std::get<1>(msg);

            SimulationFrame frame{
                .tick = std::get<0>(msg),
                .geneartor_output = generator_out,
                .p = this->pid->proportional_part,
                .i = this->pid->integral_part,
                .d = this->pid->derivative_part,
                .pid_output = pid_output,
                .error = error_output,
                .arx_output = arx_output,
                .noise = this->arx->noise_part,
            };
            this->frames.push_back(frame);
            this->draw_simulation();
            qInfo() << "sieciowa odpowiedź regulatora";
        }
        break;
    }
    case Connected_as::client:
    {
        //if(connection->values_avaiable())
        {
            auto data = connection->get_values();
            float value = this->arx->run_t(data);
            qInfo() << "value " << value << "    data: " << std::get<1>(data);
            QByteArray msg; QDataStream stream(&msg, QIODevice::ReadWrite);
            stream << std::get<0>(data) << value;
            qInfo() << "sieciowa odpowiedź Obiektu  " << value << " " << std::get<1>(data);
            connection->send_msg(msg);
        }
        break;
    }
    }


}

void Simulation::draw_simulation()
{
    emit this->add_series("I", frames.last().i, ChartPosition::top);
    emit this->add_series("D", frames.last().d, ChartPosition::top);
    emit this->add_series("P", frames.last().p, ChartPosition::top);
    emit this->add_series("PID", frames.last().pid_output, ChartPosition::top);

    emit this->add_series("Error", frames.last().error, ChartPosition::middle);
    emit this->add_series("Noise", frames.last().noise, ChartPosition::middle);

    emit this->add_series("ARX", frames.last().arx_output, ChartPosition::bottom);
    emit this->add_series("Generator", frames.last().geneartor_output, ChartPosition::bottom);

    emit this->update_chart();
}

void Simulation::set_ticks_per_second(float ticks_per_second)
{
    this->ticks_per_second = ticks_per_second;
}

void Simulation::timerEvent(QTimerEvent *event)
{
    if(connection->is_connected())
    {
        this->simulate_online();
    }
    else
    {
        this->simulate_local();
    }
}

void Simulation::start()
{
    const int temp = std::max({this->interval, 30});

    this->timer_id = this->startTimer((temp));
    this->is_running = true;

    emit this->simulation_start();
}

void Simulation::reset()
{
    this->stop();

    this->tick = 0;
    this->current_time = 0;
    this->is_running = false;

    emit this->reset_chart();

    this->pid->reset();
    this->arx->reset();
    this->frames.clear();
}

void Simulation::stop()
{
    this->killTimer(this->timer_id);
    this->is_running = false;

    emit this->simulation_stop();
}

void Simulation::set_duration(float duration)
{
    this->durration = duration;
}

void Simulation::set_interval(int interval)
{
    this->interval = interval;
}

int Simulation::get_interval() const
{
    return this->interval;
}

struct SimulationDeserialized
{
    // simulation
    int interval;    // [0:3]
    float durration; // [4:7]

    // pid
    float pid_kp; // [8:11]
    float pid_ti; // [12:15]
    float pid_td; // [16:19]

    // generator
    float generator_amplitude;    // [20:23]
    float generator_frequency;    // [24:27]
    GeneratorType generator_type; // [28:31]

    // arx
    float arx_noise;          // [32:35]
    NoiseType arx_noise_type; // [36:39]
    size_t arx_delay;         // [40:43]
};

std::vector<std::byte> Simulation::serialize()
{
    constexpr size_t data_size = sizeof(SimulationDeserialized);
    const size_t vector_sizes = sizeof(float) * this->arx->get_a().size()
                                + sizeof(float) * this->arx->get_b().size() + (sizeof(size_t) * 2);

    std::vector<std::byte> data(data_size + vector_sizes);

    SimulationDeserialized deserialized{
        this->interval,
        this->durration,
        this->pid->get_kp(),
        this->pid->get_ti(),
        this->pid->get_td(),
        this->generator->get_amplitude(),
        this->generator->get_frequency(),
        this->generator->get_type(),
        this->arx->get_noise(),
        this->arx->get_noise_type(),
        this->arx->get_delay(),
    };

    std::byte *data_ptr = data.data();

    std::memcpy(data_ptr, &deserialized, data_size);
    data_ptr += data_size;

    size_t a_size = this->arx->get_a().size();
    memcpy(data_ptr, &a_size, sizeof(size_t));
    data_ptr += sizeof(size_t);

    qDebug() << "a_size: " << a_size;

    memcpy(data_ptr, this->arx->get_a().data(), sizeof(float) * a_size);
    data_ptr += sizeof(float) * this->arx->get_a().size();

    size_t b_size = this->arx->get_b().size();
    qDebug() << "b_size" << b_size;
    memcpy(data_ptr, &b_size, sizeof(size_t));
    data_ptr += sizeof(size_t);

    memcpy(data_ptr, this->arx->get_a().data(), sizeof(float) * this->arx->get_a().size());
    data_ptr += sizeof(float) * this->arx->get_a().size();

    qDebug() << "data size: " << data.size();

    return data;
}

void Simulation::deserialize(std::vector<std::byte> data)
{
    constexpr size_t data_size = sizeof(SimulationDeserialized);

    qDebug() << "data size: " << data.size();

    SimulationDeserialized deserialized;

    std::byte *data_ptr = data.data();

    std::memcpy(&deserialized, data_ptr, sizeof(SimulationDeserialized));
    data_ptr += sizeof(SimulationDeserialized);

    size_t a_size;
    memcpy(&a_size, data_ptr, sizeof(size_t));
    data_ptr += sizeof(size_t);

    qDebug() << "a_size: " << a_size;

    std::vector<float> a(a_size);
    memcpy(a.data(), data_ptr, sizeof(float) * a_size);
    data_ptr += sizeof(float) * a_size;

    size_t b_size;
    memcpy(&b_size, data_ptr, sizeof(size_t));
    data_ptr += sizeof(size_t);
    qDebug() << "b_size" << b_size;

    std::vector<float> b(b_size);
    memcpy(b.data(), data_ptr, sizeof(float) * b_size);

    this->set_interval(deserialized.interval);
    this->set_duration(deserialized.durration);
    this->pid->set_kp(deserialized.pid_kp);
    this->pid->set_ti(deserialized.pid_ti);
    this->pid->set_td(deserialized.pid_td);
    this->generator->set_amplitude(deserialized.generator_amplitude);
    this->generator->set_frequency(deserialized.generator_frequency);
    this->generator->set_type(deserialized.generator_type);
    this->arx->set_a(a);
    this->arx->set_b(b);
    this->arx->set_noise(deserialized.arx_noise);
    this->arx->set_noise_type(deserialized.arx_noise_type);
    this->arx->set_delay(deserialized.arx_delay);
}

void Simulation::set_outside_sum(bool is_outside_sum)
{
    this->pid->set_mode(is_outside_sum);
    this->is_outside_sum = is_outside_sum;
}

bool Simulation::get_outside_sum() const
{
    return this->is_outside_sum;
}
