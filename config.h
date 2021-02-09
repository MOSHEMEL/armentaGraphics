#pragma once

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#define DEBUG_FLAG        0
#define DEBUG_STANDALONE 0

#define VERSION "3.8"
#define PROMINI 0
#define LED_TOGGLE 0
#define ILI9341_bk1        0x0555
#define Warning_RED 0x7F2E5
#define Warning_BLUE 0x010C
#define BUFFPIXEL 20
#define MAX_COMMAND_LENGTH 128
#define BATTERY_TEXT_POS 174, 184
#define AM_TEXT_POS 245, 184
#define PASS_CRC 0
#define PASS_PRESSURE 0
#define SIMPLE_BAT 1
#define WATCHDOG_TIMER_EXPIRE 10000

//#define ENGLISH					0
//#define SPANISH					1
//#define HEBREW					0
//#define GERMAN          0
#if PROMINI
	#include "drawspi.h"
#else
	#include "drawfigures.h"
#endif

//#if ENGLISH
//char LANG[16][40] = {

#define LANG_STR_NUM	17
#define LANG_STR_LEN	40
#define LANG_X			120
#define LANG_Y			30
#define LANG_W			180
#define LANG_H			90

typedef enum
{
	LANG_ENG,
	LANG_SPA,
	LANG_GER,
	LANG_MAX
} LANG_TypeDef;

char* LANG[LANG_STR_NUM];
bool lang_done = false;

char LANG_ENG_ARR[LANG_STR_NUM][LANG_STR_LEN] =  {
	{ "Attention" },
	{ "Notify APT Service" },
	{ "Contact APT Service" },
	{ "Error" },
	{ "Turn Off And" },
	{ "Reconnect APT" },
	{ "Replace AM" },
	{ "APT Maintenance Required" },
	{ "AM Pulses" },
	{ "Remaining" },
	{ "Pay attention, You are left with less" },
	{ "with less than 5000 pulses." },
	{ "Please replace AM Promptly" },
	{ "AM Number %d" },
	{ "Remaining %d" },
	{ "\0" }
};
  
//#define ATTENTION		"Attention"
//#define NOTIFY			"Notify APT Service"
//#define CONTACT			"Contact APT Service"
//#define ERROR			"Error"
//#define ERROR4001_P1	"Turn Off And"
//#define ERROR4001_P2	"Reconnect APT"
//#define REPLACE_AM		"Replace AM"
//#define MAINTENANCE		"APT Maintenance Required"
//#define AM_PULSES		"AM Pulses"
//#define REMAINING		"Remainning"
//#define SPLASH_P1		"Pay attention, You are left with less"
//#define SPLASH_P2		"than 5000 pulses."
//#define SPLASH_P3		"Please replace AM shortly"
//#define SN_P1			"AM Number %d"
//#define SN_P2			"Remainning %d"

//#elif SPANISH
//char LANG[16][40] = {
char LANG_SPA_ARR[LANG_STR_NUM][LANG_STR_LEN] =  {
	{ "Atención" },
	{ "Notificar APT Servicio" },
	{ "Contacto APT Servicio" },
	{ "Error" },
	{ "Apagar y" },
	{ "Vuelva a conectar APT" },
	{ "Reemplazar AM" },
	{ "APT mantenimiento requerido" },
	{ "AM Pulsos" },
	{ "Restante" },
	{ "Presta atención, te quedas" },
	{ "con menos de 5000 pulsos." },
	{ "Por favor reemplace AM." },
	{ "AM número %d" },
	{ "Restante %d" },
	{ "de inmediato" },
	{ "\0" }
};
//#define ATTENTION		"Atencion"
//#define NOTIFY			"Notificar a Servicio"
//#define CONTACT			"Contactar Servicio"
//#define ERROR			"Error"
//#define ERROR4001_P1	"Apagar y"
//#define ERROR4001_P2	"Reconnectar el APT"
//#define REPLACE_AM		"Reemplazar AM"
//#define MAINTENANCE		"Maintenimiento requerido del APT"
//#define AM_PULSES		"Pulses"
//#define REMAINING		"Restantes del AM"
//#define SPLASH_P1		"Presta atencion, te quedan menos"
//#define SPLASH_P2		"de 5000 pulsos."
//#define SPLASH_P3		"Por favor reemplace AM en breve."
//#define SN_P1			"AM Numero %d"
//#define SN_P2			"Restantes %d"
//#elif GERMAN
//char LANG[16][40] = {
char LANG_GER_ARR[LANG_STR_NUM][LANG_STR_LEN] =  {
	{ "Achtung" },
	{ "APT-Service verständigen" },
	{ "APT-Service kontaktieren" },
	{ "Error" },
	{ "Abschalten und" },
	{ "APT neu verbinden" },
	{ "AM austauschen" },
	{ "APT Wartung erforderlich" },
	{ "AM Pulse" },
	{ "Verbleibend" },
	{ "Vorsicht, es verbleiben" },
	{ "weniger als 5000 Pulse." },
	{ "Bitte AM bald ersetzen." },
	{ "AM Nummer %d" },
	{ "Verbleibend %d" },
	{ "\0" }
};
//#endif

