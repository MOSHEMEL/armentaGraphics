#pragma once

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"

#define DEBUG_FLAG 0
#define DEBUG_STANDALONE 0
#define VERSION "3.4"
#define PROMINI 0
#define LED_TOGGLE 0
#define ILI9341_bk1        0x0555
#define Warning_RED 0x7F2E5
#define Warning_BLUE 0x010C
#define BUFFPIXEL 20
#define MAX_COMMAND_LENGTH 128
#define BATTERY_TEXT_POS 174, 184
#define AM_TEXT_POS 245, 184
#define PASS_CRC 1
#define SIMPLE_BAT 1
#define WATCHDOG_TIMER_EXPIRE 10000

#if PROMINI
	#include "drawspi.h"
#else
	#include "drawfigures.h"
#endif
