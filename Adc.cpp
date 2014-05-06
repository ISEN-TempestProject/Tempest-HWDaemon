#include "Adc.hpp"

#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include "utils.hpp"


using namespace std;


string Adc::m_sCapePath("");
string Adc::m_sOCPPath("");

Adc::Adc(unsigned short adc){
	if(m_sCapePath==""){
		m_sCapePath = FindDirContaining("/sys/devices", "bone_capemgr");
		system(string("echo cape-bone-iio > "+m_sCapePath+"/slots").c_str());

		m_sOCPPath = FindDirContaining("/sys/devices", "ocp");
	}

	string sPinPath = FindDirContaining(m_sOCPPath, "helper")+"/AIN"+char(0x30+adc);
	m_pin.open(sPinPath);
	if(!m_pin.is_open())
		cout<<"Error: ADC file could not be opened (pinpath="<<sPinPath<<") "<<endl;
}
Adc::~Adc(){
	m_pin.close();
}


int Adc::Get(){
	int ret;
	m_pin.seekg(0);
	m_pin>>ret;
	return ret;
}
