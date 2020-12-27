#pragma once

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"

#define DEBUG_FLAG 0
#define DEBUG_STANDALONE 0
#define VERSION "1.0 TEST"
#define PROMINI 0
#define LED_TOGGLE 0
#define ILI9341_bk1        0x0555
#define Warning_RED 0x7F2E5
#define Warning_BLUE 0x010C
#define PASS_GREEN      0x07E0         
//0X8C01
#define BUFFPIXEL 20
#define MAX_COMMAND_LENGTH 128
#define BATTERY_TEXT_POS 174, 184
#define AM_TEXT_POS 245, 184
#define PASS_CRC 0
#define PASS_PRESSURE 1
#define SIMPLE_BAT 1
#define WATCHDOG_TIMER_EXPIRE 10000

#define ENGLISH					1
#define SPANISH					0
#define HEBREW					0
#if PROMINI
	#include "drawspi.h"
#else
	#include "drawfigures.h"
#endif

#if ENGLISH
#define ATTENTION		"Attention"
#define NOTIFY			"Notify APT Service"
#define CONTACT			"Contact APT Service"
#define ERROR			"Error"
#define ERROR4001_P1	"Turn Off And"
#define ERROR4001_P2	"Reconnect APT"
#define REPLACE_AM		"Replace AM"
#define MAINTENANCE		"APT Maintenance Required"
#define AM_PULSES		"AM Pulses"
#define REMAINING		"Remaining"
#define SPLASH_P1		"Pay attention, You are left with less"
#define SPLASH_P2		"than 5000 pulses."
#define SPLASH_P3		"Please replace AM shortly"
#define SN_P1			"AM Number %d"
#define SN_P2			"Remainig %d"
#elif SPANISH
#define ATTENTION		"Atencion"
#define NOTIFY			"Notificar a Servicio"
#define CONTACT			"Contactar Servicio"
#define ERROR			"Error"
#define ERROR4001_P1	"Apagar y"
#define ERROR4001_P2	"Reconnectar el APT"
#define REPLACE_AM		"Reemplazar AM"
#define MAINTENANCE		"Maintenimiento requerido del APT"
#define AM_PULSES		"Pulses"
#define REMAINING		"Restantes del AM"
#define SPLASH_P1		"Presta atencion, te quedan menos"
#define SPLASH_P2		"de 5000 pulsos."
#define SPLASH_P3		"Por favor reemplace AM en breve."
#define SN_P1			"AM Numero %d"
#define SN_P2			"Restantes %d"
#elif HEBREW
#define ATTENTION		"Attention"
#define NOTIFY			"Notify APT Service"
#define CONTACT			"Contact APT Service"
#define ERROR			"Error"
#define ERROR4001_P1	"Turn Off And"
#define ERROR4001_P2	"Reconnect APT"
#define REPLACE_AM		"Replace AM"
#define MAINTENANCE		"APT Maintenance Required"
#define AM_PULSES		"AM Pulses"
#define REMAINING		"Remaining"
#define SPLASH_P1		"Pay attention, You are left with less"
#define SPLASH_P2		"than 5000 pulses."
#define SPLASH_P3		"Please replace AM shortly"
#endif
