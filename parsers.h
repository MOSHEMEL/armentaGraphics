#pragma once
#include "config.h"
extern int percentBattery;
extern uint32_t serial_number;
#ifndef _STDBOOL
#include "stdbool.h"
#endif
extern U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

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

uint32_t currentmilis;
uint16_t RGB888toRGB565(const char* rgb32_str_);
// ****** UTF8-Decoder: convert UTF8-string to extended ASCII *******
static byte c1;  // Last character buffer

// Convert a single Character from UTF8 to Extended ASCII
// Return "0" if a byte has to be ignored
byte utf8ascii(byte ascii) {
    if ( ascii<128 )   // Standard ASCII-set 0..0x7F handling  
    {   c1=0;
        return( ascii );
    }

    // get previous input
    byte last = c1;   // get last char
    c1=ascii;         // remember actual character

    switch (last)     // conversion depending on first UTF8-character
    {   case 0xC2: return  (ascii);  break;
        case 0xC3: return  (ascii | 0xC0);  break;
        case 0x82: if(ascii==0xAC) return(0x80);       // special case Euro-symbol
    }

    return  (0);                                     // otherwise: return zero, if character has to be ignored
}

// convert String object from UTF8 String to Extended ASCII
//String utf8ascii(String s)
//{      
//        String r="";
//        char c;
//        for (int i=0; i<s.length(); i++)
//        {
//                c = utf8ascii(s.charAt(i));
//                if (c!=0) r+=c;
//        }
//        return r;
//}

// In Place conversion UTF8-string to Extended ASCII (ASCII is shorter!)
void utf8ascii(char* s)
{      
        int k=0;
        char c;
        for (int i=0; i<strlen(s); i++)
        {
                c = utf8ascii(s[i]);
                if (c!=0)
                        s[k++]=c;
        }
        s[k]=0;
}
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
		display.setCursor(8, Base_y + 20);
		Base_y = 184;
		for (int y = 0; y < 15; y++) {
			//   delay(100);
			display.drawLine(245, Base_y + y, 295, Base_y + y, ILI9341_WHITE);
		}
		display.setTextColor(ILI9341_BLACK);
		display.setFont();
		display.setTextSize(2);
		display.setCursor(AM_TEXT_POS);
		display.print(percentP);
		display.print('%');
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
		display.drawLine(174, Base_y + y, 224, Base_y + y, ILI9341_WHITE);
	}
#if !SIMPLE_BAT
	display.setTextColor(ILI9341_BLACK);
	display.setFont();
	display.setTextSize(2);
	display.setCursor(BATTERY_TEXT_POS); 
	display.print(percentBattery);
	display.print('%');
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
			display.drawLine(168 + y, Base_y + 0, 168 + y, Base_y + 18,
				ILI9341_RED);
		}
	}
	else {
		for (int y = 0; y <= percentBattery * 42 / 100; y++) {
			//   delay(100);
			display.drawLine(168 + y, Base_y + 0, 168 + y, Base_y + 18,
				ILI9341_GREEN);
		}
	}
	for (int y = percentBattery * 42 / 100; y <= 42; y++) {
		//   delay(100);
		display.drawLine(168 + y, Base_y + 0, 168 + y, Base_y + 18, ILI9341_WHITE);
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
  char *buf;
 
 u8g2_for_adafruit_gfx.setForegroundColor(color);
	while (*pointer)
	{
		ascii = *pointer;
		len++;
		pointer++;
    
	}
	len = len * 6 * size;
	int poX = 160 - len / 2;

	if (poX < 0) poX = 0;
  utf8ascii(string);
  
	while (*string)
	{
		//  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
		// display.drawChar(poX, y, *string, color, bg_color, size);
		u8g2_for_adafruit_gfx.drawGlyph(poX, y,*string );
    Serial.print(*string);
		string++;
		poX += 6*size;                  /* Move cursor right */           
}

}


