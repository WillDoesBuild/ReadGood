/*********
  Complete project details at https://randomnerdtutorials.com
  
  This is an example for our Monochrome OLEDs based on SSD1306 drivers. Pick one up today in the adafruit shop! ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 pixel display using I2C to communicate 3 pins are required to interface (two I2C and one reset).
  Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. BSD license, check license.txt for more information All text above, and the splash screen below must be included in any redistribution. 
*********/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LittleFS.h>
#include <Adafruit_TinyUSB.h>

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

//usb drive stuff
Adafruit_USBD_MSC usb_msc;

// Define storage parameters
#define DISK_BLOCK_NUM 256                            // Number of blocks
#define DISK_BLOCK_SIZE 512                           // Block size in bytes
#define DISK_SIZE (DISK_BLOCK_NUM * DISK_BLOCK_SIZE)  // Total size

// Storage buffer
uint8_t msc_disk[DISK_SIZE];

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10  // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
static const unsigned char PROGMEM logo_bmp[] = { B00000000, B11000000,
                                                  B00000001, B11000000,
                                                  B00000001, B11000000,
                                                  B00000011, B11100000,
                                                  B11110011, B11100000,
                                                  B11111110, B11111000,
                                                  B01111110, B11111111,
                                                  B00110011, B10011111,
                                                  B00011111, B11111100,
                                                  B00001101, B01110000,
                                                  B00011011, B10100000,
                                                  B00111111, B11100000,
                                                  B00111111, B11110000,
                                                  B01111100, B11110000,
                                                  B01110000, B01110000,
                                                  B00000000, B00110000 };

vector<string> words;

vector<string> split_sentence(string sen) {

  // Create a stringstream object
  stringstream ss(sen);

  // Variable to hold each word
  string word;

  // Vector to store the words
  vector<string> words;

  // Extract words from the sentence
  while (ss >> word) {

    // Add the word to the vector
    words.push_back(word);
  }

  return words;
}

#define button1 1
#define button2 2


long int currentWord = 0;
long int lastChange = 0;
boolean scroll = LOW;
int scrollSpeed = 120;

bool successfulUpload = LOW;

void setup() {
  Serial.begin(115200);

  pinMode(button2, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);

  //usb drive stuff
  memset(msc_disk, 0, DISK_SIZE);

  // Create a simple FAT12 filesystem
  format_disk();

  // Set up MSC callbacks
  usb_msc.setID("Arduino", "RP2040 Storage", "1.0");
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  usb_msc.setCapacity(DISK_BLOCK_NUM, DISK_BLOCK_SIZE);
  usb_msc.setUnitReady(true);
  usb_msc.begin();

  Serial.println("RP2040 Mass Storage Device ready");
  Serial.println("Connect to computer to access files");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000);  // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(1000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  display.setTextSize(3);

  //  for (string w : words) {
  //    Serial.println(w.c_str());
  //  }

  display.clearDisplay();
  display.fillScreen(WHITE);
  display.setCursor(10, 20);
  display.setTextColor(BLACK);

  display.setTextSize(2);
  display.println("Upload");
  display.print("a file!");

  display.display();
  while (successfulUpload == LOW) {
    delay(100);
    getAndParseFile();
  }
  displayWord();
}

void loop() {
  if (millis() - lastChange >= scrollSpeed) {
    if ((digitalRead(button1)) == LOW && currentWord < (words.size() - 1)) {
      if (scroll == LOW) {
        currentWord++;
        displayWord();
        lastChange = millis();
      } else if (scrollSpeed > 70) {
        scrollSpeed = scrollSpeed - 10;
      }
    }
    if (scroll == HIGH && currentWord < (words.size() - 1)) {
      currentWord++;
      displayWord();
      lastChange = millis();
    }
    if (digitalRead(button2) == LOW && currentWord > 0) {
      if (scroll == LOW) {
        currentWord--;
        displayWord();
        lastChange = millis();
      } else if (scrollSpeed < 400 && currentWord < words.size() - 1) {
        scrollSpeed = scrollSpeed + 10;
      }
    }
  }
  if (digitalRead(button1) == LOW && digitalRead(button2) == LOW) {
    if (scroll == HIGH) {
      scroll = LOW;
    } else {
      scroll = HIGH;
    }
    delay(200);
  }
}

