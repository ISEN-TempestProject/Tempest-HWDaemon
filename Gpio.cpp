#include "Gpio.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
using namespace std;


Gpio::Gpio(unsigned short nPin, Mode nMode){
	std::stringstream ss;
	ss<<nPin;
	m_pin = ss.str();

	system(("echo "+m_pin+" > /sys/class/gpio/export").c_str());

	m_pinpath = "/sys/class/gpio/gpio"+m_pin;

	SetMode(nMode);
}

Gpio::~Gpio(){
	if(m_filew.is_open())
		m_filew.close();
	if(m_filer.is_open())
		m_filer.close();
}

void Gpio::SetMode(Mode mode){
	m_mode = mode;
	if(m_mode==INPUT){
		system(("echo in > "+m_pinpath+"/direction").c_str());
		system(("echo 1 > "+m_pinpath+"/active_low").c_str());

		if(m_filew.is_open())
			m_filew.close();
		m_filer.open(m_pinpath+"/value");
	}
	else{
		system(("echo out > "+m_pinpath+"/direction").c_str());
		system(("echo 0 > "+m_pinpath+"/active_low").c_str());

		if(m_filer.is_open())
			m_filer.close();
		m_filew.open(m_pinpath+"/value");
	}
}

void Gpio::SetValue(bool val){
	if(m_mode==OUTPUT){
		m_filew<<(int)val;
		m_filew.flush();
	}
	else{
		cout<<"Error: Cant set value of read only pin "<<m_pin<<"!"<<endl;
	}
}

bool Gpio::GetValue(){
	if(m_mode==INPUT){
		int ret;
		m_filer.seekg(0);
		m_filer>>ret;
		return ret>0;
	}
	else{
		cout<<"Error: Cant get value of write only pin "<<m_pin<<"!"<<endl;
	}
	return false;
}
