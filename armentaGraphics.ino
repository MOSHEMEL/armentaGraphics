
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <I2S.h>
#include <Adafruit_GFX.h>
#include "config.h"
#include "armenta.h"
#include "arduino_aux.h"
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include "U8g2_for_Adafruit_GFX.h"
#include <SPI.h>
#include <SD.h>
#include "parsers.h"
#include <cstdlib>

/*
   16.09.19 Armenta Copyright
   Nick comments:
   1. change pinout on ili9341 according to working example used in previous code
   2. add support pin to adress cs on sd_card (currently pin4) - wire is conected to CCS pinout on example
   3. Give better names
   4. display.drawRGBBitmap function currently adresses by row, col which are iterators and not positional x,y
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
#define TFT_DC 6
#define TFT_CS 7


 // TFT display and SD card will share the hardware SPI interface.
 // Hardware SPI pins are specific to the Arduino board type and
 // cannot be remapped to alternate pins.  For Arduino Uno,
 // Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
#if PROMINI
Adafruit_ILI9341 display = Adafruit_ILI9341(10, 9);
#else
//Adafruit_ILI9341 tft = Adafruit_ILI9341(7, 6);
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);
#endif
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

#define TFT_RST 5
// Use hardware SPI (on MKRZERO, #8, #9, #10) and the above for CS/DC
// If using the breakout, change pins as desired
bool led_toggle = 0;
//extern char LANG[16][40];
extern char* LANG[LANG_STR_NUM];
int y = 0;
char  BufferString[MAX_COMMAND_LENGTH];
char key;
int counter = -1;
int percentBattery = 0;
int FontSizeArmenta = 2;
extern byte not_show_reset;
bool watchdog_wakeup = false;
uint32_t watchdog_last_update = 0;
uint32_t serial_number = 0;
int remaining_pulses = 0;
enum serial_state
{
	IDLE, WAITING_LENGTH, BUILDING_BUFFER, CHECKSUM
};

class Serial_Message {
public:
	bool start_message;
	bool awaiting_checksum;
	char msg_length;
	char* msg_buf;
	uint16_t checksum;
	serial_state state;
	serial_state next_state;
};
Serial_Message serial_message;

uint16_t calc_crc(uint8_t* data, int length)
{
	uint16_t crc = 0u;
	while (length)
	{
		length--;
		crc = crc ^ ((uint16_t)*data++ << 8u);
		for (uint8_t i = 0u; i < 8u; i++)
		{
			if (crc & 0x8000u)
			{
				crc = (crc << 1u) ^ 0x1021u;
			}
			else
			{
				crc = crc << 1u;
			}
		}
	}
	return crc;
}

void uart_armenta_logo()
{
	static const char logo[] = ""
		" NNNN                    NNNN  \r\n"
		" NNNNiiiiiiiiiiiiiiiiiiiiNNNN  \r\n"
		"  NNNNNNNNNNNNNNNNNNNNNNNNNN   \r\n"
		"             iNNi              \r\n"
		"    oooo     iNNi     oooo     \r\n"
		"    pppp     iNNi     pppp     \r\n"
		"             iNNi              \r\n"
		"             iNNi              \r\n"
		"             iNNi              \r\n"
		"             iNNi              \r\n"
		"             iNNi              \r\n"
		"             iNNi              \r\n"
		"             iNNi              \r\n"
		"             iNNi              \r\n"
		"             iNNi              \r\n";
	Serial.print(logo);
	Serial.print("ARMenta version: ");
	Serial.println(VERSION);
}

void PrintOnLcd(char* buf)
{
	// If the promini is the board of choise. There is no Serial1
	//Serial.println(buf);
	if ((*buf == 's') || (*buf == 'S')) // counter [number]
	{
		parse_lang(buf);
	}
	if ((*buf == 'c') || (*buf == 'C')) // counter [number]
	{
		parse_pulse_counter(buf);
	}
	if ((*buf == 'd') || (*buf == 'D')) // Force counter [number]
	{
		parse_force_pulse_counter(buf);
	}
	if ((*buf == 'h') || (*buf == 'H')) // set S/N as [number]
	{
		parse_serial(buf);
	}
	if ((*buf == 'i') || (*buf == 'I')) // Show S/N as [number] and Remaining
	{
		parse_serial_show(buf);
	}
	else if ((*buf == 'a') || (*buf == 'A')) // pulse present
	{
		parse_pulse(buf);
	} 
	else if ((*buf == 'p') || (*buf == 'P')) // pressure [number]
	{
		#if PASS_PRESSURE
				//parse_pressure(buf);
		#else
				parse_pressure(buf);
		#endif

	} 
	else if ((*buf == 't') || (*buf == 'T')) // parse applicator in size
	{
		parse_aplicator(buf);
	} 
	else if ((*buf == 'b') || (*buf == 'B')) // battery [number]
	{
		parse_battery_percent(buf);
	}
	else if ((*buf == 'e') || (*buf == 'E')) // error with number
	{
		parse_E(buf);
	}
	else if ((*buf == 'o') || (*buf == 'O')) // error with number
	{
		parse_O(buf);
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
	else if ((*buf == 'w') || (*buf == 'W'))
	{
		print_warning(buf);
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
	else if((*buf=='k') || (*buf == 'K'))
	{
	parse_fin_batt(buf); 
	}
	else if ((*buf == 'n') || (*buf == 'N'))
	{
		NVIC_SystemReset();
	}
	display.setCursor(0, 0);
}

void setup(void) {
#if PROMINI
	Serial.begin(115200);
#else
	Serial.begin(115200);
	Serial1.begin(115200);
#endif
	SPI.begin();
	display.begin();
	display.setRotation(3);
	pinMode(LED_BUILTIN, OUTPUT);
	u8g2_for_adafruit_gfx.begin(display);
	//u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR24_tf);
	 //u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR12_tf);
	u8g2_for_adafruit_gfx.setFont(u8g2_font_ncenR18_tf);
	//u8g2_for_adafruit_gfx.setFont(u8g2_font_8x13_tf);
	u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 none transparent mode
	u8g2_for_adafruit_gfx.setFontDirection(0);
	set_lang(LANG_ENG);
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

	display.fillScreen(ILI9341_WHITE);

	DRAW_WELCOME

	delay(2000);
#if DEBUG_STANDALONE
  
  
	//display.fillScreen(ILI9341_BLACK);
	//delay(2000);
	//display.fillScreen(Warning_YELLOW);// RGB888toRGB565("FFFF00"));
	//parse_fin_batt();
	//while(1);
	//DRAW_BATTERY
	//display.println("HELLO");
	//u8g2_for_adafruit_gfx.drawUTF8(0,20,"la oración");
	//utf8ascii(ENG[10]);
	// align_center_print(LANG[10], 30, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 1);
	//u8g2_for_adafruit_gfx.drawGlyph(5, 20,'Ä');
	//tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	//tft.println("DEBUG VERSION");
	//tft.println("Umlaut ÄÖÜ");
	// u8g2_for_adafruit_gfx.setCursor(0,20); 
	//  u8g2_for_adafruit_gfx.print("DEBUG VERSION"); 
	// u8g2_for_adafruit_gfx.setCursor(0,40); 
	// u8g2_for_adafruit_gfx.setCursor(0,20);
	//align_center_print(LANG[0], 30, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 5); 
	//u8g2_for_adafruit_gfx.print("Umlaut ÄÖÜ");
	//  u8g2_for_adafruit_gfx.setCursor(0,40); 
	// align_center_print(s1, 30, RGB888toRGB565("00B0F0"), RGB888toRGB565("FFFF00"), 5);
	//u8g2_for_adafruit_gfx.setCursor(0,60);  
	//u8g2_for_adafruit_gfx.print("la oración");
	//tft.println("la oración");



	//char str_serial_status[30];
	//char s1[40];
	//int i = 8;
	//int num=0;
	//for (i = 0; i < LANG_STR_NUM; i++)
	//{
	//	LANG[i] = LANG_FRA_ARR[i];
	//}
	//serial_number = 1920046;
	//remaining_pulses = 0;
	//while (1)
	//{
	//	if (i == 0)
	//	{
	//		parse_lang(" 0                                                                                            ");
	//		parse_lang(" 0                                                                                            ");
	//	}
	//	else if (i == 1)
	//	{
	//		parse_lang(" 1                                                                                            ");
	//		parse_lang(" 1                                                                                            ");
	//	}
	//	else if (i == 2)
	//	{
	//		parse_lang(" 2                                                                                            ");
	//		parse_lang(" 2                                                                                            ");
	//	}
	//	else if (i == 3)
	//	{
	//		parse_lang(" 3                                                                                            ");
	//		parse_lang(" 3                                                                                            ");
	//	}
	//	//i++;
	//	if (i > 3)
	//		i = 0;
	//	lang_set_done = false;
	//	delay(3000);
	//	
	//	parse_serial_show(" 1234567						");
	//	delay(3000);
	//	parse_E(" 4000									");
	//	delay(3000);
	//
	//	parse_serial_show(" 123456						");
	//	delay(3000);
	//	parse_E(" 4000									");
	//	delay(3000);
	//
	//	parse_serial_show(" 5000						");
	//	delay(3000);
	//	parse_E(" 4000									");
	//	delay(3000);
	//
	//	parse_serial_show(" 0							");
	//	delay(3000);
	//	parse_E(" 4000									");
	//	delay(3000);
	//
	//	parse_E(" 4001									");
	//	delay(3000);
	//	parse_E(" 4011									");
	//	delay(3000);
	//	parse_E(" 5000									");
	//	delay(3000);
	//	parse_E(" 5111									");
	//	delay(3000);
	//	parse_E(" 6111									");
	//	delay(3000);
	//	parse_E(" 503									");
	//	delay(3000);
	//	parse_E(" 504									");
	//	delay(3000);
	//	parse_fail(" 2000								");
	//	delay(3000);
	//	parse_fail(" 200								");
	//	delay(3000);
	//	parse_fail(" 0									");
	//	delay(3000);
	//}
	//while (1)
	//{
		//parse_E(" 503									");
		//delay(3000);
		//parse_E(" 504									");
		//delay(3000);
		//set_lang(LANG_FRA);
		//parse_O(" 503									");
		//delay(1500);
		//sprintf(s1, "B%d",i );
		// itoa(i,s1,10);
		//parse_battery_percent(s1);
		//delay(15000);
		//i=i+10;
		//set_lang(LANG_GER);
		//parse_O(" 503									");
		//delay(15000);
		//set_lang(LANG_ENG);
		//parse_O(" 503									");
		//delay(15000);
		//set_lang(LANG_SPA);
		//parse_O(" 503									");
		//delay(15000);
	//}
	//delay(1000);
	while (1)
	{
		remaining_pulses = 0;
		parse_E(" 4000									");
		delay(3000);
		remaining_pulses = 6000;
		parse_E(" 4000									");
		delay(3000);
		remaining_pulses = 8000;
		parse_E(" 4000									");
		delay(3000);
		parse_E(" 4001									");
		delay(3000);
		parse_E(" 4010									");
		delay(3000);
		parse_E(" 6999									");
		delay(3000);
		serial_number = 123123;
		parse_serial_show(" 8000									");
		delay(3000);
		parse_serial_show(" 6000									");
		delay(3000);
		parse_serial_show(" 0									");
		delay(3000);
		parse_syserror(" 300									");
		delay(3000);
		parse_fail(" 7000									");
		delay(3000);
		parse_fail(" 1000									");
		delay(3000);
		parse_fail(" 0									");
		delay(3000);
		parse_E(" 503									");
		delay(3000);
		parse_E(" 504									");
		delay(3000);
		parse_O(" 503									");
		delay(3000);
		parse_fin_batt(" 25									");
		delay(3000);
		parse_fin_batt(" 15									");
		delay(3000);
		parse_fin_batt(" 0									");
		delay(3000);
		parse_fin_batt(" 200									");
		delay(3000);
	}
#endif
/*
	paint_half_half();

	DRAW_PULSE_N
	DRAW_BATTERY
	DRAW_PRESSURE_LOW
	DRAW_AP_OK

	// am percentage
	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.setCursor(AM_TEXT_POS);
	tft.println("...");

#if !SIMPLE_BAT
	// battery percentage
	tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
	tft.setCursor(BATTERY_TEXT_POS);
	tft.println("...");
#endif

	blank_upper_side();
	serial_message.msg_buf = BufferString;
  */
	serial_message.start_message = false;
	serial_message.next_state = IDLE;
	serial_message.state = IDLE;
	uart_armenta_logo();
	watchdog_last_update = millis();
	// while get data till get AM + Remaining - stay at title card;
	// parse data in a while only 2 messages
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
#ifdef UART_TX
		Serial1.write(key);
