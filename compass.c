#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#define PI 3.14159265

#define CRA 0x00
#define CRB 0x01
#define MODE 0x02
#define XOUT_H 0x03
#define XOUT_L 0x04
#define ZOUT_H 0x05
#define ZOUT_L 0x06
#define YOUT_H 0x07
#define YOUT_L 0x08

#include "compass.h"
#include "i2c-dev.h"


short int initialize(int i2c_bus, int adresse);
short int writeMode(int file);
short int writeRegA(int file);
short int writeRegB(int file);
short int readX(int file);
short int readY(int file);
short int readZ(int file);
short int zero(int file);

short int XOUT_offset = -20;//+1;
short int YOUT_offset = +54;//-15;
short int ZOUT_offset = +35;//-4;


short int initialize(int i2c_bus, int adresse){
	char filename[20];
	int file;
	sprintf(filename, "/dev/i2c-%d", i2c_bus);
	file = open(filename, O_RDWR);

	if(file<0){
		return -errno;
	}

	if(ioctl(file, I2C_SLAVE, adresse) < 0){
		return -errno;
	}

	return file;
}








char *end;
int i2c_bus, adresse, dadresse, size, file;

double angle;



void InitCompass()
{
	system("echo BB-I2C1 > /sys/devices/bone_capemgr.8/slots");

	i2c_bus = 1;
	adresse = 0x1e;
	file = initialize(i2c_bus, adresse);

	i2c_smbus_write_byte_data(file, 0, 0b01110000);//CRA
	i2c_smbus_write_byte_data(file, 1, 0b10100000);//CRB
	i2c_smbus_write_byte_data(file, 2, 0b00000000);//MR
}


float GetCompass(float fRoll, float fPitch)
{
//	fRoll = -fRoll;
//	fPitch = -fPitch;

	short x, y, z;

	x = (i2c_smbus_read_byte_data(file, 3)<<8 | i2c_smbus_read_byte_data(file, 4)) + XOUT_offset;
	z = (i2c_smbus_read_byte_data(file, 5)<<8 | i2c_smbus_read_byte_data(file, 6)) + ZOUT_offset;
	y = (i2c_smbus_read_byte_data(file, 7)<<8 | i2c_smbus_read_byte_data(file, 8)) + YOUT_offset;
//	printf("%d\t%d\t%d\n", x, y, z);

//	//CALIBRATION
//	static short mx=0, my=0, mz=0,Mx=0,My=0,Mz=0;
//	if(x<mx)mx=x;
//	if(y<my)my=y;
//	if(z<mz)mz=z;
//	if(x>Mx)Mx=x;
//	if(y>My)My=y;
//	if(z>Mz)Mz=z;
//	printf("\t%d\t%d\t%d\t\t\t%d\t%d\t%d\n", mx, my, mz, Mx, My, Mz);


//	printf("%f\t%f\n", fRoll, fPitch);
	float fCosRoll = cos(fRoll*PI/180.0);
	float fSinRoll = sin(fRoll*PI/180.0);
	float fCosPitch = cos(fPitch*PI/180.0);
	float fSinPitch = sin(fPitch*PI/180.0);

	//roll = rotY
//	float fX = x*fCosRoll + z*fSinRoll;
//	float fY = y;
//	float fZ = -x*fSinRoll + z*fCosRoll;
//
//	//pitch = rotX
//	float fX2 = fX;
//	float fY2 = fY*fCosPitch - fZ*fSinPitch;
//	float fZ2 = fY*fSinPitch + fZ*fCosPitch;


	float fX2 = x*fCosRoll + y*fSinPitch*fSinRoll + z*fCosPitch*fSinRoll;
	float fY2 = y*fCosPitch - z*fSinPitch;


	angle = atan2(fY2,fX2); //calcul du cap


	float declinationAngle = 0.0457;
	//angle -= declinationAngle;
	angle *= 180/PI;

	if (angle <0){   //correction du signe
		angle += 360.0;
	}
	else if(angle >= 360){
		angle -= 360.0;
	}

//	printf("%f\t%f\t%f\t==>\t%f\n", fX2, fY2, fZ2, angle);

	return angle;
}

