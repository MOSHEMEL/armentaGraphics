#ifndef _STDBOOL
#include "stdbool.h"
#endif
#include "config.h"
#include "Adafruit_ILI9341.h"
#include "ArmentaFont.h"
#include "ArmentaFontHuge.h"
extern Adafruit_ILI9341 display ;
#define ILI9341_bk1        0x0555

extern int counter;
extern int FontSizeArmenta;
void blank_upper_side(void);
void check_digits_changed_and_blank(int curr_number);
float clip_percent(float num);
bool graphics_to_Screen(int print_number);
void paint_half_half();
void reset_screen();

void blank_5_digits(int curr_number, int digitL)
{
	if (curr_number >= 10000 || counter >= 10000)
	{
		display.fillRect(0, 0, 5 * digitL, 120, ILI9341_bk1); // width_all = 5; offset = 0;
	}
}

void blank_4_digits(int curr_number, int digitL)
{
	if (curr_number >= 10000 || counter >= 10000)
	{
		display.fillRect(digitL, 0, 4 * digitL, 120, ILI9341_bk1); // width_all = 5; offset = 1;
	}
	else if (curr_number >= 1000 || counter >= 1000)
	{
		display.fillRect(0, 0, 4 * digitL, 120, ILI9341_bk1); // width_all = 4; offset = 0;
	}
}

void blank_3_digits(int curr_number, int digitL)
{
	if (curr_number >= 10000 || counter >= 10000)
	{
		display.fillRect(2 * digitL, 0, 3 * digitL + 1, 120, ILI9341_bk1); // width_all = 5; offset = 2;
	}
	else if (curr_number >= 1000 || counter >= 1000)
	{
		display.fillRect(digitL, 0, 3 * digitL + 1, 120, ILI9341_bk1); // width_all = 4; offset = 1;
	}
	else
	{
		display.fillRect(0, 0, 3 * digitL + 1, 120, ILI9341_bk1); // width_all = 3; offset = 0;
	}
}

void blank_2_digits(int curr_number, int digitL)
{
	if (curr_number >= 10000 || counter >= 10000)
	{
		display.fillRect(3 * digitL, 0, 2 * digitL + 1, 120, ILI9341_bk1); // width_all = 5; offset = 3;
	}
	if (curr_number >= 1000 || counter >= 1000)
	{
		display.fillRect(2 * digitL, 0, 2 * digitL + 1, 120, ILI9341_bk1); // width_all = 4; offset = 2;
	}
	else if (curr_number >= 100 || counter >= 100)
	{
		display.fillRect(digitL, 0, 2 * digitL + 1, 120, ILI9341_bk1); // width_all = 3; offset = 1;
	}
	else
	{
		display.fillRect(0, 0, 2 * digitL + 10, 120, ILI9341_bk1); // width_all = 2; offset = 0;
	}
}

void blank_1_digits(int curr_number, int digitL)
{
	if (curr_number % 5==1)
	{
		display.fillRect(0, 0, 320, 120, ILI9341_bk1); // blank everything each 5 times
	}
	else if (curr_number >= 10000 || counter >= 10000)
	{
		display.fillRect(4 * digitL, 0, digitL + 1, 120, ILI9341_bk1); // width_all = 5; offset = 4;
	}
	else if (curr_number >= 1000 || counter >= 1000)
	{
		display.fillRect(3 * digitL, 0, digitL + 1, 120, ILI9341_bk1); // width_all = 4; offset = 3;
	}
	else if (curr_number >= 100 || counter >= 100)
	{
		display.fillRect(2 * digitL, 0, digitL + 1, 120, ILI9341_bk1); // width_all = 3; offset = 2;
	}
	else if (curr_number >= 10 || counter >= 10)
	{
		display.fillRect(digitL, 0, digitL + 10, 120, ILI9341_bk1); // width_all = 2; offset = 1;
	}
	else
	{
		display.fillRect(0, 0, digitL + 10, 120, ILI9341_bk1); // width_all = 1; offset = 0;
	}
}

