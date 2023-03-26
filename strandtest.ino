// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 1
#define LED_PIN_2 3

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 96+119
#define LED_COUNT_2 60


// Seeduino Xiao has 3 built-in LEDs I want to try turning off to save a bit of power when running off battery
#define PIN_LED_13  (13u)
#define PIN_LED     PIN_LED_13
#define LED_BUILTIN PIN_LED

#define PIN_LED_RXL          (12u)
#define PIN_LED_TXL          (11u)
#define PIN_LED2             PIN_LED_RXL
#define PIN_LED3             PIN_LED_TXL
// End of Seeduino Xiao built-in LED declarations


// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_2(LED_COUNT_2, LED_PIN_2, NEO_GRB + NEO_KHZ800);

  // Argument 1 = Number of pixels in NeoPixel strip
  // Argument 2 = Arduino pin number (most are valid)
  // Argument 3 = Pixel type flags, add together as needed:
  //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
  //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
  //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
  //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
  //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
const uint8_t buflen = 24;  // * 4;
char buf[buflen];
char emptyArr[buflen] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Empty array for 'quick' comparison with our input buffer from serial
uint16_t waitHue = 0; // Hue of "first pixel" to call rainbow function that we can increment while waiting for the serial connection
uint8_t waitBrightness = 150;
uint8_t emptyCount = 0; // # of times we've gotten empty data from serial
// uint8_t waitBrightness = 255;
// bool waitFalling = true;


// setup() function -- runs once at startup --------------------------------

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Built-in LED I want to turn off to try saving a bit of power
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(35); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip_2.begin();
  strip_2.show();
  strip_2.setBrightness(75);
  Serial.begin(115200);
  while (!Serial) {
    waitingRainbow();
  }
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  if (Serial.available() >= buflen) {
    int rlen = Serial.readBytes(buf, buflen);
    digitalWrite(PIN_LED2, HIGH); // High = off, basically just keep the serial LEDs from turning on
    digitalWrite(PIN_LED3, HIGH);
    if (memcmp(buf, emptyArr, buflen) == 0) {
      if (emptyCount > 10) {
        waitingRainbow();
      }
      else {
        if (emptyCount == 1) { // In case any pixels got stuck, like if music was paused - wait a few frames then clear
          strip.clear();
          strip.show();
          strip_2.clear();
          strip_2.show();
        }
        emptyCount++;
      }
    }
    else {
        emptyCount = 0;
        waitBrightness = 0;
        eqVisualizeRings();
        eqVisualizeOuter();  // Map and set the outer 60-pixel LED strip that is taped around the outer edges of controller
      }
  }
}

void waitingRainbow() {
  if (waitBrightness < 150) {
    waitBrightness += 15;
  }
  strip.rainbow(waitHue, 5, 255, waitBrightness, true);  //Serial.print("A");  // wait for serial port to connect. Needed for native USB port only
  strip.show();
  strip_2.rainbow(waitHue, 1, 255, waitBrightness, true);
  strip_2.show();
  waitHue -= 256;
}


