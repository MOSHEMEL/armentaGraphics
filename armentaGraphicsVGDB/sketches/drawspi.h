#pragma once
#define DRAW_WELCOME bmpDraw("/icon/w.bmp", 0, 0); 

#define DRAW_PULSE_N bmpDraw("/icon/pulseNo.bmp", 90, 168);
#define DRAW_PULSE_Y bmpDraw("/icon/pulseYes.bmp", 90, 168);

#define DRAW_BATTERY bmpDraw("/icon/bat_1.bmp", 160, 168);

#define DRAW_PRESSURE_LOW bmpDraw("/icon/pr1l.bmp", 10, 168);
#define DRAW_PRESSURE_HIGH bmpDraw("/icon/pr1h.bmp", 10, 168);

#define DRAW_AP_OK 	bmpDraw("/icon/ap_clean.bmp", 230, 168);
#define DRAW_AP_ERROR bmpDraw("/icon/ap_red.bmp", 230, 168);


extern Adafruit_ILI9341 tft;

uint16_t read16(File& f);
uint32_t read32(File& f);
void bmpDraw(char* filename, int16_t x, int16_t y);





// IN RGB565 // and rgb888 IS 0x00ACB2 < 0, 172, 178
// *****************************************************************************************
uint16_t read16(File& f) {
	uint16_t result;
	((uint8_t*)&result)[0] = f.read(); // LSB
	((uint8_t*)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t read32(File& f) {
	uint32_t result;
	((uint8_t*)&result)[0] = f.read(); // LSB
	((uint8_t*)&result)[1] = f.read();
	((uint8_t*)&result)[2] = f.read();
	((uint8_t*)&result)[3] = f.read(); // MSB
	return result;
}

void bmpDraw(char* filename, int16_t x, int16_t y) {

	File     bmpFile;
	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip = true;        // BMP is stored bottom-to-top
	int      w, h, row, col, x2, y2, bx1, by1;
	uint8_t  r, g, b;
	uint32_t pos = 0, startTime = millis();

	if ((x >= tft.width()) || (y >= tft.height())) return;



	Serial.println();
	Serial.print(F("Loading image '"));
	Serial.print(filename);
	Serial.println('\'');

	// Open requested file on SD card
	if ((bmpFile = SD.open(filename)) == NULL) {
		Serial.print(F("File not found"));
		return;
	}

	// Parse BMP header
	if (read16(bmpFile) == 0x4D42) { // BMP signature
		Serial.print(F("File size: ")); Serial.println(read32(bmpFile));

		(void)read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
		// Read DIB header
		Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));

		bmpWidth = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if (read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
			if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!
				Serial.print(F("Image size: "));
				Serial.print(bmpWidth);
				Serial.print('x');
				Serial.println(bmpHeight);

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if (bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip = false;
				}

				// Crop area to be loaded
				x2 = x + bmpWidth - 1; // Lower-right corner
				y2 = y + bmpHeight - 1;
				if ((x2 >= 0) && (y2 >= 0)) { // On screen?
					w = bmpWidth; // Width/height of section to load/display
					h = bmpHeight;
					bx1 = by1 = 0; // UL coordinate in BMP file
					if (x < 0) { // Clip left
						bx1 = -x;
						x = 0;
						w = x2 + 1;
					}
					if (y < 0) { // Clip top
						by1 = -y;
						y = 0;
						h = y2 + 1;
					}
					if (x2 >= tft.width())  w = tft.width() - x; // Clip right
					if (y2 >= tft.height()) h = tft.height() - y; // Clip bottom

					// Set TFT address window to clipped image bounds
					tft.startWrite(); // Requires start/end transaction now
					tft.setAddrWindow(x, y, w, h);

					for (row = 0; row < h; row++) { // For each scanline...

					  // Seek to start of scan line.  It might seem labor-
					  // intensive to be doing this on every line, but this
					  // method covers a lot of gritty details like cropping
					  // and scanline padding.  Also, the seek only takes
					  // place if the file position actually needs to change
					  // (avoids a lot of cluster math in SD library).
						if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
							pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
						else     // Bitmap is stored top-to-bottom
							pos = bmpImageoffset + (row + by1) * rowSize;
						pos += bx1 * 3; // Factor in starting column (bx1)
						if (bmpFile.position() != pos) { // Need seek?
							tft.endWrite(); // End TFT transaction
							bmpFile.seek(pos);
							buffidx = sizeof(sdbuffer); // Force buffer reload
							tft.startWrite(); // Start new TFT transaction
						}
						for (col = 0; col < w; col++) { // For each pixel...
						  // Time to read more pixel data?
							if (buffidx >= sizeof(sdbuffer)) { // Indeed
								tft.endWrite(); // End TFT transaction
								bmpFile.read(sdbuffer, sizeof(sdbuffer));
								buffidx = 0; // Set index to beginning
								tft.startWrite(); // Start new TFT transaction
							}
							// Convert pixel from BMP to TFT format, push to display
							b = sdbuffer[buffidx++];
							g = sdbuffer[buffidx++];
							r = sdbuffer[buffidx++];
							//tft.writePixel(x, y, tft.color565(r, g, b));
							tft.pushColor(tft.color565(r, g, b));
						} // end pixel
					} // end scanline
					tft.endWrite(); // End last TFT transaction
				} // end onscreen
				Serial.print(F("Loaded in "));
				Serial.print(millis() - startTime);
				Serial.println(" ms");
			} // end goodBmp
		}
	}

	bmpFile.close();
	if (!goodBmp) Serial.println(F("BMP format not recognized."));
}