void check_digits_changed_and_blank(int curr_number)
{
	// This function is blanking digits based on the difference modulo
	// Thus this is a symetric transform that works both descending and ascending
	//bool last_digit_is_changed = !(((curr_number - counter% 10== 0)));
	//bool tens_digit_is_changed = !(((curr_number / 10 - counter / 10% 10== 0)));
	//bool hundreds_digit_is_changed = !(((curr_number / 100 - counter / 100% 10== 0)));
	//bool thousands_digit_is_changed = !(((curr_number / 1000 - counter / 1000% 10== 0)));
	//bool tens_of_thousands_digit_is_changed = !(((curr_number / 10000 - counter / 10000% 10== 0)));
	bool last_digit_is_changed = !(((curr_number - counter) % 10) == 0);
	bool tens_digit_is_changed = !(((curr_number / 10 - counter / 10) % 10) == 0);
	bool hundreds_digit_is_changed = !(((curr_number / 100 - counter / 100) % 10) == 0);
	bool thousands_digit_is_changed = !(((curr_number / 1000 - counter / 1000) % 10) == 0);
	bool tens_of_thousands_digit_is_changed = !(((curr_number / 10000 - counter / 10000) % 10) == 0);
	bool flags[4];
	int digitL = 64;
	
	display.setCursor(8, 20);
	if (tens_of_thousands_digit_is_changed)
	{
		blank_5_digits(curr_number, digitL);
	}
	else if (thousands_digit_is_changed){
		blank_4_digits(curr_number, digitL);
	}
	else if (hundreds_digit_is_changed)
	{
		blank_3_digits(curr_number, digitL);
	}
	else if (tens_digit_is_changed)
	{
		blank_2_digits(curr_number, digitL);
	}
	else if (last_digit_is_changed)
	{
		blank_1_digits(curr_number, digitL);
	}
}

void blank_upper_side()
{
	int Base_y = 0;

	for (int y = 40; y < 124; y++){
		if (y == 5){
			Base_y++;
		}
		if (y == 15){
			Base_y++;
		}
		display.drawLine(18, Base_y + y, 85, Base_y + y, ILI9341_bk1);
		//   display.drawLine(18, Base_y + y, 85, Base_y + y, ILI9341_BLUE);
	}
}

void paint_half_half()
{
	int Base_y = 0;
	for (int y = 0; y <= 240; y++){
		if (y < 163){
			display.drawLine(0, Base_y + y, 320, Base_y + y, ILI9341_bk1);
		}
		else {
			display.drawLine(0, Base_y + y, 320, Base_y + y, ILI9341_WHITE);
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

bool graphics_to_Screen(int print_number)
{
	int Base_y = 0;
	
	display.setCursor(8, 20);
	display.setTextColor(ILI9341_WHITE);
#if PROMINI
	display.setFont(&ArmentaFont32pt7b);
	display.setTextSize(FontSizeArmenta);
#else
	display.setFont(&ArmentaFont64pt7b);
	display.setTextSize(FontSizeArmenta / 2);
#endif
	display.setCursor(8, Base_y + 100);
	bool printed = false;
	if ((print_number <= 99999)&& (print_number >= 0))
	{
		display.println(String(print_number));
		printed = true;
		counter = print_number;
	}
  return printed;
}

void reset_screen()
{
	paint_half_half();
	DRAW_PULSE_N
	DRAW_BATTERY
	DRAW_PRESSURE_LOW
	DRAW_AP_OK
	
	display.setFont(); // Probably the reason for the dots
	// am percentage
	display.setTextColor(ILI9341_BLACK);  display.setTextSize(2);
	display.setCursor(AM_TEXT_POS);
	display.println("...");

	// battery percentage
	display.setTextColor(ILI9341_BLACK);  display.setTextSize(2);
	display.setCursor(BATTERY_TEXT_POS);
	display.println("...");
}
