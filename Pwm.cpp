#include "Pwm.hpp"


#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

bool Pwm::m_bCapeInit = false;



Pwm::Pwm(const string& pin, const string& suffix, long periodNS, long dutyNS){
	m_sPin = pin;
	if(!m_bCapeInit){
		system("echo am33xx_pwm > /sys/devices/bone_capemgr.8/slots");
		m_bCapeInit = true;
	}
	//Activate pwm
	system(string("echo bone_pwm_"+m_sPin+" > /sys/devices/bone_capemgr.8/slots").c_str());

	m_files[0].open(string("/sys/devices/ocp.3/pwm_test_"+m_sPin+suffix+"/polarity"), ios_base::out);
	m_files[1].open(string("/sys/devices/ocp.3/pwm_test_"+m_sPin+suffix+"/period"), ios_base::out);
	m_files[2].open(string("/sys/devices/ocp.3/pwm_test_"+m_sPin+suffix+"/duty"), ios_base::out);

	if(m_files[0].is_open()+m_files[1].is_open()+m_files[2].is_open()<3)
		cout<<"Error: Some PWM files could not be opened (pin="<<m_sPin+suffix<<") "<<endl;


	SetPolarity(0);
	SetPeriod(periodNS);
	SetDuty(dutyNS);
}

Pwm::~Pwm(){
	for(auto& file : m_files)
		file.close();
}

void Pwm::SetPolarity(bool state){
	//system("echo "+std::tostring(state)+" > /sys/devices/ocp.3/pwm_test_"+m_sPin+"/polarity");
	m_files[0]<<(int)state;
	m_files[0].flush();
}
void Pwm::SetPeriod(long timeNS){
	//system("echo "+std::to_string(timeNS)+" > /sys/devices/ocp.3/pwm_test_"+m_sPin+"/period");
	m_files[1]<<timeNS;
	m_files[1].flush();
}
void Pwm::SetDuty(long timeNS){
	//system("echo "+std::to_string(timeNS)+" > /sys/devices/ocp.3/pwm_test_"+m_sPin+"/duty");
	m_files[2]<<timeNS;
	m_files[2].flush();
}
