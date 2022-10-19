#pragma once

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#define DEBUG_FLAG        0
#define DEBUG_STANDALONE  0

#define VERSION				"3.9"
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

#define LANG_STR_NUM	22//17
#define LANG_STR_LEN	50//40
#define LANG_WIN_LEN	3
#define LANG_X			30
#define LANG_Y			30
#define LANG_W			180
#define LANG_H			90

typedef enum
{
	LANG_ENG,
	LANG_SPA,
	LANG_GER,
	LANG_FRA,
	LANG_MAX
} LANG_TypeDef;

char LANG_NAMES[LANG_MAX][LANG_STR_LEN] = {
	{"English"},
	{"Español"},
	{"Deutsch"},
	{"Français"}
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
	{ "\0" }										  // 21
};
  
char LANG_SPA_ARR[LANG_STR_NUM][LANG_STR_LEN] =  {
	{ "Atención" },
	{ "Notificar Servicio" },
	{ "Contacto Servicio" },
	{ "Error" },
	{ "Apagar y" },
	{ "Vuelva a conectar APT" },
	{ "Reemplazar AM" },
	{ "Mantenimiento requerido" },
	{ "AM Pulsos" },
	{ "Restante" },
	{ "Presta atención, te quedas" },
	{ "con menos de 5000 pulsos." },
	{ "Por favor reemplace AM" },
	//{ "AM número %d" },
	//{ "Restante %d" },
	{ "AM número" },
	{ "Restante" },
	{ "de inmediato" },
	{ "\0" }
};

char LANG_GER_ARR[LANG_STR_NUM][LANG_STR_LEN] =  {
	{ "Achtung" },
	{ "Service verständigen" },
	{ "Service kontaktieren" },
	{ "Error" },
	{ "Abschalten und" },
	{ "APT neu verbinden" },
	{ "AM austauschen" },
	{ "Wartung erforderlich" },
	{ "AM Pulse" },
	{ "Verbleibend" },
	{ "Vorsicht, es verbleiben" },
	{ "weniger als 5000 Pulse." },
	{ "Bitte AM bald" },
	//{ "AM Nummer %d" },
	//{ "Verbleibend %d" },
	{ "AM Nummer" },
	{ "Verbleibend" },
	{ "ersetzen" },
	{ "\0" }
};

char LANG_FRA_ARR[LANG_STR_NUM][LANG_STR_LEN] = {
	{ "Attention" },
	{ "Notifier le Service" },
	{ "Contacter le Service" },
	{ "Erreur" },
	{ "Désactiver et" },
	{ "Reconnecter APT" },
	{ "Remplacer AM" },
	{ "Maintenance requise" },
	{ "Impulsions AM" },
	{ "Restant" },
	{ "Attention, il vous reste" },
	{ "moins de 5000 impulsions." },
	{ "Veuillez remplacer AM" },
	//{ "Nombre AM  %d" },
	//{ "Restant %d" },
	{ "Nombre AM" },
	{ "Restant" },
	{ "rapidement" },
	{ "\0" }
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
