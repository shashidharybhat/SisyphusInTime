#include <TFT_eSPI.h>
#include <SPI.h>
#include "uRTCLib.h"

TFT_eSPI tft = TFT_eSPI();
uRTCLib rtc;
byte rtcModel = URTCLIB_MODEL_DS1307;


const unsigned char sisyphus[] PROGMEM = {
  0x00, 0xe0, 0x06, 0x20, 0x07, 0x40, 0x1f, 0x80, 0x1c, 0x00, 0x38, 0x00, 0x3c, 0x00, 0x3e, 0x00,
  0x32, 0x00, 0x24, 0x00, 0x24, 0x00, 0x40, 0x00, 0x40, 0x00, 0x60, 0x00, 0x00, 0x00
};

enum STATES {
  PUSH,
  ZENITH,
  THEFALL,
  AHMAN,
  LETSGO,
  START,
  INIT
};
STATES prevStatus = START;
STATES currentStatus = PUSH;

int32_t w, h;
int y;

int prev_y[160];

int ball_x;
int sis_x;
int sis_y;

float sis_inc_x = 0.0;
float ball_inc_x = 0.0;

float phase;
float phaseChange;

void setup() {
  Serial.begin(115200);
  URTCLIB_WIRE.begin(4, 5);
  rtc.set_rtc_address(0x68);
  rtc.set_model(rtcModel);
  rtc.enableBattery();
  rtc.refresh();
  tft.init();
  w = tft.width();
  h = tft.height();
  tft.setRotation(3);
  tft.setCursor(0, 3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  randomSeed(rtc.hour() * 3600 + rtc.minute() * 60 + rtc.second());
  if (random(5) == 0) {
    tft.println("Parsecs to go");
    tft.println("before I sleep...");
  } else {
    tft.println("Miles to go");
    tft.println("before I sleep...");
  }
}

void loop() {
  rtc.refresh();
  tft.setCursor(140, 0);
  tft.print(rtc.day());
  tft.setCursor(140, 10);
  tft.print(rtc.month());
  tft.setCursor(140, 20);
  tft.print(rtc.year());

  tft.setTextSize(2);
  tft.setCursor(5, 33);
  // tft.drawRect(5, 35, 85, 16, TFT_BROWN);
  tft.print(rtc.hour());
  tft.print(":");
  tft.print(rtc.minute());
  tft.print(":");
  tft.print(rtc.second());
  tft.setTextSize(1);


  if (currentStatus == PUSH && (prevStatus == START || prevStatus == LETSGO)) {
    phase = 1.5 * PI + 0.01;
    ball_x = 80;
    sis_x = 67;
    sis_y = 15;
    sis_inc_x = 0.0;
    ball_inc_x = 0.0;
    phaseChange = 0.01;
    prevStatus = INIT;
    Serial.println("Init");
    Serial.println(currentStatus);
  }
  // if (currentStatus == THEFALL) {
  //   phaseChange = 0.005;
  //   sis_inc_x = -0.0001;
  //   ball_inc_x = 0.75;
  // }
  // if (currentStatus == AHMAN) {
  //   phaseChange = 0.0;
  //   sis_inc_x = 3;
  //   ball_inc_x = 0.0;
  // }

  for (int i = 0; i < 160; i++) {

    float rad = i * (2 * PI / 160) + phase;
    int y = 96 + 31 * sin(rad);

    tft.drawPixel(i, prev_y[i], TFT_BLACK);

    tft.drawPixel(i, y, TFT_GOLD);

    if (i == ball_x) {
      // if (y == 127 && currentStatus == THEFALL) {
      //   phaseChange = 0.0;
      //   ball_inc_x = 0.0;
      //   sis_inc_x = 0.0;
      //   currentStatus = AHMAN;
      //   prevStatus = THEFALL;
      //   Serial.println("Ahh Maaannn");
      // }
      // // Serial.println(y);
      // if (y == 65 && currentStatus == PUSH) {
      //   currentStatus = ZENITH;
      //   prevStatus = PUSH;
      //   delay(500);
      //   currentStatus = THEFALL;
      //   prevStatus = ZENITH;
      //   Serial.println("The Fall");
      // }
      if (ball_inc_x == 0.0) {
        tft.fillCircle(i, prev_y[i] - 7, 5, TFT_BLACK);
        tft.fillCircle(i, y - 7, 5, tft.color565(0x96, 0x4B, 0x00));
      } else {
        tft.fillCircle(i - ball_inc_x, prev_y[i - 1] - 7, 5, TFT_BLACK);
        tft.fillCircle(i, y - 7, 5, tft.color565(0x96, 0x4B, 0x00));
      }
    }

    if (i == sis_x) {
      // if (y == 127 && currentStatus == AHMAN) {
      //   phaseChange = 0.0;
      //   ball_inc_x = 0.0;
      //   sis_inc_x = 0.0;
      //   currentStatus = LETSGO;
      //   prevStatus = AHMAN;
      //   delay(500);
      //   currentStatus = PUSH;
      //   prevStatus = LETSGO;
      //   Serial.println("Lets Goo");
      // }
      if (sis_inc_x == 0.0) {
        tft.drawBitmap(i, prev_y[i] - sis_y, sisyphus, 11, 15, TFT_BLACK);
        tft.drawBitmap(i, y - sis_y, sisyphus, 11, 15, TFT_WHITE);
      } else {
        tft.drawBitmap(i - sis_inc_x, prev_y[i -1] - sis_y, sisyphus, 11, 15, TFT_BLACK);
        tft.drawBitmap(i, y - sis_y, sisyphus, 11, 15, TFT_WHITE);
      }
    }

    prev_y[i] = y;
  }
  sis_x += sis_inc_x;
  ball_x += ball_inc_x;
  phase += phaseChange;
  if (phase > 2 * PI) {
    phase -= 2 * PI;
  }
  delay(20);
}