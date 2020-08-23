// API: https://docs.m5stack.com/#/en/arduino/arduino_home_page

#include "M5StickC.h"

#define TOP_HEIGHT 80
#define BOTTOM_HEIGHT 80
#define WIDTH 80
#define HEIGHT 160
#define GRAIN_COUNT_TOTAL 4800

#define COLOR_GLASS 0xDEBA //0x96BF
#define COLOR_M5STICK_ORANGE 0xFAE2
#define COLOR_GRAIN 0x72A4 //0xE5A9 //0x72A4

int hourglassSpinSeconds = 300;
long hourglassSpinMillis;

int grainsTop;
int bottomGrains[BOTTOM_HEIGHT][WIDTH];

typedef struct {
  int8_t x;
  int8_t y;
} Point;

#define DIRTY_POINT_MAX 255

Point dirtyPoints[DIRTY_POINT_MAX];
int dirtyPointIndex;

long millisStart;

int getLeftBorder(int y) {
  if (y < 40)
    return 0;
  else if (y < 80)
    return y - 40;
  else if (y < 120)
    return 120 - y;
  else
    return 0;
}

int grainsInRow(int y) { return (WIDTH - getLeftBorder(y) * 2); }

void drawGrainsTop(int grainCount) {
  int currentGrains = 0;
  for (int y = TOP_HEIGHT - 1; y >= 0; y--) {
    int grainsInThisRow = grainsInRow(y);
    // draw full row if we can
    int leftBorder = getLeftBorder(y);
    int rightBorder = WIDTH - leftBorder;
    if ((currentGrains + grainsInThisRow) < grainCount) {
      M5.Lcd.drawLine(leftBorder, y, rightBorder, y, COLOR_GRAIN);
      currentGrains += grainsInThisRow;
    } else {
      // draw partial row - disappearing from center out
      int remainingGrains = grainCount - currentGrains;
      int leftHalf = remainingGrains / 2;
      M5.Lcd.drawLine(leftBorder, y, leftBorder + leftHalf, y, COLOR_GRAIN);
      M5.Lcd.drawLine(rightBorder - leftHalf, y, rightBorder, y, COLOR_GRAIN);
      
      //fill out the rest with "glass"
      M5.Lcd.drawLine(leftBorder+leftHalf, y, rightBorder - leftHalf, y, COLOR_GLASS);
      return;
    }
  }
}

void drawGrainsBottom() {
  for (int y = BOTTOM_HEIGHT - 1; y >= 0; y--) {
    for (int x = 0; x < WIDTH; x++) {
      if (bottomGrains[y][x] == 1) {
        M5.Lcd.drawPixel(x, y + BOTTOM_HEIGHT, COLOR_GRAIN);
      }
    }
  }
}

void drawBorders() {
  for (int y = 0; y < HEIGHT; y++) {
    int leftBorder = getLeftBorder(y);
    int rightBorder = WIDTH - getLeftBorder(y);
    M5.Lcd.drawLine(leftBorder, y, rightBorder, y, COLOR_GLASS);
  }
}

void initializeBottomGrains() {
  for (int y = 0; y < BOTTOM_HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      bottomGrains[y][x] = 0;
    }
  }
}

void physicsStep() {
  // note that we skip bottom-most row as nothing can happen there
  for (int y = BOTTOM_HEIGHT - 2; y >= 0; y--) {
    for (int x = 0; x < WIDTH; x++) {
      // if there's no grain, don't do anything
      if (bottomGrains[y][x] == 0) {
        continue;
      }
      // if there's a space under a grain, let it fall
      else if (bottomGrains[y + 1][x] == 0) {
        bottomGrains[y + 1][x] = 1;
        bottomGrains[y][x] = 0;
      }
      // if there IS a grain underneath - check if we can fall to the left
      else if (y < BOTTOM_HEIGHT - 2) {
        if (x > 0 && bottomGrains[y + 1][x - 1] == 0) {
          // swap the grains
          bottomGrains[y + 1][x - 1] = 1;
          bottomGrains[y][x] = 0;
        } // look to the right
        else if (x < WIDTH - 2 && bottomGrains[y + 1][x + 1] == 0) {
          // swap the grains
          bottomGrains[y + 1][x + 1] = 1;
          bottomGrains[y][x] = 0;
        }
      }
    }
  }
}

void grainTick(){
  //remove a grain from the top
  grainsTop--;
  //spawn a grain at the bottom
  bottomGrains[0][WIDTH / 2] = 1;
}

void draw() {
  drawGrainsTop(grainsTop);
  drawGrainsBottom();
}

//amount of grains in the top part at this time
int grainsForMillisElapsed(long millisElapsed){
  return ((float)millisElapsed / hourglassSpinMillis) * GRAIN_COUNT_TOTAL;
}

void tick() {
  long millisCurrent = millis();
  long millisElapsed = millisCurrent - millisStart;
  int newGrainsTop = grainsForMillisElapsed(millisElapsed);
  //calculate amount of grains to move
  int grainDifference = grainsTop - newGrainsTop;
  for(int i = 0; i < grainDifference; i++){
    grainTick();
    physicsStep();
  }
  draw();
}

void setup() {
  M5.begin();
  // horizontal rotation
  M5.Lcd.setRotation(0);
  reset();
}

void reset() {
  M5.Lcd.fillScreen(BLACK);
  hourglassSpinMillis = hourglassSpinSeconds * 1000L;
  grainsTop = 1400;
  initializeBottomGrains();
  drawBorders();
  draw();
  millisStart = millis();
}

void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    reset();
    return;
  }
  tick();
}