void eqVisualizeOuter() {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

  for (int i = 0; i < buflen; i++) {
    float v = buf[i] / 255.0;

    if (i == 23) {
      r = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(29, r, g, b);
      strip_2.setPixelColor(30, r, g, b);
    }

    else if (i == 22) {
      r = (uint8_t)(v * 220.0);
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(28, r, g, b);
      strip_2.setPixelColor(31, r, g, b);
    }

    else if (i == 21) {
      r = (uint8_t)(v * 185.0);
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(27, r, g, b);
      strip_2.setPixelColor(32, r, g, b);
    }

    else if (i == 20) {
      r = (uint8_t)(v * 150.0);
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(26, r, g, b);
      strip_2.setPixelColor(33, r, g, b);
    }

    else if (i == 19) {
      r = (uint8_t)(v * 125.0);
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(25, r, g, b);
      strip_2.setPixelColor(34, r, g, b);
    }

    else if (i == 18) {
      r = (uint8_t)(v * 90.0);
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(24, r, g, b);
      strip_2.setPixelColor(35, r, g, b);
    }

    else if (i == 17) {
      r = (uint8_t)(v * 55.0);
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(23, r, g, b);
      strip_2.setPixelColor(36, r, g, b);
    }

    else if (i == 16) {
      r = 0;
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(22, r, g, b);
      strip_2.setPixelColor(37, r, g, b);
    }

    else if (i == 15) {
      g = (uint8_t)(v * 200.0);
      b = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(21, r, g, b);
      strip_2.setPixelColor(38, r, g, b);
    }

    else if (i == 14) {
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 200.0);
      strip_2.setPixelColor(20, r, g, b);
      strip_2.setPixelColor(39, r, g, b);
    }

    else if (i == 13) {
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 165.0);
      strip_2.setPixelColor(19, r, g, b);
      strip_2.setPixelColor(40, r, g, b);
    }

    else if (i == 12) {
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 130.0);
      strip_2.setPixelColor(18, r, g, b);
      strip_2.setPixelColor(41, r, g, b);
    }

    else if (i == 11) {
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 95.0);
      strip_2.setPixelColor(17, r, g, b);
      strip_2.setPixelColor(42, r, g, b);
    }

    else if (i == 10) {
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 60.0);
      strip_2.setPixelColor(16, r, g, b);
      strip_2.setPixelColor(43, r, g, b);
    }

    else if (i == 9) {
      g = (uint8_t)(v * 255.0);
      b = (uint8_t)(v * 25.0);
      strip_2.setPixelColor(15, r, g, b);
      strip_2.setPixelColor(44, r, g, b);
    }

    else if (i == 8) {
      g = (uint8_t)(v * 255.0);
      b = 0;
      strip_2.setPixelColor(14, r, g, b);
      strip_2.setPixelColor(45, r, g, b);
    }

    else if (i == 7) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 255.0);
      strip_2.setPixelColor(13, r, g, b);
      strip_2.setPixelColor(46, r, g, b);
    }

    else if (i == 6) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 220.0);
      strip_2.setPixelColor(12, r, g, b);
      strip_2.setPixelColor(47, r, g, b);
    }

    else if (i == 5) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 185.0);
      strip_2.setPixelColor(10, r, g, b);
      strip_2.setPixelColor(11, r, g, b);
      strip_2.setPixelColor(48, r, g, b);
      strip_2.setPixelColor(49, r, g, b);
    }

    else if (i == 4) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 150.0);
      strip_2.setPixelColor(8, r, g, b);
      strip_2.setPixelColor(9, r, g, b);
      strip_2.setPixelColor(50, r, g, b);
      strip_2.setPixelColor(51, r, g, b);
    }

    else if (i == 3) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 115.0);
      strip_2.setPixelColor(6, r, g, b);
      strip_2.setPixelColor(7, r, g, b);
      strip_2.setPixelColor(52, r, g, b);
      strip_2.setPixelColor(53, r, g, b);
    }

    else if (i == 2) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 80.0);
      strip_2.setPixelColor(4, r, g, b);
      strip_2.setPixelColor(5, r, g, b);
      strip_2.setPixelColor(54, r, g, b);
      strip_2.setPixelColor(55, r, g, b);
    }

    else if (i == 1) {
      r = (uint8_t)(v * 255.0);
      g = (uint8_t)(v * 45.0);
      strip_2.setPixelColor(2, r, g, b);
      strip_2.setPixelColor(3, r, g, b);
      strip_2.setPixelColor(56, r, g, b);
      strip_2.setPixelColor(57, r, g, b);
    }

    else if (i == 0) {
      r = (uint8_t)(v * 255.0);
      g = 0;
      strip_2.setPixelColor(0, r, g, b);
      strip_2.setPixelColor(1, r, g, b);
      strip_2.setPixelColor(58, r, g, b);
      strip_2.setPixelColor(59, r, g, b);
    }

    strip_2.show();
  }
}


