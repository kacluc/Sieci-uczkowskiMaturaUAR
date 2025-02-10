#include "generator.h"
#include <QtMath>
#include <cmath>
#include <QDebug>

Generator::Generator() {}

float Generator::run(float time)
{
    const float period = 100 / this->frequency ;
    // qDebug() << "period: " << period;

    switch (this->type) {
    case GeneratorType::sine:
        return this->amplitude * qSin<float>(2 * M_PI * period * time);
    case GeneratorType::square:
        return qSin<float>(2 * M_PI * period * time) > 0 ? this->amplitude
                                                         : -this->amplitude;
    case GeneratorType::triangle:
        return this->amplitude * qAsin<float>(qSin<float>(2 * M_PI * period * time));
    case GeneratorType::sawtooth:
        return this->amplitude * qAtan<float>(qTan<float>(M_PI * period * time));
    case GeneratorType::single_jump:
        return this->amplitude * (time < 1 ? 1 : 0);
    }
}

void Generator::set_frequency(float frequency)
{
    this->frequency = frequency;
}

void Generator::set_amplitude(float amplitude)
{
    this->amplitude = amplitude;
}

void Generator::set_type(GeneratorType type)
{
    this->type = type;
}

float Generator::get_frequency() const
{
    return this->frequency;
}

float Generator::get_amplitude() const
{
    return this->amplitude;
}

GeneratorType Generator::get_type() const
{
    return this->type;
}