void parse_E(char* buf)
{ 
  char s1[40];
	buf++;

	int error_code = atoi(buf);
	if (error_code > 0)
	{
		if (error_code == 4000)
		{
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR14_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
			display.fillScreen(RGB888toRGB565("FFFF00"));
			display.setFont(); // we have no letters to show so we cant use font to print letters
			display.setTextColor(RGB888toRGB565("00B0F0"));
      memcpy(s1,LANG[0],40);
			align_center_print(s1, 30, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 5);
      memcpy(s1,LANG[1],40);
			align_center_print(s1, 90, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
      memcpy(s1,str_error,40);
			align_center_print(s1, 150, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 3);
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR12_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
		}
		else if (error_code == 4001)
		{
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR14_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
			display.fillScreen(Warning_RED);
			display.setFont(); // we have no letters to show so we cant use font to print letters
			display.setTextColor(ILI9341_WHITE);
      memcpy(s1,LANG[3],40);
			align_center_print(s1, 30, ILI9341_WHITE, Warning_RED, 6);
      memcpy(s1,LANG[4],40);
			align_center_print(s1, 90, ILI9341_WHITE, Warning_RED, 3);
      memcpy(s1,LANG[5],40);
			align_center_print(s1, 120, ILI9341_WHITE, Warning_RED, 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
      memcpy(s1,str_error,40);
			align_center_print(s1, 150, ILI9341_WHITE, Warning_RED, 3);
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR12_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
		}
		else if (error_code == 4010 || error_code == 4011)
		{
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR14_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
			display.fillScreen(Warning_RED);
			display.setFont(); // we have no letters to show so we cant use font to print letters
			display.setTextColor(ILI9341_WHITE);
      memcpy(s1,LANG[3],40);
			align_center_print(s1, 30, ILI9341_WHITE, Warning_RED, 6);
      memcpy(s1,LANG[6],40);
			align_center_print(s1, 90, ILI9341_WHITE, Warning_RED, 3);
			char str_error[10]; 
			sprintf(str_error, "E%d", error_code);
      memcpy(s1,str_error,40); 
			align_center_print(s1, 150, ILI9341_WHITE, Warning_RED, 3);
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR12_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
		}
		else if (error_code >= 5000 && error_code < 7000)
		{
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR14_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
			display.fillScreen(Warning_RED);
			display.setFont(); // we have no letters to show so we cant use font to print letters
			display.setTextColor(ILI9341_WHITE);
      memcpy(s1,LANG[3],40);
			align_center_print(s1, 30, ILI9341_WHITE, Warning_RED, 6);
      memcpy(s1,LANG[6],40);
			align_center_print(s1, 90, ILI9341_WHITE, Warning_RED, 3);
      memcpy(s1,LANG[2],40);
			align_center_print(s1, 120, ILI9341_WHITE, Warning_RED, 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
      memcpy(s1,str_error,40);
			align_center_print(s1, 150, ILI9341_WHITE, Warning_RED, 3);
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR12_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
		}
		else if (error_code == 503)
		{
			/*
			 *  Error
				APT Maintenance Required
				Contact APT Service 
				E503

			 */
			display.fillScreen(Warning_RED);
			display.setFont(); // we have no letters to show so we cant use font to print letters
			display.setTextColor(ILI9341_WHITE);
      memcpy(s1,LANG[3],40);
			align_center_print(s1, 30, ILI9341_WHITE, Warning_RED, 6);
      u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR10_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
      memcpy(s1,LANG[7],40);
			align_center_print(s1, 90, ILI9341_WHITE, Warning_RED, 2);
     u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR12_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
      memcpy(s1,LANG[2],40);
			align_center_print(s1, 120, ILI9341_WHITE, Warning_RED, 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
      memcpy(s1,str_error,40);
			align_center_print(s1, 150, ILI9341_WHITE, Warning_RED, 3);
		}
		else if (error_code == 504)
		{
			/*
			 * ATTENTION
				APT Maintenance Required
				Contact APT Service 
				E504

			 */
			display.fillScreen(RGB888toRGB565("FFFF00"));
			display.setFont(); // we have no letters to show so we cant use font to print letters
			display.setTextColor(RGB888toRGB565("00B0F0"));
			align_center_print(LANG[0], 30, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 5);
     u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR10_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
			align_center_print(LANG[7], 90, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 2);
     u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR12_tf);
      u8g2_for_adafruit_gfx.setFontMode(1);
			align_center_print(LANG[2], 120, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 2);
			char str_error[10];
			sprintf(str_error, "E%d", error_code);
			align_center_print(str_error, 150, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 3);

		}
	// After we draw the screen - we then show the 
	delay(2000);
	display.setFont();
	display.setTextSize(2);
	reset_screen();
	}
}


void parse_fail(char* buf)
{
	buf++;
	
	int ammount_left = atoi(buf);
    //int ammount_left=1200;
	if ((ammount_left >= 0) && (ammount_left % 200 == 0)) {
		if (ammount_left==0)
		{
			display.fillScreen(Warning_RED);
			display.setFont(); // we have no letters to show so we cant use font to print letters
			align_center_print(LANG[8], 35, ILI9341_WHITE, Warning_RED, 4);
			align_center_print(LANG[9], 65, ILI9341_WHITE, Warning_RED, 4);

		}
		else
		{
			display.fillScreen(RGB888toRGB565("FFFF00"));
			display.setFont(); // we have no letters to show so we cant use font to print letters

			align_center_print(LANG[0], 25, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 3);
			align_center_print(LANG[8], 55, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 3);
			align_center_print(LANG[9], 85, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 3);
		}

#if PROMINI
		display.setFont(&ArmentaFont32pt7b);
		display.setTextSize(FontSizeArmenta);
#else
		display.setFont(&ArmentaFont64pt7b);
		display.setTextSize(FontSizeArmenta / 2);
#endif
		if (ammount_left >= 1000)
		{
			display.setCursor(30, 200);
			display.println(ammount_left);
			display.setFont();
			display.setTextSize(2);
		}
		else if (ammount_left == 0)
		{
			display.setCursor(130, 200);
			display.println(ammount_left);
			display.setFont();
			display.setTextSize(2);
      u8g2_for_adafruit_gfx.setCursor(100,220);  
      u8g2_for_adafruit_gfx.print(LANG[6]);
		//	display.setCursor(100, 220);
		//	display.println(REPLACE_AM);
		}
		else
		{
			display.setCursor(80, 200);
			display.println(ammount_left);
			display.setFont();
			display.setTextSize(2);
		}
	}



	// After we draw the screen - we then show the 
	delay(2000);
	display.setFont();
	display.setTextSize(2);
	reset_screen();
}

void parse_pressure(char* buf) {
	int Base_y = 60;
	buf++;
	
	display.setCursor(8, Base_y + 20);
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
	
	display.setCursor(8, Base_y + 20);
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
	currentmilis = millis();
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
			display.setTextColor(ILI9341_WHITE);
			display.setFont();
			display.setTextSize(2);
			display.setCursor(8, 20);
			display.print(String(new_counter));
		}

	}
}

void parse_force_pulse_counter(char* buf)
{
	// This function is sent always even if no number needs to be printed
	buf++;
	int new_counter = atoi(buf);
	if (new_counter < 100000)
	{
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
		display.setTextColor(ILI9341_WHITE);
		display.setFont();
		display.setTextSize(2);
		display.setCursor(8, 20);
		display.print(String(new_counter));
	}	
}


void parse_reset_screen(char* buf)
{
	reset_screen();
}

void print_error(char* buf)
{
	buf++;
	display.fillScreen(Warning_RED);
	display.setFont();
	
	display.setTextColor(ILI9341_WHITE);
	display.setTextSize(6);
	display.setCursor(50, 30);
	display.println("Error");
	display.setCursor(50, 90);
	display.setTextSize(2);
  uint16_t size = 0;
  while(buf[size] != '\0')
  {
    size ++;
  }
  buf[size-1] = '\0';
	display.println(buf);
	delay(10000);
	reset_screen();
}

void print_warning(char* buf)
{
	buf++;
	display.fillScreen(Warning_BLUE);
	display.setFont();

	display.setTextColor(ILI9341_WHITE);
	display.setTextSize(6);
	display.setCursor(50, 30);
	display.println("Warning");
	display.setCursor(50, 90);
	display.setTextSize(2);
	uint16_t size = 0;
	while (buf[size] != '\0')
	{
		size++;
	}
	buf[size - 1] = '\0';
	display.println(buf);
	delay(10000);
	reset_screen();
}

void blank_on_reset(char* buf)
{
	buf++;
	int new_counter = atoi(buf);
	display.fillRect(0, 0, 320, 120, ILI9341_bk1);
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
	display.setTextColor(ILI9341_BLACK);
	display.setFont();
	display.setTextSize(1);
	display.setCursor(10, 155);
	display.print("Armenta ltd. gfx:");
	display.print(VERSION);
	display.print(" mcu:");
	display.print(MCU_version);
}
void parse_cs(char* buf)
{
	buf++;
	display.fillScreen(ILI9341_WHITE);
	display.setFont();
	display.setTextColor(ILI9341_BLACK);
	display.setTextSize(2);
	display.setCursor(0, 30);
	display.println(buf);
	delay(5000);
	reset_screen();
}
void parse_serial(char* buf)
{
	buf++;
	serial_number = atol(buf);
	Serial.print("S\N number is ");
	Serial.println(serial_number);
}

void parse_serial_show(char* buf)
{
  //Serial.print("show");
	buf++;
	//uint16_t bg_color = RGB888toRGB565("FFFFFF");
	//uint16_t text_color = RGB888toRGB565("000000");
	uint16_t bg_color = Warning_BLUE;
	uint16_t text_color = ILI9341_WHITE;
	display.setFont(); // we have no letters to show so we cant use font to print letters


	char str_serial_status[30];
	int remaining_pulses = atoi(buf);
	if (remaining_pulses > 5000)
	{
		//bg_color = RGB888toRGB565("00B050");
		//text_color = RGB888toRGB565("DDBD0B");
		uint16_t bg_color = Warning_BLUE;
		uint16_t text_color = ILI9341_WHITE;
		display.setTextColor(text_color);
		display.fillScreen(bg_color);
	}
	else if(remaining_pulses > 0)
	{
		//bg_color = RGB888toRGB565("FFFF00");
		//text_color = RGB888toRGB565("2ABDC8");
		uint16_t bg_color = Warning_BLUE;
		uint16_t text_color = ILI9341_WHITE;
		display.setTextColor(text_color);
		display.fillScreen(bg_color);
		sprintf(str_serial_status, LANG[10]);
		align_center_print(str_serial_status, 120, text_color, bg_color, 2);
		sprintf(str_serial_status, LANG[11]);
		align_center_print(str_serial_status, 140, text_color, bg_color, 2);
		sprintf(str_serial_status, LANG[12]);
		align_center_print(str_serial_status, 160, text_color, bg_color, 2);
    #ifdef SPANISH
    sprintf(str_serial_status, LANG[15]);
    align_center_print(str_serial_status, 180, text_color, bg_color, 2);
    #endif
	}
	else
	{

		bg_color = RGB888toRGB565("FF0000");
		text_color = RGB888toRGB565("FFFFFF");
		display.setTextColor(text_color);
		display.fillScreen(bg_color);
	}

	sprintf(str_serial_status, LANG[13], serial_number);
	align_center_print(str_serial_status, 30, text_color, bg_color, 3);
	sprintf(str_serial_status, LANG[14], remaining_pulses);
	align_center_print(str_serial_status, 90, text_color, bg_color, 3);
	
}

uint16_t color_565_from_888(uint32_t RGB888)
{
	uint16_t RGB565 = (((RGB888 & 0xf80000) >> 8) + ((RGB888 & 0xfc00) >> 5) + ((RGB888 & 0xf8) >> 3));
	return RGB565;
}
