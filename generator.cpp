#include "generator.h"
#include <QDebug>
#include <QtMath>
#include <cmath>

Generator::Generator() {}

float Generator::run(float time)
{
    switch (this->type) {
    case GeneratorType::sine:
        return this->amplitude * qSin<float>(2 * M_PI * time / this->frequency);
    case GeneratorType::square: {
        const float tperiod = this->frequency * 2;

        const float infill_period = tperiod * this->infill / 100;
        const float time_modulo = fmod(time, tperiod);

        return this->amplitude * (time_modulo < infill_period ? 1 : 0);
    }
    case GeneratorType::triangle:
        return this->amplitude * qAsin<float>(qSin<float>(2 * M_PI * time / this->frequency));
    case GeneratorType::sawtooth:
        return this->amplitude * qAtan<float>(qTan<float>(M_PI * time / this->frequency));
    case GeneratorType::single_jump:
        return this->amplitude * (time < 1 ? 1 : 0);
    }

    return 0;
}

void Generator::set_frequency(float frequency)
{
    this->frequency = frequency;
}

void Generator::set_infill(float infill)
{
    this->infill = infill;
}

float Generator::get_infill() const
{
    return this->infill;
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
