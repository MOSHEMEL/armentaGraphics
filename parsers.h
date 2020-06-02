#pragma once
#include "config.h"
extern int percentBattery;
#ifndef _STDBOOL
#include "stdbool.h"
#endif

void parse_aplicator(char* buf);
void parse_battery_percent(char* buf);
void parse_reset_screen(char* buf);
void parse_E(char* buf);
void parse_pressure(char* buf);
void parse_pulse(char* buf);
void parse_fail(char* buf);
void parse_pulse_counter(char* buf);
void print_error(char* buf);
void blank_on_reset(char* buf);
void parse_draw_rectangles(char* buf);
void parse_pulse_counter_test(char* buf);
void parse_version(char* buf);
void parse_test_battery(void);
void parse_cs(char* buf);
uint16_t RGB888toRGB565(const char* rgb32_str_);

uint16_t RGB888toRGB565(const char* rgb32_str_)
{
	long rgb32 = strtoul(rgb32_str_, 0, 16);
	return (rgb32 >> 8 & 0xf800) | (rgb32 >> 5 & 0x07e0) | (rgb32 >> 3 & 0x001f);
}

void parse_aplicator(char* buf) {
	int Base_y = 60;
	buf++;
	int percentP = atoi(buf);
	if ((percentP >= 0) && (percentP <= 100)) {
		

	#if PROMINI
		Serial.print("Percent Applicaor Counter");
		Serial.println(percentP);
	#else
		Serial.print("Percent Applicaor Counter");
		Serial.println(percentP);
	#endif
		if (percentP <= 10) // l0 percent
		{
			DRAW_AP_ERROR
		}
		else {
			DRAW_AP_OK
		}
		tft.setCursor(8, Base_y + 20);
		Base_y = 184;
		for (int y = 0; y < 15; y++) {
			//   delay(100);
			tft.drawLine(245, Base_y + y, 295, Base_y + y, ILI9341_WHITE);
		}
		tft.setTextColor(ILI9341_BLACK);
		tft.setFont();
		tft.setTextSize(2);
		tft.setCursor(AM_TEXT_POS);
		tft.print(percentP);
		tft.print('%');
	}
}

