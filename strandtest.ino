#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN 1   // Pin for LED rings and cut up strips on the top of the controller
#define LED_PIN_2 3 // Pin for 60-LED strip that's around the outer edge of the controller

#define LED_COUNT 96+119 // Rings (48 + 48) and strips (119) are all connected as one big strip
#define LED_COUNT_2 60   // 60-LED strip around the outer edge of the controller


// Seeduino Xiao has 3 built-in LEDs I want to try turning off to save a bit of power when running off battery
#define PIN_LED_13  (13u)
#define PIN_LED     PIN_LED_13
#define LED_BUILTIN PIN_LED

#define PIN_LED_RXL          (12u)
#define PIN_LED_TXL          (11u)
#define PIN_LED2             PIN_LED_RXL
#define PIN_LED3             PIN_LED_TXL
// End of Seeduino Xiao built-in LED declarations - these are from the seeeduino docs

// Declare our NeoPixel strip objects
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_2(LED_COUNT_2, LED_PIN_2, NEO_GRB + NEO_KHZ800);

// Global script variables
const uint8_t buflen = 24; // Number of frequency bins we are using, has to match what the C# program is sending over serial or it gets jaaaanky
char buf[buflen]; // byte array to hold the serial data as it comes in
char emptyArr[buflen] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Empty array for 'quick' comparison with our input buffer from serial (I feel like there should be a much easier way to do this with bitwise operators or something?)
uint16_t waitHue = 0; // Hue of "first pixel" to call rainbow function that we can increment while waiting for the serial connection
uint8_t waitBrightness = 0; // Used to fade in the waiting functions if wanted, but it doesn't look very great due to the ws2812b pixels not being sensitive enough for a smooth transition
uint8_t emptyCount = 0; // # of times we've gotten empty data from serial
uint16_t noSerialCount = 0; // # of times in a row there hasn't been serial data available - the loop goes through hundreds of times while waiting
uint8_t moduloNum = 108; // Starting modulo number for the modulo animation thing
uint8_t waitingNum = 0;
bool moduloRising = false; // Used to reverse the direction of the modulo animation thing
bool rainbowFalling = false; // Used for the rainbow fades to fade in and out although it also doesn't look great due to the pixels not being sensitive enough 


// setup() function -- runs once at startup --------------------------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Built-in LED I want to turn off to try saving a bit of power
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  strip.begin(); // Start the strip
  strip.show(); // Show the strip - this probably isn't needed here?
  strip.setBrightness(20); // Set BRIGHTNESS to about 10% (max = 255) - pixels are more sensitive with more brightness but also more blinding
  strip_2.begin();
  strip_2.show();
  strip_2.setBrightness(75); // About 30% brightness, they mainly just shine out on the table/ground that the controller is sitting on so a bit brighter is better
  Serial.begin(115200); // Start serial with baud rate of 115200 bits per second
  while (!Serial) { // Wait for the serial connection to be started from the C# program - show the adafruit rainbow animation while waiting
    waitingRainbow(512);
  }
}


// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {
  if (Serial.available() >= buflen) { // When the C# program sends the buffer
    noSerialCount = 0; // When we get serial data reset this
    int rlen = Serial.readBytes(buf, buflen); // Read the buffer from the C# program
    digitalWrite(PIN_LED2, HIGH); // High = off, basically just keep the serial LEDs from turning on
    digitalWrite(PIN_LED3, HIGH);
    if (memcmp(buf, emptyArr, buflen) == 0) { // Check if the serial data is empty (if the buf == the array of 0s)
      if (emptyCount > 20) { // Once we've hit our threshold of empty data, show our animations (20 = like 1 second)
        // waitingRainbow(1024);
        // moduloTest();
        waitingFives();
        // rainbowFades(128);
      }
      else {
        if (emptyCount == 2) { // In case any pixels got stuck, like if music was paused - wait a few frames then clear
          strip.clear();
          strip_2.clear();
          strip.show();
          strip_2.show();
        }
        emptyCount++;
      }
    }
    else {
      emptyCount = 0;
      waitBrightness = 0;
      eqVisualize();
      // waitingFives();
    }
  }
  else {
    if (noSerialCount > 40000) { // If serial connection is closed, show rainbow - apparently this loops goes through A LOT of iterations in between empty serial buffers lmao (probably due to loopback audio limitations in Windows)
      waitingRainbow(128);       // 40,000 is like 1/2 of a second
    }
    else {
      noSerialCount++; // If we loop through and no serial data is available
    }
  }
}