#endif
#endif
		if (key > -1)
		{	
			switch (serial_message.state)
			{
			case IDLE:
				if(PASS_CRC)
				{
					if (key == '$') //head
					{
						i = 0;
						BufferString[0] = '$';
					}
					else if (key == '#') // ending tail
					{
						BufferString[i] = '#';
						BufferString[i + 1] = 0;
						PrintOnLcd(&BufferString[1]);
						watchdog_last_update = millis();
					}
					else
					{
						BufferString[i] = key;
						BufferString[i + 1] = 0;
					}
			        i++;
					serial_message.state = IDLE;
				}
				else if (key == 0x01)
				{
					serial_message.next_state = WAITING_LENGTH;
					serial_message.checksum = 0;
					serial_message.msg_length = 0;
					//Serial.print('A');
				}
				break;
			case WAITING_LENGTH:
				serial_message.msg_length = key;
				serial_message.next_state = BUILDING_BUFFER;
				i = 0;
				//Serial.print('B');
				break;
			case BUILDING_BUFFER:
				if (key == '$') //head
				{
					i = 0;
					BufferString[0] = '$';
				}
				else if (key == '#') // ending tail
				{
					BufferString[i] = '#';
					BufferString[i+1] = 0;
					if (BufferString[0] == '$' && (strlen(BufferString) == serial_message.msg_length))
					{
						serial_message.next_state = CHECKSUM;
						// DO NOT RUN COMMAND YET
						//PrintOnLcd(&BufferString[1]);
					}
					else
					{
						BufferString[0] = 0;
						serial_message.next_state = IDLE;
					}
					i = -1;
				}
				else
				{
					BufferString[i] = key;
					BufferString[i + 1] = 0;
				}
				//Serial.print('C');
				//Serial.print(i);
				i++;
				break;
			case CHECKSUM:
				if (i==0)
				{
					i++;
					serial_message.checksum = ((uint16_t)key) << 8;
				}
				else if (i==1)
				{
					serial_message.checksum = serial_message.checksum | key;
					uint16_t calc_CRC = calc_crc((uint8_t*)BufferString, serial_message.msg_length);
					if (calc_CRC == serial_message.checksum)
					{
						BufferString[serial_message.msg_length - 1] = 0;  // remove the #
						PrintOnLcd(&BufferString[1]);
					}
					else
					{
						// Error calculating CRC
						//Serial.println("E");
						//Serial.print(" .. calc ");
						//Serial.print(calc_CRC);
						//Serial.print(" .. buff ");
						//Serial.println(serial_message.checksum);
						//Serial.println(BufferString);
					}
					serial_message.next_state = IDLE;
					i++;
				}
				//Serial.print('D');
				//Serial.print(i);
				watchdog_last_update = millis();
				break;
			}
		serial_message.state = serial_message.next_state;
		}
	}