//word things!
void displayWord() {
  string selectedWord = getWord(words, currentWord);
  if (selectedWord.size() > 7) {
    display.setTextSize(2);
  } else {
    display.setTextSize(3);
  }

  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextColor(WHITE);
  display.print(selectedWord.c_str());

  display.display();

  //slow down if word is big
  if (selectedWord.size() > 6) {
    delay(scrollSpeed);
  }
}

string getWord(const vector<string>& words, int whichWord) {
  return words[whichWord];
}

//file extraction and parsing
// Add this function to parse files from USB mass storage
// Complete getAndParseFile() function with fixed filename parsing
void getAndParseFile() {
  // FAT12 filesystem layout:
  // Sector 0: Boot sector
  // Sector 1: FAT table 1
  // Sector 2: FAT table 2
  // Sector 3+: Root directory (16 entries, 32 bytes each = 512 bytes = 1 sector)
  // Sector 4+: Data area

  uint8_t* root_dir = msc_disk + (3 * 512);  // Root directory starts at sector 3

  // Look for files in root directory
  for (int i = 0; i < 16; i++) {
    uint8_t* entry = root_dir + (i * 32);

    // Check if entry is valid (first byte != 0 and != 0xE5)
    if (entry[0] != 0 && entry[0] != 0xE5) {
      // Check if it's a file (not directory)
      if ((entry[11] & 0x10) == 0) {
        // Get filename - FAT12 stores as 8.3 format with space padding
        char filename[13];

        // Copy name part (8 bytes) and remove trailing spaces
        int name_len = 8;
        memcpy(filename, entry, 8);
        while (name_len > 0 && filename[name_len - 1] == ' ') {
          name_len--;
        }
        filename[name_len] = 0;

        // Check if there's an extension (3 bytes starting at offset 8)
        bool has_extension = false;
        for (int k = 8; k < 11; k++) {
          if (entry[k] != ' ') {
            has_extension = true;
            break;
          }
        }

        // Add extension if it exists
        if (has_extension) {
          filename[name_len] = '.';
          int ext_len = 0;
          for (int k = 8; k < 11; k++) {
            if (entry[k] != ' ') {
              filename[name_len + 1 + ext_len] = entry[k];
              ext_len++;
            }
          }
          filename[name_len + 1 + ext_len] = 0;
        }

        // Get file size
        uint32_t file_size = entry[28] | (entry[29] << 8) | (entry[30] << 16) | (entry[31] << 24);

        // Get starting cluster
        uint16_t start_cluster = entry[26] | (entry[27] << 8);

        Serial.print("Found file: ");
        Serial.print(filename);
        Serial.print(" Size: ");
        Serial.println(file_size);

        // Check if this is a text file (case insensitive)
        String filenameStr = String(filename);
        filenameStr.toUpperCase();
        if (filenameStr.endsWith(".TXT")) {
          // Read file content
          if (file_size > 0 && start_cluster >= 2) {
            // Calculate data sector (cluster 2 = sector 4)
            uint32_t data_sector = 4 + (start_cluster - 2);
            uint8_t* file_data = msc_disk + (data_sector * 512);

            // Create string from file data
            String fileContent;
            for (uint32_t j = 0; j < file_size && j < 512; j++) {
              if (file_data[j] == 0) break;
              fileContent += (char)file_data[j];
            }

            Serial.println("File content:");
            Serial.println(fileContent);

            // Parse the file content
            std::string sen = fileContent.c_str();
            words = split_sentence(sen);

            Serial.println("File parsed successfully");
            successfulUpload = HIGH;
            return;
          }
          Serial.println("File is txt but couldn't read content");
        }
      }
    }
  }

  Serial.println("No text file found in USB storage");
}

//usb drive callbacks
// Callback to read data
int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize) {
  // Check if LBA is valid
  if (lba >= DISK_BLOCK_NUM) return -1;

  // Copy data from storage to buffer
  memcpy(buffer, msc_disk + lba * DISK_BLOCK_SIZE, bufsize);
  return bufsize;
}

// Callback to write data
int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
  // Check if LBA is valid
  if (lba >= DISK_BLOCK_NUM) return -1;

  // Copy data from buffer to storage
  memcpy(msc_disk + lba * DISK_BLOCK_SIZE, buffer, bufsize);
  return bufsize;
}

