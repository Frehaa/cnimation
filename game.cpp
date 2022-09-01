#include "game.h"

#include <stdint.h>


typedef void(*pixel_update_fn)(uint32_t*, int, int, int);


bool inCircle(int xDiff, int yDiff, int radius) {
    return xDiff * xDiff + yDiff * yDiff < radius * radius;
}

// 0xxxRRGGBB (low bits are blue, middle green, high red, and then padding)
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return b | (g << 8) | (r << 16);
}

void render(int t, pixel_update_fn updatePixel, GameBitmap bitmap) {
    int pitch = bitmap.width;
    uint32_t *pixel = (uint32_t *)bitmap.memory;

    for (int y = 0; y < bitmap.height; ++y) {
        for (int x = 0; x < bitmap.width; ++x) {
            updatePixel(pixel, x, y, t);
            ++pixel;
        }
    }
}


// void renderGradientCircle(uint32_t *pixel, int x, int y, int t) {
//     int centerWidth = globalWindowWidth / 2;
//     int centerHeight = globalWindowHeight / 2;
//     int xDiff = abs(x - centerWidth);
//     int yDiff = abs(y - centerHeight);
//     if (inCircle(xDiff, yDiff, 10)) {
//         *pixel = rgb(250,0,0);
//     } else {
//         *pixel = rgb(55,55,55);
//     }
// }

// void renderSinusCurve(uint32_t *pixel, int x, int y, int t) { 
//     double v = sinf((float)(x + t/50) / 30.0) * 50.0;
//     int lineHeight = v + globalWindowHeight / 2;
//     int curveWidth = 5;
//     int dotWidth = 3;
//     if (lineHeight - curveWidth < y && y < lineHeight + curveWidth)  {
//         bool b = (t/2 % globalWindowWidth) - dotWidth < x && x < (t/2 % globalWindowWidth) + dotWidth;
//         *pixel = rgb(255, 0, 0) * b + rgb(0, 0, 0) * (1-b);
//     } else {
//         *pixel = 0xffffffff;
//     }
// }

void renderWeirdGradient(uint32_t *pixel, int x, int y, int t) {
    float p = (float) x / 255.0f;
    uint8_t r = 100 * (1-p) + 255 * p;
    uint8_t g = 50 * (1-p) + 150 * p;
    uint8_t b = 0 * (1-p) + 40 * p;
    *pixel = rgb(r, g, b);
}

float gradient(int cx, int cy, int x, int y, float maxSqDist) {
    int diffX = cx - x;
    int diffY = cy - y;

    float sqDist = diffX * diffX + diffY * diffY;
    if (sqDist > maxSqDist) {
        return 0;
    } else {
        return (1 - (sqDist / maxSqDist));
    }
}

void renderFlat(uint32_t* pixel, int x, int y, int t) {
    *pixel = rgb(0, 0, 255);
}

void renderWeirdGradient2(uint32_t *pixel, int x, int y, int t) {
    float rg = gradient(200, 200, x, y, 100000);
    float gg = gradient(500, 100, x, y, 100000);
    float bg = gradient(450, 500, x, y, 100000);

    uint8_t r =  255 * rg;
    uint8_t g = 255 * gg;
    uint8_t b = 255 * bg;
    *pixel = rgb(r, g, b);
}

int32_t floatToInt32(float f) {
    return (int32_t)(f + 0.5f);
}

uint32_t floatToUInt32(float f) {
    return (uint32_t)(f + 0.5f);
}

void renderRectangle(GameBitmap bitmap, float fminX, float fminY, float fmaxX, float fmaxY, float r, float g, float b) {

    int32_t minX = floatToInt32(fminX);
    int32_t minY = floatToInt32(fminY);
    int32_t maxX = floatToInt32(fmaxX);
    int32_t maxY = floatToInt32(fmaxY);

    minX = (minX < 0)? 0 : minX;
    minY = (minY < 0)? 0 : minY;
    maxX = (maxX >= bitmap.width)? bitmap.width : maxX;
    maxY = (maxY >= bitmap.height)? bitmap.height : maxY;

    uint32_t color = (floatToUInt32(255.0f * r) << 16) | (floatToUInt32(255.0f * g) << 8) | (floatToUInt32(255.0f * b));

    uint32_t *pixel = (uint32_t *)bitmap.memory;
    for (int32_t y = minY; y < maxY; ++y) {
        for (int32_t x = minX; x < maxX; ++x) {
            *(pixel + x + y * bitmap.width) = color;
        }
    }

}

void initializeGameState(GameMemory gameMemory) {
    GameState *gameState = (GameState *) gameMemory.persistent;
    gameState->playerX = 10.0f;
    gameState->playerY = 10.0f;
}

void updateAndRenderGame(GameMemory gameMemory, GameInput gameInput, GameBitmap bitmap, SoundBuffer gameSound) {
    GameState *gameState = (GameState *) gameMemory.persistent;

    float minX = 10.0f;
    float minY = 10.0f;
    float maxX = 30.0f;
    float maxY = 30.0f;
    float r = 1.0f;
    float g = 0.2f;
    float b = 0.3f;
    renderRectangle(bitmap, 0.0f, 0.0f, 720.0f, 480.0f, 1.0f, 0.0f, 1.0f);
    // renderRectangle(bitmap, minX, minY, maxX, maxY, r, g, b);

    uint32_t tilemap[9][16] = {
        { 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,},
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,},
        { 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        { 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,},
        { 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,},
        { 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1,},
        { 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1,},
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1,},
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1,},
    };

    float tileWidth = 64.0f;
    float tileHeight = 64.0f;
    for (int32_t row = 0; row < 9; ++row) {
        for (int32_t col = 0; col < 16; ++col) {
            float color = (tilemap[row][col] == 1)? 0.5f : 0.0f;

            float minX = col * tileWidth;
            float minY = row * tileHeight;
            float maxX = minX + tileWidth; 
            float maxY = minY + tileWidth;
            renderRectangle(bitmap, minX, minY, maxX, maxY, color, color, color);
        }

    }

}
