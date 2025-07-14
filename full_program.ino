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

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

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

void setup() {
  Serial.begin(115200);

  pinMode(button2, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);

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

  display.setTextSize(8);

  //word stuff!
  const string sen = "Hack When I wake up, the other side of the bed is cold. My fingers stretch out, seeking Prims warmth but finding only the rough canvas cover of the mattress. She must have had bad dreams and climbed in with our mother. Of course, she did. This is the day of the reaping. I prop myself up on one elbow. Theres enough light in the bedroom to see them. My little sister, Prim, curled up on her side, cocooned in my mothers body, their cheeks pressed together. In sleep, my mother looks younger, still worn but not so beaten-down. Prims face is as fresh as a raindrop, as lovely as the primrose for which she was named. My mother was very beautiful once, too. Or so they tell me. Sitting at Prims knees, guarding her, is the worlds ugliest cat. Mashed-in nose, half of one ear missing, eyes the color of rotting squash. Prim named him Buttercup, insisting that his muddy yellow coat matched the bright flower. He hates me. Or at least distrusts me. Even though it was years ago, I think he still remembers how I tried to drown him in a bucket when Prim brought him home. Scrawny kitten, belly swollen with worms, crawling with fleas. The last thing I needed was another mouth to feed. But Prim begged so hard, cried even, I had to let him stay. It turned out okay. My mother got rid of the vermin and hes a born mouser. Even catches the occasional rat. Sometimes, when I clean a kill, I feed Buttercup the entrails. He has stopped hissing at me. Entrails. No hissing. This is the closest we will ever come to love. I swing my legs off the bed and slide into my hunting boots. Supple leather that has molded to my feet. I pull on trousers, a shirt, tuck my long dark braid up into a cap, and grab my forage bag. On the table, under a wooden bowl to protect it from hungry rats and cats alike, sits a perfect little goat cheese wrapped in basil leaves. Prims gift to me on reaping day. I put the cheese carefully in my pocket as I slip outside. Our part of District 12, nicknamed the Seam, is usually crawling with coal miners heading out to the morning shift at this hour. Men and women with hunched shoulders, swollen knuckles, many who have long since stopped trying to scrub the coal dust out of their broken nails, the lines of their sunken faces. But today the black cinder streets are empty. Shutters on the squat gray houses are closed. The reaping isnt until two. May as well sleep in. If you can. Our house is almost at the edge of the Seam. I only have to pass a few gates to reach the scruffy field called the Meadow. Separating the Meadow from the woods, in fact enclosing all of District 12, is a high chain-link fence topped with barbed-wire loops. In theory, its supposed to be electrified twenty-four hours a day as a deterrent to the predators that live in the woods packs of wild dogs, lone cougars, bears that used to threaten our streets. But since were lucky to get two or three hours of electricity in the evenings, its usually safe to touch. Even so, I always take a moment to listen carefully for the hum that means the fence is live. Right now, its silent as a stone. Concealed by a clump of bushes, I flatten out on my belly and slide under a two-foot stretch thats been loose for years. There are several other weak spots in the fence, but this one is so close to home I almost always enter the woods here. As soon as Im in the trees, I retrieve a bow and sheath of arrows from a hollow log. Electrified or not, the fence has been successful at keeping the flesh-eaters out of District 12. Inside the woods they roam freely, and there are added concerns like venomous snakes, rabid animals, and no real paths to follow. But theres also food if you know how to find it. My father knew and he taught me some before he was blown to bits in a mine explosion. There was nothing even to bury. I was eleven then. Five years later, I still wake up screaming for him to run. Even though trespassing in the woods is illegal and poaching carries the severest of penalties, more people would risk it if they had weapons. But most are not bold enough to venture out with just a knife. My bow is a rarity, crafted by my father along with a few others that I keep well hidden in the woods, carefully wrapped in waterproof covers.";
  Serial.println(sen.max_size());
  // Call the function to split the sentence
  words = split_sentence(sen);

  //  for (string w : words) {
  //    Serial.println(w.c_str());
  //  }

  display.clearDisplay();
  display.fillScreen(WHITE);
  display.setCursor(30, 20);
  display.setTextColor(BLACK);

  string selectedWord = getWord(words, currentWord);
  if (selectedWord.size() >= 7) {
    display.setTextSize(2);
  }

display.setTextSize(4);
  display.print(selectedWord.c_str());

  display.display();
  delay(100);
}

string getWord(const vector<string>& words, int whichWord) {
  return words[whichWord];
}

void loop() {
  if (millis() - lastChange >= scrollSpeed) {
    if ((digitalRead(button1)) == LOW && currentWord < (words.size() - 1)) {
      if (scroll == LOW) {
        currentWord++;
        displayWord();
        lastChange = millis();
      } else if (scrollSpeed > 100) {
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
      } else if (scrollSpeed < 500) {
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
    delay(100);
  }
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