#ifndef COMPASS_H_INCLUDED
#define COMPASS_H_INCLUDED

short int initialize(int i2c_bus, int adresse);

short int writeMode(int file);

short int writeRegA(int file);

short int writeRegB(int file);

short int readX(int file);

short int readY(int file);

short int readZ(int file);

short int zero(int file);


#endif // COMPASS_H_INCLUDED
