#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern "C"{
	#include "socket.h"
}

#include "Pwm.hpp"
#include "Adc.hpp"


Pwm* pwmMainSail;
#define MAIN_SAIL_DUTY_MIN 600000
#define MAIN_SAIL_DUTY_MAX 2400000
Pwm* pwmSecondSail;
#define SECOND_SAIL_DUTY_MIN 600000
#define SECOND_SAIL_DUTY_MAX 2400000

Pwm* pwmHelm;
#define HELM_DUTY_MIN 600000.0
#define HELM_DUTY_MAX 2400000.0


void SocketHandleReceivedEvent(struct Event ev){
	printf("\e[2mVOUS DEVEZ REECRIRE %s DANS %s:%d\e[m\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);

	switch(ev.id){
		case DEVICE_ID_SAIL:
		{
			unsigned short val = ConvertToSailValue(ev.data);
			printf("Received Sail=%d\n", val);
			pwmMainSail->SetDuty(
								(MAIN_SAIL_DUTY_MAX-MAIN_SAIL_DUTY_MIN)*(val/255.0)+MAIN_SAIL_DUTY_MIN
								);
			pwmSecondSail->SetDuty(
								(SECOND_SAIL_DUTY_MAX-SECOND_SAIL_DUTY_MIN)*(val/255.0)+SECOND_SAIL_DUTY_MIN
								);
			break;
		}
		case DEVICE_ID_HELM:
		{
			float val = ConvertToHelmValue(ev.data);
			printf("Received Helm=%f\n", val);
			pwmHelm->SetDuty(
								(HELM_DUTY_MAX-HELM_DUTY_MIN)*(val+45.0)/90.0+HELM_DUTY_MIN
								);
			break;
		}
		default:
			printf("Received unhandled device value");
			break;
	}
}

int main(int argc, char const *argv[])
{
	pwmMainSail = new Pwm(PWM2A, 20000000, 1000000);
	pwmSecondSail = new Pwm(PWM2B, 20000000, 1000000);
	pwmHelm = new Pwm(PWM1A, 20000000, 1000000);

	Adc adc(0);


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

			printf("%d\n", adc.Get());

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
