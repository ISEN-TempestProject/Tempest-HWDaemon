#include "Accelerometer.hpp"

Accelerometer::Accelerometer(unsigned short ain_x, unsigned short ain_y, unsigned short ain_z)
    :adc_x(ain_x), adc_y(ain_y), adc_z(ain_z)
{
}

Accelerometer::~Accelerometer()
{
}


float Accelerometer::roll()
{
    float x, y, z, roll;

    //Get 3-axis gravity
    x = adc_x.GetValue();
    y = adc_y.GetValue();
    z = adc_z.GetValue();

    //Get roll
    roll = (atan2(y, sqrt(x*x + z*z)) * 180.0) / PI;

    return roll;

}

float Accelerometer::pitch()
{
    float x, y, z, pitch;

    //Get 3-axis gravity
    x = adc_x.GetValue();
    y = adc_y.GetValue();
    z = adc_z.GetValue();

    //Get pitch
    pitch = ((x / sqrt(y*y + z*z)) * 180.0) / PI;

    return pitch;

}

//DEBUG
void Accelerometer::toast()
{
    float x, y, z, roll;

    //Get 3-axis gravity
    x = adc_x.GetValue();
    y = adc_y.GetValue();
    z = adc_z.GetValue();

    std::cout << x << ";" << y << ";" << z << std::endl;
}
