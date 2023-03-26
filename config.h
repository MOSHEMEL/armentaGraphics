#pragma once

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#define DEBUG_FLAG        0
#define DEBUG_STANDALONE  0
#define UNIT_TEST

#define VERSION				"3.91"
#define PROMINI				0
#define LED_TOGGLE			0
#define ILI9341_bk1			0x0555
#define Warning_RED			0x7F2E5
#define Warning_BLUE		0x010C
#define Warning_YELLOW		RGB888toRGB565("FFFF00")
#define BUFFPIXEL			20
#define MAX_COMMAND_LENGTH	128
#define BATTERY_TEXT_POS	174, 184
#define AM_TEXT_POS			245, 184
#define PASS_CRC			0
#define PASS_PRESSURE		0
#define SIMPLE_BAT			1
#define WATCHDOG_TIMER_EXPIRE 10000

#if PROMINI
	#include "drawspi.h"
#else
	#include "drawfigures.h"
#endif
#define GREEN_WARN_BATT     668
#define LANG_STR_NUM	23//17
#define LANG_STR_LEN	50//40
#define LANG_WIN_LEN	2
#define LANG_X			30
#define LANG_Y			30
#define LANG_W			180
#define LANG_H			90

typedef enum
{
	LANG_ENG,
	LANG_SPA,
	// LANG_GER,
	// LANG_FRA,
	LANG_MAX
} LANG_TypeDef;

char LANG_NAMES[LANG_MAX][LANG_STR_LEN] = {
	{"English"},
	{"Español"},
	// {"Deutsch"},
	// {"Français"}
};

char* LANG[LANG_STR_NUM];
char* LANG_WIN[LANG_WIN_LEN];
bool lang_set_done = false;

char LANG_ENG_ARR[LANG_STR_NUM][LANG_STR_LEN] =  {
	{ "Attention" },								  // 0
	{ "Notify Service" },							  // 2
	{ "Contact Service" },							  // 1
	{ "Error" },									  // 3
	{ "Turn Off and" },								  // 4
	{ "Reconnect APT" },							  // 5
	{ "Replace AM" },								  // 6
	{ "Maintenance Required" },						  // 7
	{ "AM Pulses" },								  // 8
	{ "Remaining" },								  // 9
	{ "Pay attention, You are left" },				  // 10
	{ "with less than 5000 pulses." },				  // 11
	{ "Please replace AM" },						  // 12
	//{ "AM Number %d" },							  // 
	//{ "Remaining %d" },							  // 
	{ "AM Number" },								  // 13
	{ "Remaining" },								  // 14
	{ "promptly" },									  // 15
	{ "Warranty expired" },			     			  // 16
	{ "Refurbish service required." },				  // 17
	{ "APT efficiency" },				    		  // 18
	{ "might be compromised." },					  // 19
	{ "Further use at your own risk." },			  // 20
  { "Please charge the battery." },//21
	{ "\0" }										  // 22
};
  
char LANG_SPA_ARR[LANG_STR_NUM][LANG_STR_LEN] =  {
	{ "Atención" },									 // 0
	{ "Notificar Servicio" },						 // 2
	{ "Contacto Servicio" },						 // 1
	{ "Error" },									 // 3
	{ "Apagar y" },									 // 4
	{ "Vuelva a conectar APT" },					 // 5
	{ "Reemplazar AM" },							 // 6
	{ "Mantenimiento requerido" },					 // 7
	{ "AM Pulsos" },								 // 8
	{ "Restante" },									 // 9
	{ "Presta atención, te quedas" },				 // 10
	{ "con menos de 5000 pulsos." },				 // 11
	{ "Por favor reemplace AM" },					 // 12
	//{ "AM número %d" },							 // 
	//{ "Restante %d" },							 // 
	{ "AM número" },								 // 13
	{ "Restante" },									 // 14
	{ "de inmediato" },								 // 15
	{ "Garantía expirada" },						 // 16
	{ "Se requiere servicio de" },	 // 17
	{ "reacondicione. Eficiencia del APT " },						 // 18
	{ "puede estar comprometida." },				 // 19
	{ "Uso adicional bajo su propio riesgo." },		 // 20
	{ "\0" }										 // 21
};

char LANG_GER_ARR[LANG_STR_NUM][LANG_STR_LEN] =  {
	{ "Achtung" },                                   // 0
	{ "Service verständigen" },						 // 2
	{ "Service kontaktieren" },						 // 1
	{ "Error" },									 // 3
	{ "Abschalten und" },							 // 4
	{ "APT neu verbinden" },						 // 5
	{ "AM austauschen" },							 // 6
	{ "Wartung erforderlich" },						 // 7
	{ "AM Pulse" },									 // 8
	{ "Verbleibend" },								 // 9
	{ "Vorsicht, es verbleiben" },					 // 10
	{ "weniger als 5000 Pulse." },					 // 11
	{ "Bitte AM bald" },							 // 12
	//{ "AM Nummer %d" },							 // 
	//{ "Verbleibend %d" },							 // 
	{ "AM Nummer" },								 // 13
	{ "Verbleibend" },								 // 14
	{ "ersetzen" },									 // 15
	{ "Garantie abgelaufen" },						 // 16
	{ "Überholungsservice erforderlich." },			 // 17
	{ "APT - Effizienz könnte" },                    // 18
	{ "beeinträchtigt sein. Weitere " },             // 19
	{ "Verwendung auf eigene Gefahr." },        	 // 20
	{ "\0" }										 // 21
};
char LANG_FRA_ARR[LANG_STR_NUM][LANG_STR_LEN] = {
	{ "Attention" },                                 // 0
	{ "Notifier le Service" },						 // 2
	{ "Contacter le Service" },						 // 1
	{ "Erreur" },									 // 3
	{ "Désactiver et" },							 // 4
	{ "Reconnecter APT" },							 // 5
	{ "Remplacer AM" },								 // 6
	{ "Maintenance requise" },						 // 7
	{ "Impulsions AM" },							 // 8
	{ "Restant" },									 // 9
	{ "Attention, il vous reste" },					 // 10
	{ "moins de 5000 impulsions." },				 // 11
	{ "Veuillez remplacer AM" },					 // 12
	//{ "Nombre AM  %d" },							 // 
	//{ "Restant %d" },								 // 
	{ "Nombre AM" },								 // 13
	{ "Restant" },									 // 14
	{ "rapidement" },								 // 15
	{ "Garantie expirée" },							 // 16
	{ "Service de remise à neuf requis." },				 // 17
	{ "Efficacité APT pourrait " },		 // 18
	{ "être compromis. Utilisation" },		 // 19
	{ "ultérieure à vos risques et périls." },// 20
	{ "\0" }										 // 21
};

//static const struct {
//	unsigned int 	 width;
//	unsigned int 	 height;
//	unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
//	unsigned char	 pixel_data[24 * 12 * 2 + 1];
//} lang_errow = {
//  24, 12, 2,
//  "\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\377\377\377\377\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250"
//  "\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\377\377\377\377\377\377\377\377\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\250\250\0\250\250\0\250\250"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\250\250\0\250"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\250\250\0\250"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\250\250\0\250\250\0\250\250"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
//  "\377\377\377\377\377\377\377\377\377\377\377\377\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\377\377\377\377\377\377\377\377\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250"
//  "\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\377\377\377\377\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250"
//  "\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0"
//  "\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0\250\250\0", };
