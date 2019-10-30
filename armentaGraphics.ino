
#include "armenta.h"
#include "arduino_aux.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "aplicator_error.c"
#include "aplicator_ok.c"
#include "battery.c"
#include "pressure_high.c"
#include "pressure_low.c"
#include "pulseN.c"
#include "pulseY.c"
#include "w_cropped.c"

void parse_E(char* buf);
void parse_battery_percent(char* buf);
void parse_clean_screen(char* buf);
void parse_pulse(char* buf);
void parse_pressure(char* buf);
void parse_aplicator(char* buf);

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
#define PROMINI 0
#define DEBUG_FLAG 1
#define VERSION "1.3 OCT19"


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
#define LED_TOGGLE 0



int y = 0;
char CounterString[100];
char  BufferString[100];
unsigned char i;
char key;
uint8_t* p[10];  // pinter to array
int counter = -1;
int percentBattery = 0;
int FontSizeArmenta = 2;

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define ILI9341_bk1        0x0555
# define Warning_RED 0x7F2E5
#define BUFFPIXEL 20
// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

// *****************************************************************************************
// the lcd working in RB565 16 BIT ,while the graphics in bmp is RGB88 24 BIT
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
	tft.drawRGBBitmap(75, 50, (uint16_t*)(w_exported.pixel_data), w_exported.width, w_exported.height);
	tft.setTextColor(ILI9341_BLACK);
	tft.setFont();
	tft.setTextSize(1);
	tft.setCursor(10, 10);
	tft.print("Current Version is ");
	tft.println(VERSION);
	delay(2000);
	paint_half_half();


	delay(2000);
	tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_low.pixel_data), pressure_low.width, pressure_low.height);
	tft.drawRGBBitmap(90, 168, (uint16_t*)(pulseN.pixel_data), pulseN.width, pulseN.height);
	tft.drawRGBBitmap(160, 168, (uint16_t*)(battery.pixel_data), battery.width, battery.height);
	tft.drawRGBBitmap(230, 168, (uint16_t*)(aplicator_error.pixel_data), aplicator_error.width, aplicator_error.height);


	delay(2000);
	tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_high.pixel_data), pressure_high.width, pressure_high.height);
	tft.drawRGBBitmap(90, 168, (uint16_t*)(pulseY.pixel_data), pulseY.width, pulseY.height);
	tft.drawRGBBitmap(160, 168, (uint16_t*)(battery.pixel_data), battery.width, battery.height);
	tft.drawRGBBitmap(230, 168, (uint16_t*)(aplicator_ok.pixel_data), aplicator_ok.width, aplicator_ok.height);

	// battery
	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.setCursor(174, 184); //17,184
	tft.println("25%");


	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.setCursor(245, 180);
	tft.println("25%");

	blank_upper_side();
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
		}
		CounterString[i] = key;
		BufferString[i] = key;
		CounterString[i + 1] = 0;
		BufferString[i + 1] = 0;
		if (key == '$')
		{
			i = 0;
			BufferString[0] == '$';
		}
		else if (key == '#') // ending tail
		{
			CounterString[i] = 0;
			if (BufferString[0] == '$')
			{
				CounterString[10] = 0;
				PrintOnLcd(&CounterString[1]);
			}
		}
		i++;
		key = 0;
		if (i > 9) {
			i = 0;
		}
	}
}


void parse_E(char* buf) {
	int Base_y = 140;
	buf++;
	tft.setRotation(3);
	tft.setCursor(8, Base_y + 20);
	for (int y = 18; y < 53; y++) {
		//   delay(100);
		tft.drawLine(3, Base_y + y, 133, Base_y + y, ILI9341_BLACK);
	}
	tft.setTextColor(ILI9341_WHITE);
	tft.setFont();
	tft.setTextSize(4);
	tft.setCursor(8, Base_y + 20);
	tft.println(buf);
}

void parse_battery_percent(char* buf) {
	int Base_y = 0;
	buf++;
	int newpercentBattery = atoi(buf);
	if (percentBattery != newpercentBattery) {
		percentBattery = clip_percent(newpercentBattery);
		tft.setRotation(3);
		tft.setCursor(8, Base_y + 20);
		// battery
		Base_y = 180;
		for (int y = 0; y < 23; y++) {
			//   delay(100);
			tft.drawLine(174, Base_y + y, 224, Base_y + y, ILI9341_WHITE);
		}
		tft.setTextColor(ILI9341_BLACK);
		tft.setTextSize(2);
		tft.setCursor(174, 184); //17,184
		tft.setFont();
		tft.print(percentBattery);
		tft.print('%');
		tft.setCursor(0, 0); //17,184
		// base bars battery percent

#if PROMINI
		Serial.print("Percent Battery ");
		Serial.println(percentBattery);
#else
		Serial.print("Percent Battery ");
		Serial.println(percentBattery);
#endif


		Base_y = 208;
		if (percentBattery <= 10) {
			for (int y = 0; y <= percentBattery * 42 / 100; y++) {
				//   delay(100);
				tft.drawLine(168 + y, Base_y + 0, 168 + y, Base_y + 18,
					ILI9341_RED);
			}
		}
		else {
			for (int y = 0; y <= percentBattery * 42 / 100; y++) {
				//   delay(100);
				tft.drawLine(168 + y, Base_y + 0, 168 + y, Base_y + 18,
					ILI9341_GREEN);
			}
		}
		for (int y = percentBattery * 42 / 100; y <= 42; y++) {
			//   delay(100);
			tft.drawLine(168 + y, Base_y + 0, 168 + y, Base_y + 18, ILI9341_WHITE);
		}

	}
}