//	uint8_t x = display.readcommand8(ILI9341_RDMODE);
//	if (x != 0x94)
//	{
//		Serial.print("Power mode: ");
//		Serial.println(x);
//		display.begin();
//		display.setRotation(3);
//		reset_screen();
//	}
	if (millis() - watchdog_last_update > WATCHDOG_TIMER_EXPIRE && !DEBUG_STANDALONE)
	{
		if(!watchdog_wakeup)
		{
			watchdog_wakeup = true;
		}
		else
		{
			static unsigned long last_error_millis = millis();
			//static char timeout[] = "System Failure E300";
			static char timeout[] = "300";
			//static char timeout[] = "la oración";
			char* buf = &timeout[0];
			if (millis() - last_error_millis > WATCHDOG_TIMER_EXPIRE/2 )
			{
				Serial.println("mcu where are you?");

				parse_syserror(buf);

				//display.fillScreen(Warning_RED);
				//display.setFont();
				
				//display.setTextColor(ILI9341_WHITE);
				//display.setTextSize(6);
				//display.setCursor(75, 30);
				//display.println("Error");
				//display.setCursor(50, 90);
				//display.setTextSize(2);
				
				//display.println(timeout);
				last_error_millis = millis();
			}
		}
	}
	else
	{
		if((watchdog_wakeup)&&  (not_show_reset==0))
		{
			reset_screen();
			graphics_to_Screen(counter);
    }
			watchdog_wakeup = false;
			watchdog_last_update = millis();
		
	}
}
