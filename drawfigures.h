#pragma once
#include "aplicator_error.h"
#include "aplicator_ok.h"
#include "battery.h"
#include "pressure_high.h"
#include "pressure_low.h"
#include "pulseN.h"
#include "pulseY.h"
#include "w_cropped.h"
#define DRAW_WELCOME tft.drawRGBBitmap(75, 50, (uint16_t*)(w_exported.pixel_data), w_exported.width, w_exported.height);

#define DRAW_PULSE_N tft.drawRGBBitmap(90, 168, (uint16_t*)(pulseN.pixel_data), pulseN.width, pulseN.height);
#define DRAW_PULSE_Y tft.drawRGBBitmap(90, 168, (uint16_t*)(pulseY.pixel_data), pulseY.width, pulseY.height);

#define DRAW_BATTERY tft.drawRGBBitmap(160, 168, (uint16_t*)(battery.pixel_data), battery.width, battery.height);

#define DRAW_PRESSURE_LOW delay(1); //tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_low.pixel_data), pressure_low.width, pressure_low.height);
#define DRAW_PRESSURE_HIGH delay(1); //tft.drawRGBBitmap(10, 168, (uint16_t*)(pressure_high.pixel_data), pressure_high.width, pressure_high.height);

#define DRAW_AP_OK 	tft.drawRGBBitmap(230, 168, (uint16_t*)(aplicator_ok.pixel_data), aplicator_ok.width, aplicator_ok.height);
#define DRAW_AP_ERROR tft.drawRGBBitmap(230, 168, (uint16_t*)(aplicator_error.pixel_data), aplicator_error.width, aplicator_error.height);