// Adafruit rainbow animation on both strips that can be called while waiting
void waitingRainbow(uint16_t change) {
  if (waitBrightness < 150) {
    waitBrightness += 10;
  }
  strip.rainbow(waitHue, 5, 255, waitBrightness, true);
  strip.show();
  strip_2.rainbow(waitHue, 1, 255, waitBrightness, true);
  strip_2.show();
  waitHue -= change;
}

// Clunky modified Adafruit rainbow animation that cycles through the colors (with all pixels the same color) and fades brightness in and out
void rainbowFades(uint16_t change) {
  if (rainbowFalling) {
    waitBrightness -= 17;
  }
  else {
    waitBrightness += 17;
  }
  uint16_t hue = waitHue + 65536 / 215;
  uint32_t color = Adafruit_NeoPixel::ColorHSV(hue, 255, waitBrightness);
  color = Adafruit_NeoPixel::gamma32(color);
  strip.fill(color);
  strip.show();
  if (waitBrightness >= 240) {
    rainbowFalling = true;
  }
  else if (waitBrightness <= 15) {
    rainbowFalling = false;
  }
  waitHue += change;
}

// Modulo test that has little rainbow pixels run around, starting with 1, then 2, then more added as modulo decreases until board is full and explodes back out
// Honestly kinda fun to mess with but pretty annoying and distracting to use as a waiting animation
void moduloTest() {
  if (moduloNum == 1) {
    moduloRising = true;
  }
  else if (moduloNum == 109) {
    moduloRising = false;
  }
  strip.fill((0, 0, 0));
  for (int i = 1; i <= 215; i++) {
    if (i % moduloNum == 0) {
      strip.setPixelColor(i-1, 255, 0, 0); // Main lead pixel
      if (i < 211) {
      strip.setPixelColor(i, 255, 40, 0);  // Follor pixels that make the rainbow (kinda)
      strip.setPixelColor(i+1, 255, 140, 0);
      strip.setPixelColor(i+2, 0, 255, 0);
      strip.setPixelColor(i+3, 0, 0, 255);
      strip.setPixelColor(i+4, 70, 0, 255);
      }
    }
  }
  strip.show();
  delay(40);
  if (moduloRising) {
    moduloNum++;
  }
  else {
    moduloNum--;
  }
}

void waitingFives() {
  if (waitingNum > LED_COUNT - 1) {
    waitingNum = 0;
  }

  strip.clear();
  strip.setPixelColor(waitingNum, 255, 60, 0);
  strip.show();

  waitingNum++;
}


