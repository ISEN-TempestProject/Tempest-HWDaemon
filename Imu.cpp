#include "Imu.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include <fcntl.h>
#include <math.h>


#include "utils.hpp"

using namespace std;

Imu::Imu(const std::string& tty, const std::string& BB_UARTX){
	assert(sizeof(IMUDataHead)==2);
	assert(sizeof(IMUData)==72);

	string sCapeMgrPath = FindDirContaining("/sys/devices", "bone_capemgr");
	system(("echo "+BB_UARTX+" > "+sCapeMgrPath+"/slots").c_str());

	m_tty = open(tty.c_str(), O_RDWR | O_NOCTTY);

	struct termios termconf;
	if(tcgetattr(m_tty, &termconf)){
		printf("tcgetattr err");
		return;
	}
	if(cfsetispeed(&termconf, B115200)){
		printf("cfsetispeed err");
		return;
	}
	if(cfsetospeed(&termconf, B115200)){
		printf("cfsetospeed err");
		return;
	}

	termconf.c_iflag = 0;
	termconf.c_oflag = 0;
	termconf.c_lflag = 0;
	termconf.c_cc[VMIN] = 1;
	termconf.c_cc[VTIME] = 10;

	tcsetattr(m_tty, TCSANOW, &termconf);

}
Imu::~Imu(){
	close(m_tty);
}


void Imu::Query()
{
	//Send F
	write(m_tty, "F", 1);
	// fsync(m_tty);

	//Read data head (Command & length)
	IMUDataHead dataHead;
	read(m_tty, &dataHead, sizeof(dataHead));

	do{
		if(dataHead.Cmd == 'F'){
			//Read the rest of the data
			int nRead = read(m_tty, &m_lastData, sizeof(m_lastData));
			if(nRead == sizeof(m_lastData)){
				//Nothing to do, m_lastData contains everything now

				//Heading
				//printf("Psi=%.2f\n", m_lastData.Psi*180.0/M_PI);

				return;
			}
			else if(nRead!=0){
				printf("Read count mismatch: %d should be %d", nRead, sizeof(m_lastData));
			}
			else{
				printf("Read error\n");
			}
		}
		else{
			//Ignore data
			// TODO: should not allocate data
			uint8_t buf[dataHead.Length];
			read(m_tty, buf, dataHead.Length);
		}
	}while(dataHead.Cmd != 'F');

}