// Maps the 24 bins to the two 48-LED rings that are connected
void eqVisualizeRings() {
  int low = 36;   // Can also set this to buflen?  Hard coding for now, would have to treat differently for odd number of pixels
  int high = 36;  // Both start at 24 + 12 (offset for rings on sides instead of vertical)

  int red = 255.0;
  int green = 0.0;
  int blue = 0.0;

  for (int i = 0; i < buflen; i++) {
    float v = buf[i] / 255.0;

    // strip.setPixelColor(i, (uint8_t)(v * red), (uint8_t)(v * green), (uint8_t)(v * blue));
    // continue;


    if (i == 0) {  // first pixel (it's backwards, I can't remember why I did it like that) has 3, it's the highest frequencies
      strip.setPixelColor(low, (uint8_t)(v * red), (uint8_t)(v * green), (uint8_t)(v * blue));
      strip.setPixelColor(LED_COUNT - low - 1, (int)(v * red), (int)(v * green), (int)(v * blue));
      // strip.setPixelColor(LED_COUNT - low - 1, (int)(v * red), (int)(v * green), (int)(v * blue));
      low--;
      high++;
    } else if (i == buflen - 1) {  // "last" pixel for the lowest frequencies only has 1, it isn't very popular
      strip.setPixelColor(low, (uint8_t)(v * red), (uint8_t)(v * green), (uint8_t)(v * blue));
      strip.setPixelColor(LED_COUNT - low - 1, (int)(v * red), (int)(v * green), (int)(v * blue));
      // strip.setPixelColor(LED_COUNT - low - 1, (uint8_t)(v * red), (uint8_t)(v * green), (uint8_t)(v * blue));
      break;  // Break so we don't attempt to set pixels below 0 or above our strip length
    }
    if (high > 47) {  // Pixel overflow, due to the 12 offset to reorient the rings (high actually wraps around and gets the lowest 1/4 of pixels after the highest 1/4)
      high -= 48;
    }
    strip.setPixelColor(low, (uint8_t)(v * red), (uint8_t)(v * green), (uint8_t)(v * blue));
    strip.setPixelColor(LED_COUNT - low - 1, (int)(v * red), (int)(v * green), (int)(v * blue));
    strip.setPixelColor(high, (uint8_t)(v * red), (uint8_t)(v * green), (uint8_t)(v * blue));
    strip.setPixelColor(LED_COUNT - high, (int)(v * red), (int)(v * green), (int)(v * blue));
    low--;
    high++;

    // Checks for changing colors, basically the first block is red with increasing green, then green with increasing blue, then blue with increasing red
    // These are getting set for the next loop
    // This only makes sense to me when I had manually mapped the pixels and colors out... maybe that would have been better than converting it to loops and if/else statements...
    if (i == 6) {
      green = 255.0;
    } else if (i == 7) {
      red = 0.0;
    } else if (i == 14) {
      blue = 255.0;
      green = 200.0;  // Why this, it is maxed then they swap?
    } else if (i == 15) {
      green = 0.0;
    }

    // Incrementing colors when they are between their min and max (0 and 255)
    if (i >= 0 && i <= 5) {
      green += 35.0;
    } else if (i >= 8 && i <= 13) {
      blue += 35.0;
    } else if (i >= 16) {
      red += 35.0;
    }
  }

  strip.show();
}



// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
// void colorWipe(uint32_t color, int wait) {
//   for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
//     strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
//     strip.show();                          //  Update strip to match
//     delay(wait);                           //  Pause for a moment
//   }
// }

// // Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// // a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// // between frames.
// void theaterChase(uint32_t color, int wait) {
//   for(int a=0; a<10; a++) {  // Repeat 10 times...
//     for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
//       strip.clear();         //   Set all pixels in RAM to 0 (off)
//       // 'c' counts up from 'b' to end of strip in steps of 3...
//       for(int c=b; c<strip.numPixels(); c += 3) {
//         strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
//       }
//       strip.show(); // Update strip with new contents
//       delay(wait);  // Pause for a moment
//     }
//   }
// }

// // Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
// void rainbow(int wait) {
//   // Hue of first pixel runs 5 complete loops through the color wheel.
//   // Color wheel has a range of 65536 but it's OK if we roll over, so
//   // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
//   // means we'll make 5*65536/256 = 1280 passes through this loop:
//   for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
//     // strip.rainbow() can take a single argument (first pixel hue) or
//     // optionally a few extras: number of rainbow repetitions (default 1),
//     // saturation and value (brightness) (both 0-255, similar to the
//     // ColorHSV() function, default 255), and a true/false flag for whether
//     // to apply gamma correction to provide 'truer' colors (default true).
//     strip.rainbow(firstPixelHue);
//     // Above line is equivalent to:
//     // strip.rainbow(firstPixelHue, 1, 255, 255, true);
//     strip.show(); // Update strip with new contents
//     delay(wait);  // Pause for a moment
//   }
// }

// // Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
// void theaterChaseRainbow(int wait) {
//   int firstPixelHue = 0;     // First pixel starts at red (hue 0)
//   for(int a=0; a<30; a++) {  // Repeat 30 times...
//     for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
//       strip.clear();         //   Set all pixels in RAM to 0 (off)
//       // 'c' counts up from 'b' to end of strip in increments of 3...
//       for(int c=b; c<strip.numPixels(); c += 3) {
//         // hue of pixel 'c' is offset by an amount to make one full
//         // revolution of the color wheel (range 65536) along the length
//         // of the strip (strip.numPixels() steps):
//         int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
//         uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
//         strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
//       }
//       strip.show();                // Update strip with new contents
//       delay(wait);                 // Pause for a moment
//       firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
//     }
//   }
// }
