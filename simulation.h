#ifndef SIMULATION_H
#define SIMULATION_H

#include <QMainWindow>
#include <QObject>

#include "arx.h"
#include "generator.h"
#include "pid.h"

class Connection;

enum class ChartPosition {
    top,
    middle,
    bottom,
};

struct Point
{
    float x;
    float y;
};

struct SimulationFrame
{
    size_t tick;
    float geneartor_output;

    float p;
    float i;
    float d;
    float pid_output;

    float error;

    float arx_output;
    float noise;
};

class Simulation : public QObject
{
    Q_OBJECT
public:
    static Simulation &get_instance();

    void start();
    void stop();

    void set_ticks_per_second(float ticks_per_second);
    void set_duration(float duration);

    void set_outside_sum(bool is_outside_sum);
    bool get_outside_sum() const;

    size_t get_tick() const;
    float get_current_time() const;
    float get_ticks_per_second() const;

    void increment_tick();

    float durration{0};

    bool is_running{false};

    size_t duration_timer_id{0};

    void reset();

    void set_interval(int interval);
    int get_interval() const;

    std::unique_ptr<PID> pid;
    std::unique_ptr<Generator> generator;
    std::unique_ptr<ARX> arx;
    Connection* connection;

    QList<SimulationFrame> frames{};

    std::vector<std::byte> serialize();
    void deserialize(std::vector<std::byte> data);
    void recived_online_simulation();
signals:
    void simulation_start();
    void simulation_stop();

    void reset_chart();
    void update_chart();
    void add_series(QString series_name, float y, ChartPosition position);

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:
    //void recived_online_simulation();

private:
    void simulate();
    void simulate_local();
    void simulate_online();
    void draw_simulation();

    bool is_outside_sum{true};
    float ticks_per_second{60};
    size_t tick{0};
    float current_time{0};
    size_t timer_id{0};
    int interval{100};

    float error_output = 0;
    float arx_output = 0;
    float pid_output = 0;
    float generator_out = 0;

    explicit Simulation(QObject *parent = nullptr);
    ~Simulation();

signals:
};

#endif // SIMULATION_H
