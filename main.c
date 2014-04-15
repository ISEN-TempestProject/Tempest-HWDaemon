#include <stdio.h>
#include "socket.h"

int main(int argc, char const *argv[])
{
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



	return 0;
}