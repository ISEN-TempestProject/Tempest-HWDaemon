#include "Pwm.hpp"


#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

using namespace std;

string Pwm::m_sCapePath("");
string Pwm::m_sOCPPath("");


string FindDirContaining(const string& sParent, const string& sName){
	DIR* dir = opendir(sParent.c_str());
	if(dir==nullptr)
		return "";

	struct dirent* file;
	while ((file = readdir(dir)) != nullptr){
		if(string(file->d_name).find(sName)!=string::npos)
			return sParent+"/"+file->d_name;
	}
	closedir(dir);
	return "";
}


Pwm::Pwm(const string& pin, long periodNS, long dutyNS){
	m_sPin = pin;
	if(m_sCapePath==""){
		m_sCapePath = FindDirContaining("/sys/devices", "bone_capemgr");
		system(string("echo am33xx_pwm > "+m_sCapePath+"/slots").c_str());

		m_sOCPPath = FindDirContaining("/sys/devices", "ocp");
	}
	//Activate pwm on pin
	system(string("echo bone_pwm_"+m_sPin+" > "+m_sCapePath+"/slots").c_str());

	m_sPinPath = FindDirContaining(m_sOCPPath, "pwm_test_"+m_sPin);

	m_files[0].open(string(m_sPinPath+"/polarity"), ios_base::out);
	m_files[1].open(string(m_sPinPath+"/period"), ios_base::out);
	m_files[2].open(string(m_sPinPath+"/duty"), ios_base::out);

	if(m_files[0].is_open()+m_files[1].is_open()+m_files[2].is_open()<3)
		cout<<"Error: Some PWM files could not be opened (pinpath="<<m_sPinPath<<") "<<endl;

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
