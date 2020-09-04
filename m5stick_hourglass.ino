// API: https://docs.m5stack.com/#/en/arduino/arduino_home_page

#include "M5StickC.h"

#define TOP_HEIGHT 80
#define BOTTOM_HEIGHT 80
#define WIDTH 80
#define HEIGHT 160
#define GRAIN_COUNT_TOTAL 4800

#define COLOR_GLASS 0xDEBA // 0x96BF
#define COLOR_M5STICK_ORANGE 0xFAE2
#define COLOR_GRAIN 0xE122 //0x72A4 // 0xE5A9 //0x72A4

// the main configuration parameter
int hourglassSpinSeconds = 300;
long hourglassSpinMillis;

int grainsTop;
int bottomGrains[BOTTOM_HEIGHT][WIDTH];

#define GRAIN_COLOR_LENGTH 214
int grainColors[GRAIN_COLOR_LENGTH] = {0x3000,0x6000,0x9000,0xc000,0xf800,0x6,0x3006,0x6006,0x9006,0xc006,0xf806,0xc,0x300c,0x600c,0x900c,0xc00c,0xf80c,0x12,0x3012,0x6012,0x9012,0xc012,0xf812,0x18,0x3018,0x6018,0x9018,0xc018,0xf818,0x1f,0x301f,0x601f,0x901f,0xc01f,0xf81f,0x180,0x3180,0x6180,0x9180,0xc180,0xf980,0x186,0x3186,0x6186,0x9186,0xc186,0xf986,0x18c,0x318c,0x618c,0x918c,0xc18c,0xf98c,0x192,0x3192,0x6192,0x9192,0xc192,0xf992,0x198,0x3198,0x6198,0x9198,0xc198,0xf998,0x19f,0x319f,0x619f,0x919f,0xc19f,0xf99f,0x320,0x3320,0x6320,0x9320,0xc320,0xfb20,0x326,0x3326,0x6326,0x9326,0xc326,0xfb26,0x32c,0x332c,0x632c,0x932c,0xc32c,0xfb2c,0x332,0x3332,0x6332,0x9332,0xc332,0xfb32,0x338,0x3338,0x6338,0x9338,0xc338,0xfb38,0x33f,0x333f,0x633f,0x933f,0xc33f,0xfb3f,0x4a0,0x34a0,0x64a0,0x94a0,0xc4a0,0xfca0,0x4a6,0x34a6,0x64a6,0x94a6,0xc4a6,0xfca6,0x4ac,0x34ac,0x64ac,0x94ac,0xc4ac,0xfcac,0x4b2,0x34b2,0x64b2,0x94b2,0xc4b2,0xfcb2,0x4b8,0x34b8,0x64b8,0x94b8,0xc4b8,0xfcb8,0x4bf,0x34bf,0x64bf,0x94bf,0xc4bf,0xfcbf,0x640,0x3640,0x6640,0x9640,0xc640,0xfe40,0x646,0x3646,0x6646,0x9646,0xc646,0xfe46,0x64c,0x364c,0x664c,0x964c,0xc64c,0xfe4c,0x652,0x3652,0x6652,0x9652,0xc652,0xfe52,0x658,0x3658,0x6658,0x9658,0xc658,0xfe58,0x65f,0x365f,0x665f,0x965f,0xc65f,0xfe5f,0x7e0,0x37e0,0x67e0,0x97e0,0xc7e0,0xffe0,0x7e6,0x37e6,0x67e6,0x97e6,0xc7e6,0xffe6,0x7ec,0x37ec,0x67ec,0x97ec,0xc7ec,0xffec,0x7f2,0x37f2,0x67f2,0x97f2,0xc7f2,0xfff2,0x7f8,0x37f8,0x67f8,0x97f8,0xc7f8,0xfff8,0x7ff,0x37ff,0x67ff,0x97ff,0xc7ff};

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
      if (y < TOP_HEIGHT - 1) {
        // draw partial row - disappearing from center out
        int remainingGrains = grainCount - currentGrains;
        int leftHalf = remainingGrains / 2;
        M5.Lcd.drawLine(leftBorder, y, leftBorder + leftHalf, y, COLOR_GRAIN);
        M5.Lcd.drawLine(rightBorder - leftHalf, y, rightBorder, y, COLOR_GRAIN);

        // fill out the rest with "glass"

        M5.Lcd.drawLine(leftBorder + leftHalf, y, rightBorder - leftHalf, y,
                        COLOR_GLASS);
      }
      // fill out the line above to clear it
      M5.Lcd.drawLine(leftBorder, y - 1, rightBorder, y - 1, COLOR_GLASS);
      return;
    }
  }
}

