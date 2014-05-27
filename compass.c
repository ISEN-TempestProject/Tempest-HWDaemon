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


short int writeMode(int file){

	i2c_smbus_write_byte_data(file, MODE, 0x00);
	return 0;
}

short int writeRegA(int file){
	i2c_smbus_write_byte_data(file, CRA, 0x70);
	return 0;
}

short int writeRegB(int file){
	i2c_smbus_write_byte_data(file, CRB, 0xa0);
	return 0;
}

short int readX(int file){

	i2c_smbus_write_byte(file, XOUT_H)<<8;
	i2c_smbus_write_byte(file, XOUT_L);
	short int data=0;
  	data = i2c_smbus_read_byte_data(file, XOUT_H)<<8;
  	data |= i2c_smbus_read_byte_data(file, XOUT_L);

  	return data + XOUT_offset;
}

short int readY(int file){
	i2c_smbus_write_byte(file, YOUT_H)<<8;
	i2c_smbus_write_byte(file, YOUT_L);
	short int data=0;
	data = i2c_smbus_read_byte_data(file, YOUT_H)<<8;
	data |= i2c_smbus_read_byte_data(file, YOUT_L);

	return data + YOUT_offset;
}


short int readZ(int file){
	i2c_smbus_write_byte(file, ZOUT_H)<<8;
	i2c_smbus_write_byte(file, ZOUT_L);
	short int data=0;
	data = i2c_smbus_read_byte_data(file, ZOUT_H)<<8;
	data |= i2c_smbus_read_byte_data(file, ZOUT_L);

	return data + ZOUT_offset;
}

short int zero(int file){
	XOUT_offset = -readX(file);
	YOUT_offset = -readY(file);
	ZOUT_offset = -readZ(file);
	return 0;
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
	//zero(file);
}


float GetCompass(float fRoll, float fPitch)
{
//	fRoll = -fRoll;
//	fPitch = -fPitch;

	short x, y, z;

	writeRegA(file);
	writeRegB(file);
	writeMode(file);

	x = readX(file);
	y = readY(file);
	z = readZ(file);

//	static short mx=0, my=0, mz=0,Mx=0,My=0,Mz=0;
//	if(x<mx)mx=x;
//	if(y<my)my=y;
//	if(z<mz)mz=z;
//	if(x>Mx)Mx=x;
//	if(y>My)My=y;
//	if(z>Mz)Mz=z;
//	printf("\t%d\t%d\t%d\t\t\t%d\t%d\t%d\n", mx, my, mz, Mx, My, Mz);

	float fCosRoll = cos(fRoll*PI/180.0);
	float fSinRoll = sin(fRoll*PI/180.0);
	float fCosPitch = cos(fPitch*PI/180.0);
	float fSinPitch = sin(fPitch*PI/180.0);

//	printf("%f\t%f\n", fRoll, fPitch);

	//roll = rotY
	float fX = x*fCosRoll + z*fSinRoll;
	float fY = y;
	float fZ = -x*fSinRoll + z*fCosRoll;

	//pitch = rotX
	float fX2 = fX;
	float fY2 = fY*fCosPitch - fZ*fSinPitch;
	float fZ2 = fY*fSinPitch + fZ*fCosPitch;


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

