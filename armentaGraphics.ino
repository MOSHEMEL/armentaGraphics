#include <Adafruit_GFX.h>
#include "config.h"
#include "armenta.h"
#include "arduino_aux.h"
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include "parsers.h"


/*
   16.09.19 Armenta Copyright
   Nick comments:
   1. change pinout on ili9341 according to working example used in previous code
   2. add support pin to adress cs on sd_card (currently pin4) - wire is conected to CCS pinout on example
   3. Give better names
   4. tft.drawRGBBitmap function currently adresses by row, col which are iterators and not positional x,y
   5. If it works. Add to the codebase on github and seal up.
   20.09.19 Armenta
   Nick comments:
   1. prev list is done
   2. Working on size reduction, currently program is 27.7 KB which is 2KB less than max space
   3. Working program is v107 based which is outdated and needs to be updated to v109 or later
   24.09.19 Nick:
   1. MKRZERO and PROMINI working with adafruit gfx fonts
   2. Size is not an issue currently for promini
   3. Font was selected randomly
   4. Check if font can be changed to Font by graphic designer, ran throught process to create a font
   5. Collect additional requirements from Edi and Sela and add Those
	15.10.19 Nick:
	1. created a special branch for mkrzero that doesn't use sdcard
	2. fonts are from graphical designer
	3. size on mkrzero was rescaled to half , cause otherwise the file is too big
	4. so far testing seems o.k and not much stuff is broken
	5. TODO: add other screens like errors and etc
	6. TODO: let guys in field work with new version and write their grievances
	7. TODO: add flashing from stm32, OTA update of sw.
 */


 // TFT display and SD card will share the hardware SPI interface.
 // Hardware SPI pins are specific to the Arduino board type and
 // cannot be remapped to alternate pins.  For Arduino Uno,
 // Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
#if PROMINI
Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9);
#else
Adafruit_ILI9341 tft = Adafruit_ILI9341(7, 6);
#endif

#define TFT_RST 5
// Use hardware SPI (on MKRZERO, #8, #9, #10) and the above for CS/DC
// If using the breakout, change pins as desired
bool led_toggle = 0;



int y = 0;
char  BufferString[MAX_COMMAND_LENGTH];
unsigned char i;
char key;
int counter = -1;
int percentBattery = 0;
int FontSizeArmenta = 2;
bool start_command = false;



void setup(void) {
#if PROMINI
	Serial.begin(115200);
#else
	Serial.begin(115200);
	Serial1.begin(115200);
#endif
	SPI.beginTransaction(SPISettings(96000000, MSBFIRST, SPI_MODE0));
	tft.begin();
	pinMode(LED_TOGGLE, OUTPUT);

#if PROMINI
	Serial.print("Initializing SD card...");
#else
	Serial.print("Initializing SD card...");
#endif
#if PROMINI
	if (!SD.begin(4)) { // SD_CS define is acting up
		Serial.println("failed!");
		Serial.println("Pro mini setting setup: if not apprropriate board, please switch");
#else
	if (!SD.begin(28)) { // SD_CS define is acting up
		Serial.println("MKRZERO setting setup: if not apprropriate board, please switch");
		Serial.println("failed!");
#endif
	}
	else {
#if PROMINI
		Serial.println("OK!");
#else
		Serial.println("OK!");
#endif
	}
	tft.fillScreen(ILI9341_WHITE);
	tft.setRotation(3);
	DRAW_WELCOME
	tft.setTextColor(ILI9341_BLACK);
	tft.setFont();
	tft.setTextSize(1);
	tft.setCursor(10, 10);
	tft.print("Current Version is ");
	tft.println(VERSION);
	delay(2000);
	paint_half_half();


	delay(2000);
	DRAW_PULSE_N
	DRAW_BATTERY
	DRAW_PRESSURE_LOW
	DRAW_AP_ERROR


	delay(2000);
	DRAW_PULSE_Y
	DRAW_BATTERY
	DRAW_PRESSURE_HIGH
	DRAW_AP_OK

	// battery
	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.setCursor(174, 184); //17,184
	tft.println("25%");


	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.setCursor(245, 180);
	tft.println("25%");

	blank_upper_side();
	graphics_to_Screen(0);
	delay(100);
}

void loop(void) {
	tft.setRotation(3);
	led_toggle = !led_toggle;
	digitalWrite(LED_TOGGLE, led_toggle);
	if (
#if PROMINI
		Serial.available() > 0
#else
		Serial1.available() > 0
#endif

		) {
#if PROMINI
		key = Serial.read();
#else
		key = Serial1.read();
		Serial.write(key);
#endif
		if (key == '$') //head
		{
			i = 0;
			start_command = true;
		}
		else if(key == '#')
		{
			if(start_command && (BufferString[0] == '$'))
			{
				PrintOnLcd(&BufferString[1]);
				// Restart sequence reading
				i = 0;
				start_command = false;
			}
		}
		else if(start_command)
		{
			BufferString[i] = key;
		}
		i++;
		key = 0;
		if (i > MAX_COMMAND_LENGTH) {
			// Restart sequence reading
			i = 0;
			start_command = false;
		}
	}
}

void PrintOnLcd(char* buf)
{
	// If the promini is the board of choise. There is no Serial1
#if PROMINI
	Serial.println(buf);
#else
	Serial.println(buf);
#endif
	if ((*buf == 'C') || (*buf == 'c')) // counter [number]
	{
		parse_pulse_counter(buf);
	} // enf if
	else if ((*buf == 'A') || (*buf == 'a')) // pulse present
	{
		parse_pulse(buf);
	} // end if
	else if ((*buf == 'P') || (*buf == 'p')) // pressure [number]
	{
		parse_pressure(buf);
	} // end if
	else if ((*buf == 't') || (*buf == 'T')) // parse applicator in size
	{
		parse_aplicator(buf);
	} // end if
	else if ((*buf == 'B') || (*buf == 'b')) // battery [number]
	{
		parse_battery_percent(buf);
	} // end if
	else if ((*buf == 'E') || (*buf == 'e')) // error with number
	{
		parse_E(buf);
	}
	else if ((*buf == 'r') || (*buf == 'R')) // reset screen and wipe
	{
		parse_reset_screen(buf);
	}
	else if ((*buf == 'f') || (*buf == 'F')) // last few pulses left
	{
		parse_fail(buf);
	}
	else if ((*buf == 'q') || (*buf == 'Q'))
	{
		blank_on_reset(buf);
	}
	else if ((*buf == 'z') || (*buf == 'Z'))
	{
		print_error(buf);
	}
}

