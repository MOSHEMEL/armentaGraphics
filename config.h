#pragma once

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"

#define DEBUG_FLAG 0
#define DEBUG_STANDALONE 0
#define VERSION "2.2"
#define PROMINI 0
#define LED_TOGGLE 0
#define ILI9341_bk1        0x0555
#define Warning_RED 0x7F2E5
#define BUFFPIXEL 20
#define MAX_COMMAND_LENGTH 50
#define BATTERY_TEXT_POS 174, 184
#define AM_TEXT_POS 245, 184

#if PROMINI
	#include "drawspi.h"
#else
	#include "drawfigures.h"
#endif