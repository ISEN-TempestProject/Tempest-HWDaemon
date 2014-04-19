#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern "C"{
	#include "socket.h"
}

#include "Pwm.hpp"


Pwm* pwmMainSail;
#define MAIN_SAIL_DUTY_MIN 1000000
#define MAIN_SAIL_DUTY_MAX 2000000
Pwm* pwmSecondSail;
#define SECOND_SAIL_DUTY_MIN 1000000
#define SECOND_SAIL_DUTY_MAX 2000000

Pwm* pwmHelm;
#define SECOND_SAIL_DUTY_MIN 1000000
#define SECOND_SAIL_DUTY_MAX 2000000


void SocketHandleReceivedEvent(struct Event ev){
	printf("\e[2mVOUS DEVEZ REECRIRE %s DANS %s:%d\e[m\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);

	switch(ev.id){
		case DEVICE_ID_SAIL:
		{
			unsigned short val = ConvertToSailValue(ev.data);
			printf("Received Sail=%d\n", val);
			pwmMainSail->SetDuty(
								(MAIN_SAIL_DUTY_MAX+MAIN_SAIL_DUTY_MIN)*(val/255.0)+MAIN_SAIL_DUTY_MIN
								);
			pwmSecondSail->SetDuty(
								(SECOND_SAIL_DUTY_MAX+SECOND_SAIL_DUTY_MIN)*(val/255.0)+SECOND_SAIL_DUTY_MIN
								);
			break;
		}
		case DEVICE_ID_HELM:
			printf("Received Helm=%f\n", ConvertToHelmValue(ev.data));
			break;

		default:
			printf("Received unhandled device value");
			break;
	}
}

int main(int argc, char const *argv[])
{
	pwmMainSail = new Pwm(PWM1A, ".10", 20000000, 1000000);
	pwmSecondSail = new Pwm(PWM1B, ".11", 20000000, 1000000);
	pwmHelm = new Pwm(PWM2A, ".12", 20000000, 1000000);


	int error = SocketInit();
	if(error==0){

		SocketHandleClients();

		//Boucle principale du programme
		while(1){

			//Envoi d'un evenement au hasard
			float value, lat, lon;
			switch(rand()%6){
				case 0:
					lat = 12.3456 + rand()%20;
					lon = 13.37 + rand()%20;
					printf("Sending GPS=(%f,%f)\n", lat, lon);
					SocketSendGps(lat, lon);
					break;
				case 1:
					value = 23.254 + rand()%20;
					printf("Sending Roll=%f\n", value);
					SocketSendRoll(value);
					break;
				case 2:
					value = 90.3456 + rand()%20;
					printf("Sending WindDir=%f\n", value);
					SocketSendWindDir(value);
					break;
				case 3:
					value = 12.3456 + rand()%20;
					printf("Sending Compass=%f\n", value);
					SocketSendCompass(value);
					break;
			}

			usleep(100000*(rand()%10+5));
		}
		SocketClose();
	}
	else{
		printf("Unable to init socket ! Error code %d",error);
	}


	delete pwmHelm;
	delete pwmMainSail;
	delete pwmSecondSail;
	return 0;
}
