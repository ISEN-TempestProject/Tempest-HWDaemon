#include "socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>

struct sockaddr_un addressServer;
int socketClient=-1, socketServer=-1;
pthread_t thread;
int term;



void* SocketThread(){
	while(!term){
		//On attend qu'un client se connecte au serveur
		printf("Waiting for client...\n");
		socketClient = accept(socketServer, NULL, NULL);
		if(socketClient<0){
			printf("Accept error\n");
			continue;
		}
		printf("Client is connected\n");

		//On lit ce qu'il raconte
		while(!term){

			//Réception des données
			char buffer[32];
			int nReceivedBytes=recv(socketClient, buffer, sizeof(buffer), MSG_WAITALL);
			if(nReceivedBytes>0){
				SocketHandleReceivedEvent(*((struct Event*)(buffer)));
			}
			else
				break;
		}
		socketClient = -1;
		printf("Client closed connection\n");
	}
	return 0;
}


int SocketHandleClients(){
	term = 0;
int created = pthread_create(&thread, NULL, SocketThread, NULL);
	if(created<0){
		printf("Unable to start socket thread");
		return created;
	}
	return 0;
}

void SocketClose(){
	printf("Closing sockets\n");
    if(socketClient>=0)
        close(socketClient);
    if(socketServer>=0)
        close(socketServer);

	printf("Socket closed\n");

	if(thread!=0){

		term = 1;
		//pthread_join(thread, NULL);
	}
	system("rm /tmp/hwsocket>/dev/null");
}


int SocketInit(){
	//Suppression de l'ancienne socket
	system("rm /tmp/hwsocket>/dev/null");

	//Init de la socket
	socketServer = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if(socketServer < 0){
		printf("Unable to init socket\n");
		return socketServer;
	}

	//Préparation de l'adresse côté serveur
	memset(&addressServer, 0, sizeof(addressServer));
	addressServer.sun_family = AF_UNIX;
	strncpy(addressServer.sun_path, "/tmp/hwsocket", sizeof(addressServer.sun_path)-1);

	//Lien entre la socket et l'adresse
	int binded = bind(socketServer, (struct sockaddr*)&addressServer, sizeof(addressServer));
	if(binded<0){
		printf("Unable to bind socket\n");
		return binded;
	}

	//On écoute la socket pour attendre une connexion
	int listened = listen(socketServer, 1);
	if(listened<0){
		printf("Unable to listen to socket\n");
		return listened;
	}

	printf("Listening to socket /tmp/hwsocket ...\n");

	return 0;
}

void SocketSendEvent(struct Event ev){
	if(socketClient>=0)
		send(socketClient, &ev, sizeof(ev), MSG_DONTWAIT);
}


unsigned short ConvertToSailValue(uint64_t data[2]){
	return data[0];
}

float ConvertToHelmValue(uint64_t data[2]){
	return (float)(90.0*data[0]/UINT64_MAX)-45.0;
}


void SocketSendGps(double latitude, double longitude){
	//lat -90 : 90
	//lon -180 : 180
	if(latitude<-90 || latitude>90 || longitude<-180 || longitude>180){
		printf("WARNING: GPS value out of bound (%f|%f)\n", latitude, longitude);
		return;
	}

	struct Event ev;
	ev.id = DEVICE_ID_GPS;
	ev.data[0] = (uint64_t)((latitude+90.0)*(UINT64_MAX/180.0));
	ev.data[1] = (uint64_t)((longitude+180.0)*(UINT64_MAX/360.0));
	SocketSendEvent(ev);
}
void SocketSendRoll(double angle){
	//angle -180 : 180
	if(angle<-180 || angle>180){
		printf("WARNING: Roll value out of bound (%f)\n", angle);
		return;
	}
	struct Event ev;
	ev.id = DEVICE_ID_ROLL;
	ev.data[0] = (uint64_t)((angle+180.0)*UINT64_MAX/360.0);
	ev.data[1] = 0;
	SocketSendEvent(ev);
}
void SocketSendWindDir(double angle){
	//angle -180 : 180
	if(angle<-180 || angle>180){
		printf("WARNING: WindDir value out of bound (%f)\n", angle);
		return;
	}
	struct Event ev;
	ev.id = DEVICE_ID_WINDDIR;
	ev.data[0] = (uint64_t)((angle+180.0)*UINT64_MAX/360.0);
	ev.data[1] = 0;
	SocketSendEvent(ev);
}
void SocketSendCompass(double angle){
	//angle 0 : 360
	if(angle<0 || angle>360){
		printf("WARNING: Compass value out of bound (%f)\n", angle);
		return;
	}
	struct Event ev;
	ev.id = DEVICE_ID_COMPASS;
	ev.data[0] = (uint64_t)((angle)*UINT64_MAX/360.0);
	ev.data[1] = 0;
	SocketSendEvent(ev);
}
void SocketSendBattery(float voltage){
	//voltage 0 : 18
	if(voltage<0 || voltage>10){
		printf("WARNING: Battery value out of bound (%f)\n", voltage);
		return;
	}
	struct Event ev;
	ev.id = DEVICE_ID_BATTERY;
	ev.data[0] = (uint64_t)((voltage)*UINT64_MAX/10.0);
	ev.data[1] = 0;
	SocketSendEvent(ev);
}