// Maps the 24 bins to the two 48-LED rings, 119 pixels of strip pieces, and 60-LED outer ring
// Mapping is tedious and can be time consuming but it's also a lot of fun to make different patterns
void eqVisualize() {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

  for (int i = 0; i < buflen; i++) {
    float v = buf[i] / 255.0;

    if (i == 0) {
      r = (uint8_t)(v * 255.0);
      g = 0;
      b = 0;
      strip.setPixelColor(36, r, g, b);         // Right ring
      strip.setPixelColor(95 - 35, r, g, b);    // Left ring
      
      strip.setPixelColor(95 + 1, r, g, b);     // Top left strip on left side
      strip.setPixelColor(95 + 2, r, g, b);
      strip.setPixelColor(95 + 47, r, g, b);    // Top right of strip on left side
      strip.setPixelColor(95 + 48, r, g, b);
      strip.setPixelColor(95 + 72, r, g, b);    // Top left strip on right side
      strip.setPixelColor(95 + 73, r, g, b);
      strip.setPixelColor(95 + 118, r, g, b);   // Top right strip on right side
      strip.setPixelColor(95 + 119, r, g, b);
      
      strip.setPixelColor(95 + 49, r, g, b);    // Left strip of top middle
      strip.setPixelColor(95 + 71, r, g, b);    // Right strip of top middle
      
      strip_2.setPixelColor(29, r, g, b);       // Outer ring
      strip_2.setPixelColor(30, r, g, b);
    }
    else if (i == 1) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 60.0);
      b = 0;
      strip.setPixelColor(35, r, g, b);
      strip.setPixelColor(37, r, g, b);
      strip.setPixelColor(95 - 34, r, g, b);
      strip.setPixelColor(95 - 36, r, g, b);
      
      strip.setPixelColor(95 + 3, r, g, b);
      strip.setPixelColor(95 + 46, r, g, b);
      strip.setPixelColor(95 + 74, r, g, b);
      strip.setPixelColor(95 + 117, r, g, b);
      
      strip.setPixelColor(95 + 50, r, g, b);
      strip.setPixelColor(95 + 70, r, g, b);
      
      strip_2.setPixelColor(28, r, g, b);
      strip_2.setPixelColor(31, r, g, b);
    }
    else if (i == 2) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 90.0);
      b = 0;
      strip.setPixelColor(34, r, g, b);
      strip.setPixelColor(38, r, g, b);
      strip.setPixelColor(95 - 33, r, g, b);
      strip.setPixelColor(95 - 37, r, g, b);
      
      strip.setPixelColor(95 + 4, r, g, b);
      strip.setPixelColor(95 + 45, r, g, b);
      strip.setPixelColor(95 + 75, r, g, b);
      strip.setPixelColor(95 + 116, r, g, b);
      
      strip.setPixelColor(95 + 51, r, g, b);
      strip.setPixelColor(95 + 69, r, g, b);
      
      strip_2.setPixelColor(27, r, g, b);
      strip_2.setPixelColor(32, r, g, b);
    }
    else if (i == 3) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 130.0);
      b = 0;
      strip.setPixelColor(33, r, g, b);
      strip.setPixelColor(39, r, g, b);
      strip.setPixelColor(95 - 32, r, g, b);
      strip.setPixelColor(95 - 38, r, g, b);
      
      strip.setPixelColor(95 + 5, r, g, b);
      strip.setPixelColor(95 + 44, r, g, b);
      strip.setPixelColor(95 + 76, r, g, b);
      strip.setPixelColor(95 + 115, r, g, b);
      
      strip.setPixelColor(95 + 52, r, g, b);
      strip.setPixelColor(95 + 68, r, g, b);
      
      strip_2.setPixelColor(26, r, g, b);
      strip_2.setPixelColor(33, r, g, b);
    }
    else if (i == 4) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 170.0);
      b = 0;
      strip.setPixelColor(32, r, g, b);
      strip.setPixelColor(40, r, g, b);
      strip.setPixelColor(95 - 31, r, g, b);
      strip.setPixelColor(95 - 39, r, g, b);
      
      strip.setPixelColor(95 + 6, r, g, b);
      strip.setPixelColor(95 + 43, r, g, b);
      strip.setPixelColor(95 + 77, r, g, b);
      strip.setPixelColor(95 + 114, r, g, b);
      
      strip.setPixelColor(95 + 53, r, g, b);
      strip.setPixelColor(95 + 67, r, g, b);
      
      strip_2.setPixelColor(25, r, g, b);
      strip_2.setPixelColor(34, r, g, b);
    }
    else if (i == 5) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 190.0);
      b = 0;
      strip.setPixelColor(31, r, g, b);
      strip.setPixelColor(41, r, g, b);
      strip.setPixelColor(95 - 30, r, g, b);
      strip.setPixelColor(95 - 40, r, g, b);
      
      strip.setPixelColor(95 + 7, r, g, b);
      strip.setPixelColor(95 + 42, r, g, b);
      strip.setPixelColor(95 + 78, r, g, b);
      strip.setPixelColor(95 + 113, r, g, b);
      
      strip.setPixelColor(95 + 54, r, g, b);
      strip.setPixelColor(95 + 66, r, g, b);
      
      strip_2.setPixelColor(24, r, g, b);
      strip_2.setPixelColor(35, r, g, b);
    }
    else if (i == 6) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 220.0);
      b = 0;
      strip.setPixelColor(30, r, g, b);
      strip.setPixelColor(42, r, g, b);
      strip.setPixelColor(95 - 29, r, g, b);
      strip.setPixelColor(95 - 41, r, g, b);
      
      strip.setPixelColor(95 + 8, r, g, b);
      strip.setPixelColor(95 + 9, r, g, b);
      strip.setPixelColor(95 + 40, r, g, b);
      strip.setPixelColor(95 + 41, r, g, b);
      strip.setPixelColor(95 + 79, r, g, b);
      strip.setPixelColor(95 + 80, r, g, b);
      strip.setPixelColor(95 + 111, r, g, b);
      strip.setPixelColor(95 + 112, r, g, b);
      
      strip.setPixelColor(95 + 55, r, g, b);
      strip.setPixelColor(95 + 65, r, g, b);
      
      strip_2.setPixelColor(23, r, g, b);
      strip_2.setPixelColor(36, r, g, b);
    }
    else if (i == 7) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 255.0);
      b = 0;
      strip.setPixelColor(29, r, g, b);
      strip.setPixelColor(43, r, g, b);
      strip.setPixelColor(95 - 28, r, g, b);
      strip.setPixelColor(95 - 42, r, g, b);
      
      strip.setPixelColor(95 + 56, r, g, b);
      strip.setPixelColor(95 + 64, r, g, b);
      
      strip_2.setPixelColor(22, r, g, b);
      strip_2.setPixelColor(37, r, g, b);
    }
    else if (i == 8) {
      r = 0;
      g = (uint8_t)(v * 255.0);
      b = 0;
      strip.setPixelColor(28, r, g, b);
      strip.setPixelColor(44, r, g, b);
      strip.setPixelColor(95 - 27, r, g, b);
      strip.setPixelColor(95 - 43, r, g, b);
      
      strip.setPixelColor(95 + 57, r, g, b);
      strip.setPixelColor(95 + 63, r, g, b);
      
      strip_2.setPixelColor(21, r, g, b);
      strip_2.setPixelColor(38, r, g, b);
    }
    else if (i == 9) {
      r = 0;
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 40.0);
      strip.setPixelColor(27, r, g, b);
      strip.setPixelColor(45, r, g, b);
      strip.setPixelColor(95 - 26, r, g, b);
      strip.setPixelColor(95 - 44, r, g, b);
      
      strip.setPixelColor(95 + 58, r, g, b);
      strip.setPixelColor(95 + 62, r, g, b);
      
      strip_2.setPixelColor(20, r, g, b);
      strip_2.setPixelColor(39, r, g, b);
    }
    else if (i == 10) {
      r = 0;
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 80.0);
      strip.setPixelColor(26, r, g, b);
      strip.setPixelColor(46, r, g, b);
      strip.setPixelColor(95 - 25, r, g, b);
      strip.setPixelColor(95 - 45, r, g, b);
      
      strip.setPixelColor(95 + 59, r, g, b);
      strip.setPixelColor(95 + 61, r, g, b);
      
      strip_2.setPixelColor(19, r, g, b);
      strip_2.setPixelColor(40, r, g, b);
    }
    else if (i == 11) {
      r = 0;
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 120.0);
      strip.setPixelColor(25, r, g, b);
      strip.setPixelColor(47, r, g, b);
      strip.setPixelColor(95 - 24, r, g, b);
      strip.setPixelColor(95 - 46, r, g, b);
      
      strip.setPixelColor(95 + 60, r, g, b);
      
      strip_2.setPixelColor(18, r, g, b);
      strip_2.setPixelColor(41, r, g, b);
    }
    else if (i == 12) {
      r = 0;
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 160.0);
      strip.setPixelColor(24, r, g, b);
      strip.setPixelColor(0, r, g, b);
      strip.setPixelColor(95 - 23, r, g, b);
      strip.setPixelColor(95 - 47, r, g, b);
      
      strip_2.setPixelColor(17, r, g, b);
      strip_2.setPixelColor(42, r, g, b);
    }
    else if (i == 13) {
      r = 0;
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 195.0);
      strip.setPixelColor(23, r, g, b);
      strip.setPixelColor(1, r, g, b);
      strip.setPixelColor(95 - 22, r, g, b);
      strip.setPixelColor(95 - 0, r, g, b);
      
      strip_2.setPixelColor(16, r, g, b);
      strip_2.setPixelColor(43, r, g, b);
    }
    else if (i == 14) {
      r = 0;
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 225.0);
      strip.setPixelColor(22, r, g, b);
      strip.setPixelColor(2, r, g, b);
      strip.setPixelColor(95 - 21, r, g, b);
      strip.setPixelColor(95 - 1, r, g, b);
      
      strip_2.setPixelColor(15, r, g, b);
      strip_2.setPixelColor(44, r, g, b);
    }
    else if (i == 15) {
      r = 0;
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(21, r, g, b);
      strip.setPixelColor(3, r, g, b);
      strip.setPixelColor(95 - 20, r, g, b);
      strip.setPixelColor(95 - 2, r, g, b);
      
      strip_2.setPixelColor(14, r, g, b);
      strip_2.setPixelColor(45, r, g, b);
    }
    else if (i == 16) {
      r = 0;
      g = 0;
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(20, r, g, b);
      strip.setPixelColor(4, r, g, b);
      strip.setPixelColor(95 - 19, r, g, b);
      strip.setPixelColor(95 - 3, r, g, b);
      
      strip_2.setPixelColor(13, r, g, b);
      strip_2.setPixelColor(46, r, g, b);
    }
    else if (i == 17) {
      r = (uint8_t)(v * 60.0);
      g = 0;
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(19, r, g, b);
      strip.setPixelColor(5, r, g, b);
      strip.setPixelColor(95 - 18, r, g, b);
      strip.setPixelColor(95 - 4, r, g, b);

      strip.setPixelColor(95 + 10, r, g, b);
      strip.setPixelColor(95 + 11, r, g, b);
      strip.setPixelColor(95 + 25, r, g, b);
      strip.setPixelColor(95 + 26, r, g, b);
      strip.setPixelColor(95 + 81, r, g, b);
      strip.setPixelColor(95 + 82, r, g, b);
      strip.setPixelColor(95 + 96, r, g, b);
      strip.setPixelColor(95 + 97, r, g, b);
      
      strip_2.setPixelColor(12, r, g, b);
      strip_2.setPixelColor(47, r, g, b);
    }
    else if (i == 18) {
      r = (uint8_t)(v * 100.0);
      g = 0;
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(18, r, g, b);
      strip.setPixelColor(6, r, g, b);
      strip.setPixelColor(95 - 17, r, g, b);
      strip.setPixelColor(95 - 5, r, g, b);

      strip.setPixelColor(95 + 12, r, g, b);
      strip.setPixelColor(95 + 13, r, g, b);
      strip.setPixelColor(95 + 27, r, g, b);
      strip.setPixelColor(95 + 28, r, g, b);
      strip.setPixelColor(95 + 83, r, g, b);
      strip.setPixelColor(95 + 84, r, g, b);
      strip.setPixelColor(95 + 98, r, g, b);
      strip.setPixelColor(95 + 99, r, g, b);
      
      strip_2.setPixelColor(10, r, g, b);
      strip_2.setPixelColor(11, r, g, b);
      strip_2.setPixelColor(48, r, g, b);
      strip_2.setPixelColor(49, r, g, b);
    }
    else if (i == 19) {
      r = (uint8_t)(v * 140.0);
      g = 0;
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(17, r, g, b);
      strip.setPixelColor(7, r, g, b);
      strip.setPixelColor(95 - 16, r, g, b);
      strip.setPixelColor(95 - 6, r, g, b);

      strip.setPixelColor(95 + 14, r, g, b);
      strip.setPixelColor(95 + 15, r, g, b);
      strip.setPixelColor(95 + 29, r, g, b);
      strip.setPixelColor(95 + 30, r, g, b);
      strip.setPixelColor(95 + 85, r, g, b);
      strip.setPixelColor(95 + 86, r, g, b);
      strip.setPixelColor(95 + 100, r, g, b);
      strip.setPixelColor(95 + 101, r, g, b);
      
      strip_2.setPixelColor(8, r, g, b);
      strip_2.setPixelColor(9, r, g, b);
      strip_2.setPixelColor(50, r, g, b);
      strip_2.setPixelColor(51, r, g, b);
    }
    else if (i == 20) {
      r = (uint8_t)(v * 180.0);
      g = 0;
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(16, r, g, b);
      strip.setPixelColor(8, r, g, b);
      strip.setPixelColor(95 - 15, r, g, b);
      strip.setPixelColor(95 - 7, r, g, b);

      strip.setPixelColor(95 + 16, r, g, b);
      strip.setPixelColor(95 + 17, r, g, b);
      strip.setPixelColor(95 + 31, r, g, b);
      strip.setPixelColor(95 + 32, r, g, b);
      strip.setPixelColor(95 + 87, r, g, b);
      strip.setPixelColor(95 + 88, r, g, b);
      strip.setPixelColor(95 + 102, r, g, b);
      strip.setPixelColor(95 + 103, r, g, b);
      
      strip_2.setPixelColor(6, r, g, b);
      strip_2.setPixelColor(7, r, g, b);
      strip_2.setPixelColor(52, r, g, b);
      strip_2.setPixelColor(53, r, g, b);
    }
    else if (i == 21) {
      r = (uint8_t)(v * 210.0);
      g = 0;
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(15, r, g, b);
      strip.setPixelColor(9, r, g, b);
      strip.setPixelColor(95 - 14, r, g, b);
      strip.setPixelColor(95 - 8, r, g, b);

      strip.setPixelColor(95 + 18, r, g, b);
      strip.setPixelColor(95 + 19, r, g, b);
      strip.setPixelColor(95 + 33, r, g, b);
      strip.setPixelColor(95 + 34, r, g, b);
      strip.setPixelColor(95 + 89, r, g, b);
      strip.setPixelColor(95 + 90, r, g, b);
      strip.setPixelColor(95 + 104, r, g, b);
      strip.setPixelColor(95 + 105, r, g, b);
      
      strip_2.setPixelColor(4, r, g, b);
      strip_2.setPixelColor(5, r, g, b);
      strip_2.setPixelColor(54, r, g, b);
      strip_2.setPixelColor(55, r, g, b);
    }
    else if (i == 22) {
      r = (uint8_t)(v * 230.0);
      g = 0;
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(14, r, g, b);
      strip.setPixelColor(10, r, g, b);
      strip.setPixelColor(95 - 13, r, g, b);
      strip.setPixelColor(95 - 9, r, g, b);

      strip.setPixelColor(95 + 20, r, g, b);
      strip.setPixelColor(95 + 21, r, g, b);
      strip.setPixelColor(95 + 35, r, g, b);
      strip.setPixelColor(95 + 36, r, g, b);
      strip.setPixelColor(95 + 91, r, g, b);
      strip.setPixelColor(95 + 92, r, g, b);
      strip.setPixelColor(95 + 106, r, g, b);
      strip.setPixelColor(95 + 107, r, g, b);
      
      strip_2.setPixelColor(2, r, g, b);
      strip_2.setPixelColor(3, r, g, b);
      strip_2.setPixelColor(56, r, g, b);
      strip_2.setPixelColor(57, r, g, b);
    }
    else if (i == 23) {
      r = (uint8_t)(v * 255.0);
      g = 0;
      b = (uint8_t)(v * 255.0);
      strip.setPixelColor(13, r, g, b);         // Right ring
      strip.setPixelColor(12, r, g, b);
      strip.setPixelColor(11, r, g, b);
      strip.setPixelColor(95 - 12, r, g, b);    // Left ring
      strip.setPixelColor(95 - 11, r, g, b);
      strip.setPixelColor(95 - 10, r, g, b);

      strip.setPixelColor(95 + 22, r, g, b);    // Bottom left strip on left side
      strip.setPixelColor(95 + 23, r, g, b);
      strip.setPixelColor(95 + 24, r, g, b);
      strip.setPixelColor(95 + 37, r, g, b);    // Bottom right strip on left side
      strip.setPixelColor(95 + 38, r, g, b);
      strip.setPixelColor(95 + 39, r, g, b);
      strip.setPixelColor(95 + 93, r, g, b);    // Bottom left strip on right side
      strip.setPixelColor(95 + 94, r, g, b);
      strip.setPixelColor(95 + 95, r, g, b);
      strip.setPixelColor(95 + 108, r, g, b);   // Bottom right strip on right side
      strip.setPixelColor(95 + 109, r, g, b);
      strip.setPixelColor(95 + 110, r, g, b);
      
      strip_2.setPixelColor(0, r, g, b);        // Outer ring
      strip_2.setPixelColor(1, r, g, b);
      strip_2.setPixelColor(58, r, g, b);
      strip_2.setPixelColor(59, r, g, b);
    }

  }
  
  strip.show();
  strip_2.show();
}
