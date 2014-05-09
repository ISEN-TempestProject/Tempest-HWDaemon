#ifndef ACCELEROMETER_HPP_INCLUDED
#define ACCELEROMETER_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <cmath>
#include "Adc.hpp"

#define PI 3.14159265358979323846


class Accelerometer{

public:
	Accelerometer(unsigned short ain_x, unsigned short ain_y, unsigned short ain_z);
	~Accelerometer();

	float roll();
	float pitch();

	void toast();




private:
	Adc adc_x, adc_y, adc_z;

};

#endif // ACCELEROMETER_HPP_INCLUDED
