//API: https://docs.m5stack.com/#/en/arduino/arduino_home_page

#include "M5StickC.h"

#define TOP_HEIGHT 80
#define BOTTOM_HEIGHT 80
#define WIDTH 80
#define HEIGHT 160
#define GRAIN_COUNT_TOTAL 4800

#define COLOR_GLASS 0x96BF

RTC_TimeTypeDef RTC_TimeStruct;
//RTC_DateTypeDef RTC_DateStruct;



int getLeftBorder(int y){
  if (y < 40)
        return 0;
    else if (y < 80)
        return y - 40;
    else if (y < 120)
        return 120 - y;
    else
        return 0;
}

int getRightBorder(int y){
  return WIDTH - getLeftBorder(y);
}

void drawBorders(){
  //let glassStyle = '#DCEAF7';
  //ctx.fillStyle = glassStyle;
  for (int y = 0; y < HEIGHT; y++) {
    int leftBorder = getLeftBorder(y);
    int rightBorder = getRightBorder(y);
    //ctx.fillRect(leftBorder, y, rightBorder - leftBorder, 1);
    M5.Lcd.drawLine(leftBorder, y, rightBorder, y, COLOR_GLASS);
  }
}

void setup() {
M5.begin();
//horizontal rotation
M5.Lcd.setRotation(0);
M5.Lcd.fillScreen(BLACK);
M5.Lcd.drawLine(0,0,40,80, RED);
drawBorders();
}

void loop(){
  M5.update();
  M5.Rtc.GetTime(&RTC_TimeStruct);
  
}
