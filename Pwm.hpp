#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

#include <string>
#include <fstream>

#define PWM1A "P9_14"
#define PWM1B "P9_16"
#define PWM2A "P8_19"
#define PWM2B "P8_13"

class Pwm{

public:
	Pwm(const std::string& pin, long periodNS, long dutyNS);
	~Pwm();

	void SetPolarity(bool state);

	void SetPeriod(long timeNS);

	void SetDuty(long timeNS);

private:
	std::string m_sPin;
	std::string m_sPinPath;
	std::ofstream m_files[3];

	static std::string m_sCapePath;
	static std::string m_sOCPPath;
};




#endif

