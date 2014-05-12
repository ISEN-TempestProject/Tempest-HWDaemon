#ifndef ADC_HPP_INCLUDED
#define ADC_HPP_INCLUDED

#include <fstream>

class Adc{

public:
	Adc(unsigned short adc);
	~Adc();

	/**
		@brief Returns the value of the ADC between 0 and 1 (for 0v to 1.8v)
	**/
	float GetValue();



private:
	std::ifstream m_pin;

	static std::string m_sCapePath;
	static std::string m_sOCPPath;

};

#endif // ADC_HPP_INCLUDED
