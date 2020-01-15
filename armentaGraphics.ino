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
	15.01.2020 Nick:
	1. Bugs identified by V&V:
		1.1 sometimes an additional letter is added to transmission of counter which makes a 3 digit num
		into 4 digit and then it is not cleared.
		1.2 if you send some big number lets say 5 digits(12345) and then a smaller lets say 3 digit
		there is no clearing of screen
		1.3 an error screen (AM remaining 1000 and such)
		is sent about 10 / 20 times, since update function is run faster than pulse is updated
	2. In order to process less data, maybe implement checksum?!
	3. Maybe save state to know what is displayed or supposed.
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
char key;
int counter = -1;
int percentBattery = 0;
int FontSizeArmenta = 2;
bool start_command = false;

void PrintOnLcd(char* buf)
{
	// If the promini is the board of choise. There is no Serial1
	//Serial.println(buf);
	if ((*buf == 'c') || (*buf == 'C')) // counter [number]
	{
		parse_pulse_counter(buf);
	} // enf if
	else if ((*buf == 'a') || (*buf == 'A')) // pulse present
	{
		parse_pulse(buf);
	} // end if
	else if ((*buf == 'p') || (*buf == 'P')) // pressure [number]
	{
		parse_pressure(buf);
	} // end if
	else if ((*buf == 't') || (*buf == 'T')) // parse applicator in size
	{
		parse_aplicator(buf);
	} // end if
	else if ((*buf == 'b') || (*buf == 'B')) // battery [number]
	{
		parse_battery_percent(buf);
	} // end if
	else if ((*buf == 'e') || (*buf == 'E')) // error with number
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
	else if ((*buf == 'm') || (*buf == 'M'))
	{
		parse_test_battery();
	}
	else if ((*buf == 'v') || (*buf == 'V'))
	{
		parse_version(buf);
	}
	else if ((*buf == 'g') || (*buf == 'G'))
	{
		parse_cs(buf);
	}
	tft.setCursor(0, 0);
}

void setup(void) {
#if PROMINI
	Serial.begin(115200);
#else
	Serial.begin(115200);
	Serial1.begin(115200);
#endif
	SPI.begin();
	tft.begin();
	tft.setRotation(3);
	pinMode(LED_BUILTIN, OUTPUT);

#if PROMINI
	Serial.print("Initializing SD card...");

#endif
#if PROMINI
	if (!SD.begin(4)) 
	{ // SD_CS define is acting up
		Serial.println("failed!");
		Serial.println("Pro mini setting setup: if not apprropriate board, please switch");
	}
	else {
		Serial.println("OK!");
	}
#endif
	
	tft.fillScreen(ILI9341_WHITE);
	
	DRAW_WELCOME

	delay(200);
#if DEBUG_STANDALONE
	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.println("DEBUG VERSION");
	tft.println("DEBUG VERSION");
	tft.println("DEBUG VERSION");
	delay(1000);
#endif
	paint_half_half();

	delay(20);
	DRAW_PULSE_Y
	DRAW_BATTERY
	DRAW_PRESSURE_HIGH
	DRAW_AP_ERROR

	delay(20);
	DRAW_PULSE_N
	DRAW_BATTERY
	DRAW_PRESSURE_LOW
	DRAW_AP_OK




	// am percentage
	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.setCursor(AM_TEXT_POS);
	tft.println("...");

	// battery percentage
	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.setCursor(BATTERY_TEXT_POS);
	tft.println("...");

	blank_upper_side();
}


void loop(void) {
	static int i;

	led_toggle = !led_toggle;
	digitalWrite(LED_BUILTIN, led_toggle);
	if (
#if PROMINI || DEBUG_STANDALONE
		Serial.available() > 0
#else
		Serial1.available() > 0
#endif

		) {
#if PROMINI || DEBUG_STANDALONE
		key = Serial.read();
#else
		key = Serial1.read();
#endif
		if (key == '$') //head
		{
			i = 0;
			BufferString[0] = '$';
		}
		else if (key == '#') // ending tail
		{
			BufferString[i] = 0;
			if (BufferString[0] == '$')
			{
				PrintOnLcd(&BufferString[1]);
			}
			i = MAX_COMMAND_LENGTH+1;
		}
		else if (key > -1)
		{
			BufferString[i] = key;
			BufferString[i + 1] = 0;
		}
		i++;
		key = 0;
		if (i >= MAX_COMMAND_LENGTH) {
			i = 0;
		}
	}
}

#define SMALL_INTERVAL_TIME 100
#define THRESHOLD 20
void update(void)
{
	static long time_to_update = 0;
	static long update_dynamic_window = 10000; // 10seconds
	static int update_packages = 0;
	
	
	if (millis() - time_to_update > update_dynamic_window)
	{
		// UPDATE
		if (update_packages / update_dynamic_window > THRESHOLD)
			update_dynamic_window + 1 * SMALL_INTERVAL_TIME;
		else
		{
			update_dynamic_window = update_dynamic_window / 2;
			if (update_dynamic_window < SMALL_INTERVAL_TIME)
			{
				update_dynamic_window = SMALL_INTERVAL_TIME;
			}
		}
		update_packages = 0;
		// Get last time
		time_to_update = millis();
	}
}
