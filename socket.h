#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <stdint.h>

#define DEVICE_ID_INVALID 0
#define DEVICE_ID_SAIL 1
#define DEVICE_ID_HELM 2
#define DEVICE_ID_GPS 3
#define DEVICE_ID_ROLL 4
#define DEVICE_ID_WINDDIR 5
#define DEVICE_ID_COMPASS 6
#define DEVICE_ID_BATTERY 7

struct __attribute__((packed)) Event{
	uint8_t id;
	uint64_t data[2];
};

/**
	@brief Initialisation de la socket
	@return 0 si tout c'est bien passé, code d'erreur négatif sinon
**/
int SocketInit();

/**
	@brief Ferme proprement la socket
*/
void SocketClose();

/**
	@brief Demarre la gestion parallèle des clients
*/
void SocketHandleClients();

/**
	@brief Envoi un évènement à l'intelligence
*/
void SocketSendEvent(struct Event ev);

/**
	@brief Cette fonction est appelée quand un event est reçu
*/
void SocketHandleReceivedEvent(struct Event ev);


/**
	@brief Convertit des données brutes en donnée tension voile (de 0 à 255)
*/
unsigned short ConvertToSailValue(uint64_t data[2]);

/**
	@brief Convertit des données brutes en donnée orientation barre (de -45° à 45°)
*/
float ConvertToHelmValue(uint64_t data[2]);

/**
	@brief Envoi de la position GPS (2x IEEE-754 64 bit floating point)
	@arg latitude en degrés décimaux, entre -90° et 90°
	@arg longitude en degrés décimaux, entre -180° et 180°
*/
void SocketSendGps(double latitude, double longitude);

/**
	@brief Envoi du roulis (IEEE-754 64 bit floating point)
	@arg angle Entre -180° et 180°. 0=bateau droit. 180° étant un cas très fâcheux...
*/
void SocketSendRoll(double angle);

/**
	@brief Envoi de la direction du vent (IEEE-754 64 bit floating point)
	@arg angle Entre -180° et 180°. 0=vent de face
*/
void SocketSendWindDir(double angle);

/**
	@brief Envoi de la direction du compas (IEEE-754 64 bit floating point)
	@arg angle Entre 0° et 360°. 0=Nord
*/
void SocketSendCompass(double angle);

/**
	@arg voltage Entre 0 et 18 volts
**/
void SocketSendBattery(float voltage);


#endif
