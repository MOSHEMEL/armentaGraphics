
#include "armenta.h"
#include "charactersArmenta.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include "ArmentaFont.h"
#include "ArmentaFontHuge.h"
#include "welcome.c"
#include "aplicator_error.c"
#include "aplicator_ok.c"
#include "battery.c"
#include "pressure_high.c"
#include "pressure_low.c"
#include "pulseN.c"
#include "pulseY.c"

void check_digits_changed_and_blank(int curr_number);
void blank_upper_side();
void paint_half_half();
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
 */
#define PROMINI 0
#define DEBUG_FLAG 1
#define VERSION "1.1 OCT19"


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
	tft.fillScreen(ILI9341_BLACK);

	tft.drawRGBBitmap(0, 0, (uint16_t*)(WELCOME_pixel_data), WELCOME_WIDTH, WELCOME_HEIGHT);
	tft.setRotation(3);
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


	// tft.drawLine(0, 168, 320, 168, ILI9341_RED);
	// tft.drawLine(0, 162, 320, 162, ILI9341_RED);


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
		Serial.available() > 0
#endif

		) {
#if PROMINI
		key = Serial.read();
#else
		key = Serial.read();
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


void blank_upper_side() {
	int Base_y = 0;

	for (int y = 40; y < 124; y++) {
		if (y == 5) {
			Base_y++;
		}
		if (y == 15) {
			Base_y++;
		}
		tft.drawLine(18, Base_y + y, 85, Base_y + y, ILI9341_bk1);
		//   tft.drawLine(18, Base_y + y, 85, Base_y + y, ILI9341_BLUE);
	}
}

void paint_half_half() {

	int Base_y = 0;
	for (int y = 0; y <= 240; y++) {
		if (y < 163) {
			tft.drawLine(0, Base_y + y, 320, Base_y + y, ILI9341_bk1);
		}
		else {
			tft.drawLine(0, Base_y + y, 320, Base_y + y, ILI9341_WHITE);
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
float clip_percent(float num)
{
	if (num > 100)
	{
		return 100.0;
	}
	else if (num < 0)
	{
		return 0.0;
	}
	else
	{
		return num;
	}
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

bool graphics_to_Screen(int print_number)
{
	int Base_y = 0;
	tft.setRotation(3);
	tft.setCursor(8, 20);
	tft.setTextColor(ILI9341_WHITE);
#if PROMINI
	tft.setFont(&ArmentaFont32pt7b);
	tft.setTextSize(FontSizeArmenta);
#else
	tft.setFont(&ArmentaFont32pt7b);
	tft.setTextSize(FontSizeArmenta);
#endif
	tft.setCursor(8, Base_y + 100);
	bool printed = false;
	if ((print_number <= 99999) && (print_number >= 0))
	{
		tft.println(String(print_number));
		printed = true;
		counter = print_number;
	}
	return printed;
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
			Serial.print(" and takes ");
			Serial.print(millis() - currentmilis);
			Serial.println(F(" milisecond long"));
#endif
		}
	}
}

void parse_pulse_counter_test() {
	while (true)
	{
		// PERPETUAL BURN IN TEST. WILL RUN UNTIL RESET
		for (int pulse_count = 0; pulse_count < 100000; pulse_count++) {
			uint32_t currentmilis = millis();
			check_digits_changed_and_blank(pulse_count);
			bool printed = graphics_to_Screen(pulse_count);
			if (printed)
			{
#if DEBUG_FLAG
				Serial.print(F("Printing char is "));
				Serial.print(pulse_count);
				Serial.print(" and takes ");
				Serial.print(millis() - currentmilis);
				Serial.println(F(" milisecond long"));
#endif
			}
		}
		for (int pulse_count = 99999; pulse_count > 0; pulse_count--) {
			uint32_t currentmilis = millis();
			check_digits_changed_and_blank(pulse_count);
			bool printed = graphics_to_Screen(pulse_count);
			if (printed)
			{
#if DEBUG_FLAG
				Serial.print(F("Printing char is "));
				Serial.print(pulse_count);
				Serial.print(" and takes ");
				Serial.print(millis() - currentmilis);
				Serial.println(F(" milisecond long"));
#endif
			}
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
	int Base_y = 60;
	buf++;
	tft.setRotation(3);
	tft.setCursor(8, Base_y + 20);
	buf[2] = 0;
	int pressure = atoi(buf);
	if ((pressure >= 0) && (pressure <= 100)) {
		// TODO: add functionality depending on Selas' input in regards to 27 psi pressure.
		if (pressure > 27) {
			tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_high.pixel_data), pressure_high.width, pressure_high.height);
		}
		else {
			tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_low.pixel_data), pressure_low.width, pressure_low.height);
		}
	}
}

void parse_draw_rectangles(char* buf) {
	buf++;
	int rect_is = atoi(buf);
	tft.drawRect(0, 0, rect_is, rect_is, ILI9341_WHITE);
	delay(5000);
	tft.drawRect(0, 0, rect_is, rect_is, ILI9341_BLACK);
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

void PrintOnLcd(char* buf)
{
	// If the promini is the board of choise. There is no Serial1
#if PROMINI
	Serial.println(buf);
#else
	Serial.println(buf);
#endif
	if ((*buf == 'C') || (*buf == 'c'))
	{
		parse_pulse_counter(buf);
	} // enf if
	else if ((*buf == 'A') || (*buf == 'a'))
	{
		parse_pulse(buf);
	} // end if
	else if ((*buf == 'P') || (*buf == 'p'))
	{
		parse_pressure(buf);
	} // end if
	else if ((*buf == 't') || (*buf == 'T'))
	{
		parse_aplicator(buf);
	} // end if
	else if ((*buf == 'B') || (*buf == 'b'))
	{
		parse_battery_percent(buf);
	} // end if
	else if ((*buf == 'E') || (*buf == 'b'))
	{
		parse_E(buf);
	} // end if
	else if ((*buf == 'R') || (*buf == 'r'))
	{
		parse_pulse_counter_test();
	}
	else if ((*buf == 'D') || (*buf == 'd'))
	{
		parse_draw_rectangles(buf);
	}
}


void check_digits_changed_and_blank(int curr_number)
{
	// This function is blanking digits based on the difference modulo
	// Thus this is a symetric transform that works both descending and ascending
	bool last_digit_is_changed = !(((curr_number - counter) % 10) == 0);
	bool tens_digit_is_changed = !(((curr_number / 10 - counter / 10) % 10) == 0);
	bool hundreds_digit_is_changed = !(((curr_number / 100 - counter / 100) % 10) == 0);
	bool thousands_digit_is_changed = !(((curr_number / 1000 - counter / 1000) % 10) == 0);
	bool tens_of_thousands_digit_is_changed = !(((curr_number / 10000 - counter / 10000) % 10) == 0);
	bool flags[4];
	int digitL = 64;
	tft.setRotation(3);
	tft.setCursor(8, 20);
	if (tens_of_thousands_digit_is_changed)
	{
		if (curr_number >= 10000)
		{
			tft.fillRect(0, 0, 5 * digitL, 120, ILI9341_bk1);
		}
	}
	else if (thousands_digit_is_changed) {
		if (curr_number >= 10000)
		{
			tft.fillRect(digitL, 0, 4 * digitL, 120, ILI9341_bk1);
		}
		else if (curr_number >= 1000)
		{
			tft.fillRect(0, 0, 4 * digitL, 120, ILI9341_bk1);
		}
	}
	else if (hundreds_digit_is_changed)
	{
		if (curr_number >= 10000)
		{
			tft.fillRect(2 * digitL, 0, 3 * digitL + 1, 120, ILI9341_bk1);
		}
		else if (curr_number >= 1000)
		{
			tft.fillRect(digitL, 0, 3 * digitL + 1, 120, ILI9341_bk1); // The hundreds are one digit to the right
		}
		else
		{
			tft.fillRect(0, 0, 3 * digitL + 1, 120, ILI9341_bk1); // The hundreds are MSB
		}
	}
	else if (tens_digit_is_changed)
	{
		if (curr_number >= 10000)
		{
			tft.fillRect(3 * digitL, 0, 2 * digitL + 1, 120, ILI9341_bk1);
		}
		if (curr_number >= 1000)
		{
			tft.fillRect(2 * digitL, 0, 2 * digitL + 1, 120, ILI9341_bk1); // The tens are two digit to the right
		}
		else if (curr_number >= 100)
		{
			tft.fillRect(digitL, 0, 2 * digitL + 1, 120, ILI9341_bk1); // The tens are one digit to the right
		}
		else
		{
			tft.fillRect(0, 0, 2 * digitL + 10, 120, ILI9341_bk1); // The tens are MSB
		}
	}
	else if (last_digit_is_changed)
	{
		if (curr_number >= 10000)
		{
			tft.fillRect(4 * digitL, 0, digitL + 1, 120, ILI9341_bk1);
		}
		else if (curr_number >= 1000)
		{
			tft.fillRect(3 * digitL, 0, digitL + 1, 120, ILI9341_bk1); // The singles are 3 digit to the right
		}
		else if (curr_number >= 100)
		{
			tft.fillRect(2 * digitL, 0, digitL + 1, 120, ILI9341_bk1); // The singles are 2 digit to the right
		}
		else if (curr_number >= 10)
		{
			tft.fillRect(digitL, 0, digitL + 10, 120, ILI9341_bk1); // The singles are one digit to the right
		}
		else
		{
			tft.fillRect(0, 0, digitL + 10, 120, ILI9341_bk1); // The singles are MSB
		}
	}
}