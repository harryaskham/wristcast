#include <Adafruit_NeoPixel.h>
#include "utils.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define BUTTON              0
#define RING                1
#define JEWEL               2
#define NUMPIXELS_RING      16
#define NUMPIXELS_JEWEL     7

Adafruit_NeoPixel ringPixels = Adafruit_NeoPixel(NUMPIXELS_RING, RING, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel jewelPixels = Adafruit_NeoPixel(NUMPIXELS_JEWEL, JEWEL, NEO_GRB + NEO_KHZ800);

uint8_t sinTable[] = {128, 140, 153, 165, 177, 189, 200, 210, 219, 228, 235, 242, 247, 251, 254, 255, 255, 254, 252, 249, 244, 238, 231, 223, 214, 204, 193, 182, 170, 158, 146, 133, 120, 107, 95, 83, 71, 60, 49, 39, 31, 23, 16, 10, 6, 2, 0, 0, 0, 2, 5, 9, 14, 21, 29, 37, 47, 57, 68, 80, 92, 104};

void setup() {
  ringPixels.begin();
  jewelPixels.begin();
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
}

long t = 0;
uint8_t numBrightnessLevels = 3;
uint8_t brightnessLevel = 1;

void setBrightness(uint8_t brightness) {
  ringPixels.setBrightness(brightness);
  jewelPixels.setBrightness(brightness);
  ringPixels.show();
  jewelPixels.show();
}

void setColorRing(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  ringPixels.setPixelColor(i % NUMPIXELS_RING, r, g, b);
  ringPixels.show();
}

void setColorJewel(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  jewelPixels.setPixelColor(i % NUMPIXELS_JEWEL, r, g, b);
  jewelPixels.show();
}

// Integer scaling func
uint8_t scaleBy(uint8_t x, uint8_t scale) {
  return (x * scale) / 255;
}

long scaleTime(long t, uint8_t scale) {
  return (t * long(scale)) / 255;
}

uint8_t sin255Approx(long x) {
  return sinTable[x/2 % 43];
}

int sinColor(uint8_t offset, uint8_t phaseShift) {
  return sin255Approx(scaleTime(t, phaseShift) + long(offset));
}

// button stuff
int presses = 0;
int debounceDelay = 400;
long lastTime = 0;
int lowCounter = 0;
bool handleButtonPress() {
  int button = digitalRead(BUTTON);
  if (button == LOW && millis() > lastTime + debounceDelay) {
    presses++;
    lastTime = millis();
  }

  // Hold down for special controls.
  if (button == LOW) {
    lowCounter++;
  } else {
    lowCounter = 0;
  }
  return button == LOW;
}

void strobe(SetColor setColor, uint8_t i) {
  int color = 255 * (t % 2);
  setColor(i, color, color, color);
}

void strobeRing(uint8_t i) {
  strobe(setColorRing, i);
}

void strobeJewel(uint8_t i) {
  strobe(setColorJewel, i);
}

void spinner(SetColor setColor, uint8_t i, uint8_t rMod, uint8_t gMod, uint8_t bMod, uint8_t numPeaks) {
  uint8_t divScale = 4 * (i % (NUMPIXELS_RING/numPeaks)) + 1;
  setColor(
      (t-i) % NUMPIXELS_RING,
      scaleBy(sinColor(i, 255), rMod) / divScale,
      scaleBy(sinColor(i + 16, 196), gMod) / divScale,
      scaleBy(sinColor(i + 31, 128), bMod) / divScale);
}

void colorSpinnerRing(uint8_t i) {
  spinner(setColorRing, i, 255, 255, 255, (brightnessLevel + 1) / 2);
}

void colorSpinnerJewel(uint8_t i) {
  spinner(setColorJewel, i, 255, 255, 255, 1);
}

void blueSpinnerRing(uint8_t i) {
  spinner(setColorRing, i, 0, 0, 255, (brightnessLevel + 1) / 2);
}

void redSpinnerJewel(uint8_t i) {
  spinner(setColorJewel, i, 255, 0, 0, 1);
}

void xmasSpinnerRing(uint8_t i) {
  spinner(setColorRing, i, t % 255, 255 - t % 255, 0, 2);
}

void xmasSpinnerJewel(uint8_t i) {
  spinner(setColorJewel, i, 255 - t % 255, t % 255, 0, 1);
}

void solid(SetColor setColor, uint8_t i) {
  setColor(
      i,
      sinColor(0, 255),
      sinColor(16, 255),
      sinColor(31, 255));
}

void solidRing(uint8_t i) {
  solid(setColorRing, i);
}

void solidJewel(uint8_t i) {
  solid(setColorJewel, i);
}

void offRing(uint8_t i) {
  setColorRing(i, 0, 0, 0);
}

void offJewel(uint8_t i) {
  setColorJewel(i, 0, 0, 0);
}

long tickx = 0;
long tickv = 0;
long ticka = 2;
void ticker(SetColor setColor, uint8_t i) {
  int color = i == tickx/1000 ? 255 : 0;
  setColor(i, 
      scaleBy(sinColor(i, color), color),
      scaleBy(sinColor(i+8, color/2), color),
      scaleBy(sinColor(i+16, color/4), color));

  if (i == NUMPIXELS_RING - 1) {
    tickv += ticka;
    tickx += tickv;
    if (tickx > 1000*NUMPIXELS_RING) {
      tickx -= 1000*NUMPIXELS_RING;
    }
    if (tickx < 0) {
      tickx += 1000*NUMPIXELS_RING;
    }
    if (tickv >= 1300 || tickv <= -1300) {
      ticka = -1*ticka;
    }
  }
}

void tickerRing(uint8_t i) {
  ticker(setColorRing, i);
}

typedef void (*Pattern) (uint8_t i);

Pattern ringPatterns[] = {
  offRing,
  solidRing,
  colorSpinnerRing,
  blueSpinnerRing,
  //xmasSpinnerRing,
  //tickerRing,
  strobeRing
};

Pattern jewelPatterns[] = {
  offJewel,
  solidJewel,
  colorSpinnerJewel,
  redSpinnerJewel,
  //xmasSpinnerJewel,
  //offJewel,
  strobeJewel
};

uint8_t numModes = 5;

void loop() {
  for (uint8_t i = 0; i < NUMPIXELS_RING * NUMPIXELS_JEWEL; i++) {
    handleButtonPress();

    if (lowCounter > 200) {
      // Go to watch mode
      lowCounter = 0;
      watchMode();
      return;
    }
    
    brightnessLevel = (presses % numBrightnessLevels) + 1;
    setBrightness(brightnessLevel * 64 - 1);
    uint8_t mode = (presses / numBrightnessLevels) % numModes;
    if (i % NUMPIXELS_RING == 0) {
      t++;
    }
    ringPatterns[mode](i % NUMPIXELS_RING);
    jewelPatterns[mode](i % NUMPIXELS_JEWEL);
    if (mode == 0 && presses % numBrightnessLevels == 0) {
      // Only off for one press
      presses += (numBrightnessLevels-1);
    }
  }
}

// returning from here re-enters main pattern loop
uint8_t hour = 8; // 0-11
uint8_t minute = 30; // 0-59
uint16_t dsecond = 00; // 0-599
uint8_t hourI;
uint8_t minuteI;
uint8_t dsecondI;
void watchMode() {
  for (uint8_t i = 0; i < NUMPIXELS_JEWEL; i++) {
    offJewel(i);    
  }
  setBrightness(255); // o shit
  while(true) {
    bool down = handleButtonPress();
    if (lowCounter > 255) {
      // go to pattern mode
      lowCounter = 0;
      return;
    }

    // Set watch by holding button but not for too long...
    if (down) {
      updateTime(600);
    }

    hourI = getScaledTime(hour, 12);
    minuteI = getScaledTime(minute, 60);
    dsecondI = getScaledTime(dsecond, 600);
  
    for (uint8_t i = 0; i < NUMPIXELS_RING; i++) {
      timeRing(i);
    }
    updateTime(1);  // advance a decisec

    if (!down) { // fast watch setting
      delay(87); // hand tweaked for 8mhz proc
    }
  }
}

// inverts for direction, scales, adjusts to 16 LEDs.
uint8_t getScaledTime(uint32_t realTime, uint32_t base) {
  return uint8_t((base-1-realTime) * uint32_t(NUMPIXELS_RING)*10 / base);
}

void updateTime(uint16_t dseconds) {
  dsecond += dseconds;
  while (dsecond >= 600) {
    dsecond -= 600;
    minute++;
    if (minute == 60) {
      minute = 0;
      hour++;
      if (hour == 12) {
        hour = 0;
      }
    }
  }
}

void setColorRingTime(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  setColorRing(i-4, r, g, b);
}

// 255 for right on, 0 for far away
int getDist255(uint8_t i, uint8_t timeI, uint8_t width) {
  if (width == 0) {
    return i == timeI/10 ? 255 : 0;
  }
  int dist = getDist(i*10, timeI, NUMPIXELS_RING*10);
  return 255 - min((dist*dist) / (width*width), 255);
}

// distance modulo 16
int getDist(uint8_t x, uint8_t y, uint8_t base) {
  int dist = abs(x-y);
  return min(dist, base-dist);
}

void timeRing(uint8_t i) {
  uint8_t hourCol = getDist255(i, hourI, 0);
  uint8_t minuteCol = getDist255(i, minuteI, 0);
  uint8_t dsecondCol = getDist255(i, dsecondI, 1);
  setColorRingTime(i,
      scaleBy(hourCol, 196) + scaleBy(minuteCol, 0) + scaleBy(dsecondCol, 64),
      scaleBy(hourCol, 64) + scaleBy(minuteCol, 196) + scaleBy(dsecondCol, 0),
      scaleBy(hourCol, 0) + scaleBy(minuteCol, 64) + scaleBy(dsecondCol, 196));
}