void makePointDirty(int x, int y) {
  Point p;
  p.x = x;
  p.y = y;
  dirtyPoints[dirtyPointIndex++] = p;
}

void drawGrainsBottom() {
  for (int i = 0; i < dirtyPointIndex; i++) {
    int color = bottomGrains[dirtyPoints[i].y][dirtyPoints[i].x];
    M5.Lcd.drawPixel(dirtyPoints[i].x, dirtyPoints[i].y + BOTTOM_HEIGHT,
                        color
                         ? color
                         : COLOR_GLASS);
  }
  dirtyPointIndex = 0;
}

void drawBorders() {
  for (int y = 0; y < HEIGHT; y++) {
    int leftBorder = getLeftBorder(y);
    int rightBorder = WIDTH - getLeftBorder(y);
    M5.Lcd.drawLine(leftBorder, y, rightBorder, y, COLOR_GLASS);
  }
  M5.Lcd.setCursor(0, 80, 1);
  M5.Lcd.printf("%d:00", hourglassSpinSeconds / 60);
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
        bottomGrains[y + 1][x] = bottomGrains[y][x];
        bottomGrains[y][x] = 0;
        makePointDirty(x, y + 1);
        makePointDirty(x, y);
      }
      // if there IS a grain underneath - check if we can fall to the left
      else if (y < BOTTOM_HEIGHT - 2) {
        if (x > 0 && bottomGrains[y + 1][x - 1] == 0) {
          // swap the grains
          bottomGrains[y + 1][x - 1] = bottomGrains[y][x];
          bottomGrains[y][x] = 0;
          makePointDirty(x - 1, y + 1);
          makePointDirty(x, y);
        } // look to the right
        else if (x < WIDTH - 1 && bottomGrains[y + 1][x + 1] == 0) {
          // swap the grains
          bottomGrains[y + 1][x + 1] = bottomGrains[y][x];
          bottomGrains[y][x] = 0;
          makePointDirty(x + 1, y + 1);
          makePointDirty(x, y);
        }
      }
    }
  }
}

inline int randomGrainColor(){
  return grainColors[random(GRAIN_COLOR_LENGTH)];
}

void grainTick() {
  // remove a grain from the top
  grainsTop--;
  // spawn a grain at the bottom
  bottomGrains[0][WIDTH / 2] = randomGrainColor();
}

void draw() {
  drawGrainsTop(grainsTop);
  drawGrainsBottom();
}

// amount of grains in the top part at this time
int grainsForMillisElapsed(long millisElapsed) {
  return GRAIN_COUNT_TOTAL -
         ((float)millisElapsed / hourglassSpinMillis) * GRAIN_COUNT_TOTAL;
}

void tick() {
  long millisCurrent = millis();
  long millisElapsed = millisCurrent - millisStart;
  int newGrainsTop = grainsForMillisElapsed(millisElapsed);

  // calculate amount of grains to move
  int grainDifference = grainsTop - newGrainsTop;
  while (grainsTop > newGrainsTop) {
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
  grainsTop = GRAIN_COUNT_TOTAL;
  initializeBottomGrains();
  drawBorders();
  draw();
  millisStart = millis();
}

void nextInterval() {
  if (hourglassSpinSeconds == 60) {
    hourglassSpinSeconds = 180;
  } else if (hourglassSpinSeconds == 180) {
    hourglassSpinSeconds = 300;
  } else if (hourglassSpinSeconds == 300) {
    hourglassSpinSeconds = 600;
  } else if (hourglassSpinSeconds == 600) {
    hourglassSpinSeconds = 900;
  } else if (hourglassSpinSeconds == 900) {
    hourglassSpinSeconds = 60;
  }
}

void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    reset();
    return;
  }
  if (M5.BtnB.wasPressed()) {
    nextInterval();
    reset();
    return;
  }
  tick();
}