// Callback to flush data
void msc_flush_cb(void) {
  // Flush any pending writes (not needed for RAM storage)
}

// Simple FAT12 filesystem creation
void format_disk() {
  // Create minimal FAT12 boot sector
  uint8_t boot_sector[512] = { 0 };

  // FAT12 boot sector header
  boot_sector[0] = 0xEB;  // Jump instruction
  boot_sector[1] = 0x3C;
  boot_sector[2] = 0x90;

  // OEM name
  memcpy(&boot_sector[3], "ARDUINO ", 8);

  // Bytes per sector
  boot_sector[11] = 0x00;
  boot_sector[12] = 0x02;  // 512 bytes

  // Sectors per cluster
  boot_sector[13] = 0x01;  // 1 sector per cluster

  // Reserved sectors
  boot_sector[14] = 0x01;
  boot_sector[15] = 0x00;  // 1 reserved sector

  // Number of FATs
  boot_sector[16] = 0x02;  // 2 FATs

  // Root directory entries
  boot_sector[17] = 0x10;
  boot_sector[18] = 0x00;  // 16 entries

  // Total sectors
  boot_sector[19] = (DISK_BLOCK_NUM)&0xFF;
  boot_sector[20] = (DISK_BLOCK_NUM >> 8) & 0xFF;

  // Media descriptor
  boot_sector[21] = 0xF8;

  // Sectors per FAT
  boot_sector[22] = 0x01;
  boot_sector[23] = 0x00;  // 1 sector per FAT

  // Boot signature
  boot_sector[510] = 0x55;
  boot_sector[511] = 0xAA;

  // Copy boot sector to disk
  memcpy(msc_disk, boot_sector, 512);

  // Initialize FAT tables
  msc_disk[512] = 0xF8;  // Media descriptor in FAT
  msc_disk[513] = 0xFF;
  msc_disk[514] = 0xFF;

  // Second FAT copy
  msc_disk[1024] = 0xF8;
  msc_disk[1025] = 0xFF;
  msc_disk[1026] = 0xFF;
}



/* void testdrawline() {
  int16_t i;

  display.clearDisplay();  // Clear display buffer

  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, WHITE);
    display.display();  // Update screen with each newly-drawn line
    delay(1);
  }
  for (i = 0; i < display.height(); i += 4) {
    display.drawLine(0, 0, display.width() - 1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(0, display.height() - 1, i, 0, WHITE);
    display.display();
    delay(1);
  }
  for (i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for (i = display.width() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, WHITE);
    display.display();
    delay(1);
  }
  for (i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for (i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i, WHITE);
    display.display();
    delay(1);
  }
  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, WHITE);
    display.display();
    delay(1);
  }

  delay(2000);  // Pause for 2 seconds
}

void testdrawrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, WHITE);
    display.display();  // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testfillrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, INVERSE);
    display.display();  // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testdrawcircle(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillcircle(void) {
  display.clearDisplay();

  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, INVERSE);
    display.display();  // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}

void testdrawroundrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                          display.height() / 4, WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillroundrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                          display.height() / 4, INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawtriangle(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 5) {
    display.drawTriangle(
      display.width() / 2, display.height() / 2 - i,
      display.width() / 2 - i, display.height() / 2 + i,
      display.width() / 2 + i, display.height() / 2 + i, WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfilltriangle(void) {
  display.clearDisplay();

  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width() / 2, display.height() / 2 - i,
      display.width() / 2 - i, display.height() / 2 + i,
      display.width() / 2 + i, display.height() / 2 + i, INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.setCursor(0, 0);      // Start at top-left corner
  display.cp437(true);          // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for (int16_t i = 0; i < 256; i++) {
    if (i == '\n') display.write(' ');
    else display.write(i);
  }

  display.display();
  delay(2000);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.setCursor(0, 0);      // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(BLACK, WHITE);  // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);  // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.print(F("0x"));
  display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2);  // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();  // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width() - LOGO_WIDTH) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS 0  // Indexes into the 'icons' array in function below
#define YPOS 1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for (f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS] = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for (;;) {                 // Loop forever...
    display.clearDisplay();  // Clear the display buffer

    // Draw each snowflake:
    for (f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }

    display.display();  // Show the display buffer on the screen
    delay(200);         // Pause for 1/10 second

    // Then update coordinates of each flake...
    for (f = 0; f < NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS] = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}
*/