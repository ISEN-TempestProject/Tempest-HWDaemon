#ifndef ADC_HPP_INCLUDED
#define ADC_HPP_INCLUDED

#include <fstream>

class Adc{

public:
	Adc(unsigned short adc);
	~Adc();

	int Get();



private:
	std::ifstream m_pin;

	static std::string m_sCapePath;
	static std::string m_sOCPPath;

};

#endif // ADC_HPP_INCLUDED
