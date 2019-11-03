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
			DRAW_AP_ERROR
		}
		else {
			DRAW_AP_OK
		}
		tft.setTextColor(ILI9341_BLACK);
		tft.setFont();
		tft.setTextSize(2);
		tft.setCursor(245, 180);
		tft.print(percentP);
		tft.print('%');
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
		else if (ammount_left == 0)
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

void parse_pressure(char* buf) {
	// TODO: change to parse pLOW or pO.K
	int Base_y = 60;
	buf++;
	tft.setRotation(3);
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
	tft.setRotation(3);
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

void parse_reset_screen(char* buf)
{
	reset_screen();
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

void blank_on_reset(char* buf)
{
	buf++;
	int new_counter = atoi(buf);
	tft.setRotation(3);
	tft.fillRect(0, 0, 320, 120, ILI9341_bk1);
	graphics_to_Screen(new_counter);
}
