#ifndef _STDBOOL
	#include "stdbool.h"
#endif
#include "Adafruit_ILI9341.h"
#include "ArmentaFont.h"
#include "ArmentaFontHuge.h"
extern Adafruit_ILI9341 tft;
#define ILI9341_bk1        0x0555
extern int counter;
extern int FontSizeArmenta;

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

void parse_draw_rectangles(char* buf) {
	buf++;
	int rect_is = atoi(buf);
	tft.drawRect(0, 0, rect_is, rect_is, ILI9341_WHITE);
	delay(5000);
	tft.drawRect(0, 0, rect_is, rect_is, ILI9341_BLACK);
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
	tft.setFont(&ArmentaFont64pt7b);
	tft.setTextSize(FontSizeArmenta/2);
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

void parse_pulse_counter_test(char* buf) {
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