void parse_pulse_counter(char* buf) {
	// This function is sent always even if no number needs to be printed
	uint32_t currentmilis = millis();
	buf++;
	int new_counter = atoi(buf);
	if (counter != new_counter)
	{
		check_digits_changed_and_blank(new_counter);
		bool printed = graphics_to_Screen(new_counter);
		if (printed)
		{
#if DEBUG_FLAG
			Serial.print(F("Printing char is "));
			Serial.print(counter);
			Serial.print(F(" and takes "));
			Serial.print(millis() - currentmilis);
			Serial.println(F(" milisecond long"));
#endif
		}
	}
}


void parse_pulse(char* buf) {
	int Base_y = 60;
	buf++;
	tft.setRotation(3);
	tft.setCursor(8, Base_y + 20);
	buf[1] = 0;
	if ((*buf == 'Y') || (*buf == 'y')) {
		tft.drawRGBBitmap(90, 168, (uint16_t*)(pulseY.pixel_data), pulseY.width, pulseY.height);

	}
	else {
		tft.drawRGBBitmap(90, 168, (uint16_t*)(pulseN.pixel_data), pulseN.width, pulseN.height);
	}
}

void parse_pressure(char* buf) {
	// TODO: change to parse pLOW or pO.K
	int Base_y = 60;
	buf++;
	tft.setRotation(3);
	tft.setCursor(8, Base_y + 20);
	if(strcmp(buf,"LOW") == 0)
	{
		tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_low.pixel_data), pressure_low.width, pressure_low.height);
	}
	else if(strcmp(buf,"O.K") == 0)
	{
		tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_high.pixel_data), pressure_high.width, pressure_high.height);
	}
}



void parse_aplicator(char* buf) {
	int Base_y = 60;
	buf++;
	tft.setRotation(3);
	tft.setCursor(8, Base_y + 20);
	buf[2] = 0;
	int percentP = atoi(buf);
#if PROMINI
	Serial.print("Percent Applicaor Counter");
	Serial.println(percentP);
#else
	Serial.print("Percent Applicaor Counter");
	Serial.println(percentP);
#endif
	if ((percentP >= 0) && (percentP <= 100)) {
		if (percentP <= 10) // l0 percent
		{
			tft.drawRGBBitmap(230, 168, (uint16_t*)(aplicator_error.pixel_data), aplicator_error.width, aplicator_error.height);
		}
		else {
			tft.drawRGBBitmap(230, 168, (uint16_t*)(aplicator_ok.pixel_data), aplicator_ok.width, aplicator_ok.height);
		}
		tft.setTextColor(ILI9341_BLACK);
		tft.setFont();
		tft.setTextSize(2);
		tft.setCursor(245, 180);
		tft.print(percentP);
		tft.print('%');
	}
}
void reset_screen()
{
	paint_half_half();
	tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_high.pixel_data), pressure_high.width, pressure_high.height);
	tft.drawRGBBitmap(90, 168, (uint16_t*)(pulseY.pixel_data), pulseY.width, pulseY.height);
	tft.drawRGBBitmap(160, 168, (uint16_t*)(battery.pixel_data), battery.width, battery.height);
	tft.drawRGBBitmap(230, 168, (uint16_t*)(aplicator_ok.pixel_data), aplicator_ok.width, aplicator_ok.height);
}

void parse_fail(char* buf)
{
	buf++;
	tft.setRotation(3);
	int ammount_left = atoi(buf);
	if ((ammount_left >= 0) && (ammount_left % 200 == 0)) {
		// TODO: add functionality depending on Selas' input in regards to 27 psi pressure.
		tft.fillScreen(Warning_RED);
		tft.setFont();
		tft.setRotation(3);
		tft.setTextColor(ILI9341_WHITE);
		tft.setCursor(130, 30);
		tft.println("AM");
		tft.setCursor(50, 60);
		tft.println("Remaining Pulses");
#if PROMINI
		tft.setFont(&ArmentaFont32pt7b);
		tft.setTextSize(FontSizeArmenta);
#else
		tft.setFont(&ArmentaFont64pt7b);
		tft.setTextSize(FontSizeArmenta / 2);
#endif
		if (ammount_left == 1000)
		{
			tft.setCursor(30, 200);
			tft.println(ammount_left);
		}
		else if(ammount_left == 0)
		{
			tft.setCursor(130, 200);
			tft.println(ammount_left);
		}
		else
		{
			tft.setCursor(80, 200);
			tft.println(ammount_left);
		}
	}
	// After we draw the screen - we then show the 
	delay(2000);
	reset_screen();
}



void parse_reset_screen(char* buf)
{
	reset_screen();
}

void blank_on_reset(char* buf)
{
	buf++;
	int new_counter = atoi(buf);
	tft.setRotation(3);
	tft.fillRect(0, 0, 320, 120, ILI9341_bk1);
	graphics_to_Screen(new_counter);
}

void print_error(char* buf)
{
	buf++;
	tft.fillScreen(Warning_RED);
	tft.setFont();
	tft.setRotation(3);
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(6);
	tft.setCursor(50, 60);
	tft.println("Error");
	tft.println(buf);
	tft.setTextSize(2);
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
	else if ((*buf == 'd') || (*buf == 'D')) // test rectangles
	{
		parse_draw_rectangles(buf);
	}
	else if ((*buf == 'f') || (*buf == 'F')) // last few pulses left
	{
		parse_fail(buf);
	}
	else if ((*buf == 'g') || (*buf == 'G')) // test the screen run
	{
		parse_pulse_counter_test(buf);
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

