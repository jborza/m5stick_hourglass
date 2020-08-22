//API: https://docs.m5stack.com/#/en/arduino/arduino_home_page

#include "M5StickC.h"

#define TOP_HEIGHT 80
#define BOTTOM_HEIGHT 80
#define WIDTH 80
#define HEIGHT 160
#define GRAIN_COUNT_TOTAL 4800

#define COLOR_GLASS 0x96BF
#define COLOR_M5STICK_ORANGE 0xFAE2
#define COLOR_GRAIN 0x72A4

RTC_TimeTypeDef RTC_TimeStruct;
//RTC_DateTypeDef RTC_DateStruct;

int grainsTop;

int getLeftBorder(int y)
{
  if (y < 40)
    return 0;
  else if (y < 80)
    return y - 40;
  else if (y < 120)
    return 120 - y;
  else
    return 0;
}

int getRightBorder(int y)
{
  return WIDTH - getLeftBorder(y);
}

int grainsInRow(int y)
{
  return (WIDTH - getLeftBorder(y) * 2);
}

void drawGrainsTop(int grainCount)
{
  int currentGrains = 0;
  for (int y = TOP_HEIGHT - 1; y >= 0; y--)
  {
    int grainsInThisRow = grainsInRow(y);
    //draw full row if we can
    int leftBorder = getLeftBorder(y);
    int rightBorder = WIDTH - leftBorder;
    if ((currentGrains + grainsInThisRow) < grainCount)
    {
      M5.Lcd.drawLine(leftBorder, y, rightBorder, y, COLOR_GRAIN);
      currentGrains += grainsInThisRow;
    }
    else
    {
      //draw partial row - disappearing from center out
      int remainingGrains = grainCount - currentGrains;
      int leftHalf = remainingGrains / 2;
      M5.Lcd.drawLine(leftBorder, y, leftBorder + leftHalf, y, COLOR_GRAIN);
      M5.Lcd.drawLine(rightBorder - leftHalf, y, rightBorder, y, COLOR_GRAIN);
      return;
    }
  }
}

void drawBorders()
{
  for (int y = 0; y < HEIGHT; y++)
  {
    int leftBorder = getLeftBorder(y);
    int rightBorder = getRightBorder(y);
    M5.Lcd.drawLine(leftBorder, y, rightBorder, y, COLOR_GLASS);
  }
}

void draw()
{
  drawGrainsTop(grainsTop);
}

void setup()
{
  M5.begin();
  //horizontal rotation
  M5.Lcd.setRotation(0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.drawLine(0, 0, 40, 80, RED);
  grainsTop = GRAIN_COUNT_TOTAL;
  grainsTop = 400;
  drawBorders();
  draw();
}

void loop()
{
  M5.update();
  M5.Rtc.GetTime(&RTC_TimeStruct);
}
