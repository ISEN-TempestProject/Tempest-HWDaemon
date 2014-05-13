#ifndef GPIO_HPP_INCLUDED
#define GPIO_HPP_INCLUDED

#include <fstream>
#include <string>

class Gpio{

public:
	enum Mode{
		INPUT, OUTPUT
	};

	Gpio(unsigned short nPin, Mode nMode);
	~Gpio();


	void SetMode(Mode mode);

	void SetValue(bool val);

	bool GetValue();


private:
	Mode m_mode;
	std::string m_pin;
	std::string m_pinpath;
	std::ifstream m_filer;
	std::ofstream m_filew;

};

#endif // GPIO_HPP_INCLUDED
