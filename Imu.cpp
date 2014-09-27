#include "Imu.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include <fcntl.h>
#include <math.h>

#include <stropts.h>


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

	//Configure stream to message-nondiscard mode
	// read ends when the specified number of bits are read or when got a "message end"
	// TODO: Dont seem to work correctly
//	ioctl(m_tty, I_SRDOPT, RMSGN);
}
Imu::~Imu(){
	close(m_tty);
}


void Imu::Query()
{
	//Send F
	printf("Send F\n");
	write(m_tty, "F", 1);
	//fsync(m_tty);

	// TODO: Because ioctl dont work, wait for answer to be received
	usleep(100000);

	IMUDataHead dataHead;
//	printf("%d %d\n", sizeof(dataHead), sizeof(m_lastData));
	int iBit = 0;
	do{


		//Read data head (Command & length)
		int nRead = read(m_tty, &dataHead, sizeof(dataHead));

		if(nRead<=0){
			printf("Head read error %d\n", nRead);
			return;
		}


		printf("Head = %d bits: Cmd=%c Length=%d\n", iBit+=2, dataHead.Cmd, dataHead.Length);


		if(dataHead.Cmd == 'F'){
			//Read the rest of the data
			nRead = read(m_tty, &m_lastData, sizeof(m_lastData));
			if(nRead == sizeof(m_lastData)){
				//Nothing to do, m_lastData contains everything now

				//Heading
				//printf("Psi=%.2f\n", m_lastData.Psi*180.0/M_PI);
				printf("H%f P%f R%f\n", Heading(), Pitch(), Roll());
				return;
			}
			else if(nRead!=0){
				printf("Read count mismatch: %d should be %d\n", nRead, sizeof(m_lastData));
			}
			else if(nRead<0){
				printf("Read error\n");
			}
		}
		else{
			//Garbage data
			printf("Garbage data :/\n");
		}
	}while(dataHead.Cmd != 'F');

}
