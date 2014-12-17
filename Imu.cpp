#include "Imu.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include <fcntl.h>
#include <math.h>

#include <stropts.h>
#include <errno.h>


#include "utils.hpp"

using namespace std;

Imu::Imu(const std::string& tty, const std::string& BB_UARTX){
	assert(sizeof(IMUData)==74);

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
	write(m_tty, "F", 1);

	//Assemble the received message
	size_t nByteCount = 0;
	while(nByteCount<sizeof(m_lastData)){

		//Pad received bytes at the right place in the struct
		unsigned long addr = (unsigned long)(&m_lastData)+nByteCount;

		//Read from stream
		int nRead = read(m_tty, (void*)addr, sizeof(m_lastData)-nByteCount);

		if(nRead<0){
			//Oh my gosh ! THIS SHOULD NEVER HAPPEN !
			printf("Read error %d in %s:%d\n", errno, __FILE__, __LINE__);
			break;
		}
		nByteCount+=nRead;
		//printf("Read %d bytes. Total=%d\n", nRead, nByteCount);
	}

	//printf("H%f P%f R%f\n", Heading(), Pitch(), Roll());

}