void parse_battery_percent(char* buf) {
	int Base_y = 0;
	buf++;
	int newpercentBattery = atoi(buf);
	percentBattery = clip_percent(newpercentBattery);
	
	// battery blank prev text
	Base_y = 184;
	for (int y = 0; y < 15; y++) {
		tft.drawLine(174, Base_y + y, 224, Base_y + y, ILI9341_WHITE);
	}
#if !SIMPLE_BAT
	tft.setTextColor(ILI9341_BLACK);
	tft.setFont();
	tft.setTextSize(2);
	tft.setCursor(BATTERY_TEXT_POS); 
	tft.print(percentBattery);
	tft.print('%');
#endif
	
#if PROMINI
	Serial.print("Percent Battery ");
	Serial.println(percentBattery);
#else
	Serial.print("Percent Battery ");
	Serial.println(percentBattery);
#endif

#if SIMPLE_BAT
	if (percentBattery <= 10)
	{
		percentBattery = 10;
	}
	else 
	{
		percentBattery = 100;
	}
#endif

	Base_y = 208;
	if (percentBattery <= 10) 
	{
		for (int y = 0; y <= percentBattery * 42 / 100; y++) 
		{
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

void align_center_print(char *string, int y, uint16_t color, uint16_t bg_color, int size)
{

	/*
	 * TextSize(1)
		The space occupied by a character using the standard font is 6 pixels wide by 8 pixels high.
		A two characters string sent with this command occup a space that is 12 pixels wide by 8 pixels high.
	 */
	int len = 0;
	char *pointer = string;
	char ascii;

	while (*pointer)
	{
		ascii = *pointer;
		len++;
		* pointer++;
	}
	len = len * 6 * size;
	int poX = 160 - len / 2;

	if (poX < 0) poX = 0;

	while (*string)
	{
		//  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
		tft.drawChar(poX, y, *string, color, bg_color, size);
		*string++;
		poX += 6*size;                  /* Move cursor right            */
	}

}


void parse_E(char* buf)
{
	buf++;

	int error_code = atoi(buf);
	if (error_code > 0)
	{
		if (error_code == 4000)
		{
			tft.fillScreen(RGB888toRGB565("FFFF00"));
			tft.setFont(); // we have no letters to show so we cant use font to print letters
			tft.setTextColor(RGB888toRGB565("00B0F0"));
			align_center_print("Attention", 30, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 5);
			align_center_print("Notify APT Service", 90, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
			align_center_print(str_error, 150, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 3);
		}
		else if (error_code == 4001)
		{
			tft.fillScreen(Warning_RED);
			tft.setFont(); // we have no letters to show so we cant use font to print letters
			tft.setTextColor(ILI9341_WHITE);
			align_center_print("Error", 30, ILI9341_WHITE, Warning_RED, 6);
			align_center_print("Turn Off And", 90, ILI9341_WHITE, Warning_RED, 3);
			align_center_print("Reconnect APT", 120, ILI9341_WHITE, Warning_RED, 3);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
			align_center_print(str_error, 150, ILI9341_WHITE, Warning_RED, 3);
		}
		else if (error_code == 4010 || error_code == 4011)
		{
			tft.fillScreen(Warning_RED);
			tft.setFont(); // we have no letters to show so we cant use font to print letters
			tft.setTextColor(ILI9341_WHITE);
			align_center_print("Error", 30, ILI9341_WHITE, Warning_RED, 6);
			align_center_print("Replace AM", 90, ILI9341_WHITE, Warning_RED, 3);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
			align_center_print(str_error, 150, ILI9341_WHITE, Warning_RED, 3);
		}
		else if (error_code >= 5000 && error_code < 7000)
		{
			tft.fillScreen(Warning_RED);
			tft.setFont(); // we have no letters to show so we cant use font to print letters
			tft.setTextColor(ILI9341_WHITE);
			align_center_print("Error", 30, ILI9341_WHITE, Warning_RED, 6);
			align_center_print("Replace AM", 90, ILI9341_WHITE, Warning_RED, 3);
			align_center_print("Contact APT Service", 120, ILI9341_WHITE, Warning_RED, 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
			align_center_print(str_error, 150, ILI9341_WHITE, Warning_RED, 3);
		}
		else if (error_code == 503)
		{
			/*
			 *  Error
				APT Maintenance Required
				Contact APT Service 
				E503

			 */
			tft.fillScreen(Warning_RED);
			tft.setFont(); // we have no letters to show so we cant use font to print letters
			tft.setTextColor(ILI9341_WHITE);
			align_center_print("Error", 30, ILI9341_WHITE, Warning_RED, 6);
			align_center_print("APT Maintenance Required", 90, ILI9341_WHITE, Warning_RED, 2);
			align_center_print("Contact APT Service", 120, ILI9341_WHITE, Warning_RED, 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
			align_center_print(str_error, 150, ILI9341_WHITE, Warning_RED, 3);
		}
		else if (error_code == 504)
		{
			/*
			 * ATTENTION
				APT Maintenance Required
				Contact APT Service 
				E504

			 */
			tft.fillScreen(RGB888toRGB565("FFFF00"));
			tft.setFont(); // we have no letters to show so we cant use font to print letters
			tft.setTextColor(RGB888toRGB565("00B0F0"));
			align_center_print("Attention", 30, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 5);
			align_center_print("APT Maintenance Required", 90, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 2);
			align_center_print("Contact APT Service", 120, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
			align_center_print(str_error, 150, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 3);

		}
	// After we draw the screen - we then show the 
	delay(2000);
	tft.setFont();
	tft.setTextSize(2);
	reset_screen();
	}
}


void parse_fail(char* buf)
{
	buf++;
	
	int ammount_left = atoi(buf);
	if ((ammount_left >= 0) && (ammount_left % 200 == 0)) {
		if (ammount_left==0)
		{
			tft.fillScreen(Warning_RED);
			tft.setFont(); // we have no letters to show so we cant use font to print letters
			tft.setTextSize(4);
			tft.setTextColor(ILI9341_WHITE);
			tft.setCursor(50, 35);
			tft.println("AM Pulses");
			tft.setCursor(50, 65);
			tft.println("Remaining");
		}
		else
		{
			tft.fillScreen(RGB888toRGB565("FFFF00"));
			tft.setFont(); // we have no letters to show so we cant use font to print letters
			tft.setTextSize(3);
			tft.setTextColor(RGB888toRGB565("00B0F0"));
			tft.setCursor(75, 5);
			tft.println("Attention");
			tft.setCursor(75, 35);
			tft.println("AM Pulses");
			tft.setCursor(75, 65);
			tft.println("Remaining");
		}

#if PROMINI
		tft.setFont(&ArmentaFont32pt7b);
		tft.setTextSize(FontSizeArmenta);
#else
		tft.setFont(&ArmentaFont64pt7b);
		tft.setTextSize(FontSizeArmenta / 2);
#endif
		if (ammount_left >= 1000)
		{
			tft.setCursor(30, 200);
			tft.println(ammount_left);
			tft.setFont();
			tft.setTextSize(2);
		}
		else if (ammount_left == 0)
		{
			tft.setCursor(130, 200);
			tft.println(ammount_left);
			tft.setFont();
			tft.setTextSize(2);

			tft.setCursor(100, 220);
			tft.println("Replace AM");
		}
		else
		{
			tft.setCursor(80, 200);
			tft.println(ammount_left);
			tft.setFont();
			tft.setTextSize(2);
		}
	}



	// After we draw the screen - we then show the 
	delay(2000);
	tft.setFont();
	tft.setTextSize(2);
	reset_screen();
}

void parse_pressure(char* buf) {
	int Base_y = 60;
	buf++;
	
	tft.setCursor(8, Base_y + 20);
	if (strncmp(buf, "LOW",3) == 0)
	{
		DRAW_PRESSURE_LOW
	}
	else if (strncmp(buf, "O.K",3) == 0)
	{
		DRAW_PRESSURE_HIGH
	}
}

void parse_pulse(char* buf) {
	int Base_y = 60;
	buf++;
	
	tft.setCursor(8, Base_y + 20);
	buf[1] = 0;
	if ((*buf == 'Y') || (*buf == 'y')) {
		DRAW_PULSE_Y
	}
	else {
		DRAW_PULSE_N
	}
}

void parse_pulse_counter(char* buf)
{
	// This function is sent always even if no number needs to be printed
	uint32_t currentmilis = millis();
	buf++;
	int new_counter = atoi(buf);
	if (counter != new_counter)
	{
		if(new_counter<100000)
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
		else
		{
			blank_5_digits(new_counter, 64);
			tft.setTextColor(ILI9341_WHITE);
			tft.setFont();
			tft.setTextSize(2);
			tft.setCursor(8, 20);
			tft.print(String(new_counter));
		}

	}
}

void parse_reset_screen(char* buf)
{
	reset_screen();
}

void print_error(char* buf)
{
	buf++;
	tft.fillScreen(Warning_RED);
	tft.setFont();
	
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(6);
	tft.setCursor(50, 30);
	tft.println("Error");
	tft.setCursor(50, 90);
	tft.setTextSize(2);
  uint16_t size = 0;
  while(buf[size] != '\0')
  {
    size ++;
  }
  buf[size-1] = '\0';
	tft.println(buf);
	delay(10000);
	reset_screen();
}

void print_warning(char* buf)
{
	buf++;
	tft.fillScreen(Warning_BLUE);
	tft.setFont();

	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(6);
	tft.setCursor(50, 30);
	tft.println("Warning");
	tft.setCursor(50, 90);
	tft.setTextSize(2);
	uint16_t size = 0;
	while (buf[size] != '\0')
	{
		size++;
	}
	buf[size - 1] = '\0';
	tft.println(buf);
	delay(10000);
	reset_screen();
}

void blank_on_reset(char* buf)
{
	buf++;
	int new_counter = atoi(buf);
	tft.fillRect(0, 0, 320, 120, ILI9341_bk1);
	graphics_to_Screen(new_counter);
}

void parse_test_battery(void)
{
	char buf[4];
	buf[0] = 'b';
	for (int i = 0; i <= 100; i++)
	{
		String buffer = String(i);
		buffer.toCharArray(&buf[1], 4);
		parse_battery_percent(buf);
		delay(1);
	}
}

void parse_version(char* buf)
{
	String MCU_version = String(buf);
	MCU_version.toLowerCase();
	tft.setTextColor(ILI9341_BLACK);
	tft.setFont();
	tft.setTextSize(1);
	tft.setCursor(10, 155);
	tft.print("Armenta ltd. gfx:");
	tft.print(VERSION);
	tft.print(" mcu:");
	tft.print(MCU_version);
}
void parse_cs(char* buf)
{
	buf++;
	tft.fillScreen(ILI9341_WHITE);
	tft.setFont();
	tft.setTextColor(ILI9341_BLACK);
	tft.setTextSize(2);
	tft.setCursor(0, 30);
	tft.println(buf);
	delay(5000);
	reset_screen();
}

uint16_t color_565_from_888(uint32_t RGB888)
{
	uint16_t RGB565 = (((RGB888 & 0xf80000) >> 8) + ((RGB888 & 0xfc00) >> 5) + ((RGB888 & 0xf8) >> 3));
	return RGB565;
}