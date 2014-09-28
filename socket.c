#include "socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

int term;

struct sockcs{
	int type;
	int client;
	int server;
};

struct sockcs* sockcsUnix=NULL;
struct sockcs* sockcsTcp=NULL;
pthread_t sockThreadUnix=0, sockThreadTcp=0;
struct sockaddr_un sockaddrUnix;
struct sockaddr_in sockaddrTcp;

void CloseSockCS(struct sockcs* sock){
	if(sock!=NULL){
		if(sock->client>=0)
			close(sock->client);
		if(sock->server>=0)
			close(sock->server);

		free(sock);
	}
}


void* SocketThread(void* args){
	struct sockcs* sock = args;
	return;
	while(!term){
		//On attend qu'un client se connecte au serveur
		printf("\e[32mWaiting for client on %s socket...\e[m\n", sock->type==AF_UNIX?"UNIX":"TCP");
		sock->client = accept(sock->server, NULL, NULL);
		if(sock->client<0){
			if(term) break;
			printf("\e[1;33mAccept error on %s socket\e[m\n", sock->type==AF_UNIX?"UNIX":"TCP");
			continue;
		}
		printf("\e[32mClient is connected on %s socket\e[m\n", sock->type==AF_UNIX?"UNIX":"TCP");

		//On lit ce qu'il raconte
		while(!term){

			//Réception des données
			char buffer[32];
			int nReceivedBytes=recv(sock->client, buffer, sizeof(buffer), MSG_WAITALL);
			if(nReceivedBytes>0){
				SocketHandleReceivedEvent(*((struct Event*)(buffer)));
			}
			else
				break;
		}
		sock->client = -1;
		printf("\e[1;33mClient closed connection on %s socket\e[m\n", sock->type==AF_UNIX?"UNIX":"TCP");
	}
	return 0;
}


void SocketHandleClients(){
	term = 0;

	int created = pthread_create(&sockThreadUnix, NULL, SocketThread, sockcsUnix);
	if(created<0){
		printf("\e[1;31;43mUnable to start UNIX socket thread\e[m");
	}

	created = pthread_create(&sockThreadTcp, NULL, SocketThread, sockcsTcp);
	if(created<0){
		printf("\e[1;31;43mUnable to start TCP socket thread\e[m");
	}
	return 0;
}


void SocketClose(){
	printf("\e[33mClosing sockets\e[m\n");
	CloseSockCS(sockcsUnix);
	CloseSockCS(sockcsTcp);

	printf("\e[33mSockets closed\e[m\n");

	if(sockThreadUnix!=0 || sockThreadTcp!=0)
		term = 1;

	system("if [ -e /tmp/hwsocket ]\nthen\nrm /tmp/hwsocket\nfi");
}


int SocketInit(){


	//UNIX SOCKET
	{
		sockcsUnix = malloc(sizeof(struct sockcs));
		sockcsUnix->type = AF_UNIX;


		//Suppression de l'ancienne socket
		system("if [ -e /tmp/hwsocket ]\nthen\nrm /tmp/hwsocket\nfi");

		//Init de la socket
		sockcsUnix->server = socket(AF_UNIX, SOCK_SEQPACKET, 0);
		if(sockcsUnix->server < 0){
			printf("\e[1;31;43mUnable to init UNIX socket err %d\e[m\n", errno);
			return sockcsUnix->server;
		}

		//Préparation de l'adresse côté serveur
		memset(&sockaddrUnix, 0, sizeof(sockaddrUnix));
		sockaddrUnix.sun_family = AF_UNIX;
		strncpy(sockaddrUnix.sun_path, "/tmp/hwsocket", sizeof(sockaddrUnix.sun_path)-1);

		//Lien entre la socket et l'adresse
		int binded = bind(sockcsUnix->server, (struct sockaddr*)&sockaddrUnix, sizeof(sockaddrUnix));
		if(binded<0){
			printf("\e[1;31;43mUnable to bind UNIX socket\e[m\n");
			return binded;
		}

		//On écoute la socket pour attendre une connexion
		int listened = listen(sockcsUnix->server, 1);
		if(listened<0){
			printf("\e[1;31;43mUnable to listen to UNIX socket\e[m\n");
			return listened;
		}

		printf("\e[32mListening to UNIX socket /tmp/hwsocket ...\e[m\n");
	}

	{
		//TCP/IP SOCKET
		sockcsTcp = malloc(sizeof(struct sockcs));
		sockcsTcp->type = AF_INET;

		//Init de la socket
		sockcsTcp->server = socket(AF_INET, SOCK_STREAM, 0);
		if(sockcsTcp->server < 0){
			printf("\e[1;31;43mUnable to init TCP socket: err %d\e[m\n", errno);
			return sockcsTcp->server;
		}

		//Préparation de l'adresse côté serveur
		sockaddrTcp.sin_addr.s_addr = htonl(INADDR_ANY);
		sockaddrTcp.sin_family = AF_INET;
		sockaddrTcp.sin_port = htons(1338);

		//Lien entre la socket et l'adresse
		int binded = bind(sockcsTcp->server, (struct sockaddr*)&sockaddrTcp, sizeof(sockaddrTcp));
		if(binded<0){
			printf("\e[1;31;43mUnable to bind TCP socket\e[m\n");
			return binded;
		}

		//On écoute la socket pour attendre une connexion
		int listened = listen(sockcsTcp->server, 1);
		if(listened<0){
			printf("\e[1;31;43mUnable to listen to TCP socket\e[m\n");
			return listened;
		}

		printf("\e[32mListening to TCP socket /tmp/hwsocket ...\e[m\n");
	}

	return 0;
}

void SocketSendEvent(struct Event ev){
	if(sockcsUnix->client>=0)
		send(sockcsUnix->client, &ev, sizeof(ev), MSG_DONTWAIT);
	if(sockcsTcp->client>=0)
		send(sockcsTcp->client, &ev, sizeof(ev), MSG_DONTWAIT);
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
		printf("\e[1;33mWARNING: GPS value out of bound (%f|%f)\e[m\n", latitude, longitude);
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
		printf("\e[1;33mWARNING: Roll value out of bound (%f)\e[m\n", angle);
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
		printf("\e[1;33mWARNING: WindDir value out of bound (%f)\e[m\n", angle);
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
		printf("\e[1;33mWARNING: Compass value out of bound (%f)\e[m\n", angle);
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
		printf("\e[1;33mWARNING: Battery value out of bound (%f)\e[m\n", voltage);
		return;
	}
	struct Event ev;
	ev.id = DEVICE_ID_BATTERY;
	ev.data[0] = (uint64_t)((voltage)*UINT64_MAX/10.0);
	ev.data[1] = 0;
	SocketSendEvent(ev);
}







