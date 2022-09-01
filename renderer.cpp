#include "game.h"
#include "renderer.h"

#include <stdint.h>
#include <math.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062f

void drawDot(Bitmap bitmap, Point position, int radius, uint32_t color) {
    uint32_t *pixel = (uint32_t *) bitmap.memory;
    pixel += ((int32_t)position.x) + bitmap.width * ((int32_t)position.y);

    if (position.x < 0 || position.x > bitmap.width || position.y < 0 || position.y > bitmap.height) return;
    *pixel = color;

    // *(pixel - 1) = color;
    // *(pixel + 1) = color;
    // *(pixel - bitmap.width) = color;
    // *(pixel + bitmap.width) = color;
}

void fillRect(Bitmap bitmap, Point topLeft, Point bottomRight, uint32_t color) {
    fillRect(bitmap, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y, color);
}

void fillRect(Bitmap bitmap, float minX, float minY, float maxX, float maxY, uint32_t color) {
    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX > width) maxX = b.width;
    if (maxY > height) maxY = b.height;
    
    uint32_t *pixel = (uint32_t *) b.memory;

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            (pixel + y * width + x) = color;
        }
    }

}

void drawRect(Bitmap bitmap, Point topLeft, Point topRight, float width, uint32_t color) {
    drawRect(bitmap, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y, width, color);
}

void drawRect(Bitmap bitmap, float minX, float minY, float maxX, float maxY, float width, uint32_t color) {
    // TODO: Assuming width = 1
    int width = b.width;
    int height = b.height;
    uint32_t *pixel = (uint32_t *) b.memory;

}

void drawLine(Bitmap bitmap, Point start, Point end, float width, uint32_t color) { 
    int pitch = bitmap.width;

    uint32_t *pixel = (uint32_t *) bitmap.memory;
    pixel += ((int32_t)start.x) + pitch * ((int32_t)start.y);

    int dx = end.x - start.x;
    int dy = end.y - start.y;
    int yi = 1;
    int xi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int steps; // Number + 1 pixels to color. Is based on the longest axis  
    int step;  // The addition to move to the next pixel along the longest axis
    int errorStep; // Step to take in the "error" along the short axis. When the error goes over the threshold it is corrected by moving along the shorter axis
    int sideStep; // The addition to move to the next pixel along the shorter axis
    if (dx >= dy) {
        steps = dx;
        step = xi;
        errorStep = 2 * dy;
        sideStep = pitch * yi;
    } else {
        steps = dy;
        step = pitch * yi;
        errorStep = 2 * dx;
        sideStep = xi;
    }

    int D = errorStep - steps; 
    for (int _ = 0; _ <= steps; ++_) {
        *pixel = color;
        pixel += step;
        if (D > 0) {
            pixel += sideStep;
            D -= 2*steps;
        }
        D += errorStep;
    }
}

void drawLine(Bitmap bitmap, Line line, int width, uint32_t color) {
    drawLine(bitmap, line.a, line.b, width, color);
}

float length(Point a, Point b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

float length(Line line) {
    return length(line.a, line.b);
}

float circumference(Triangle triangle) {
    return length(triangle.a, triangle.b) + length(triangle.a, triangle.c) + length(triangle.b, triangle.c);
}

void drawTriangle(Bitmap bitmap, Triangle triangle, int width, uint32_t color) {
    drawLine(bitmap, triangle.a, triangle.b, width, color);
    drawLine(bitmap, triangle.a, triangle.c, width, color);
    drawLine(bitmap, triangle.b, triangle.c, width, color);
}

void moveTriangleToLine(Triangle &triangle, Line line) {
    int newX = 0; 
    int newY = 0;
    Point newC(newX, newY);
    triangle.c = newC;
    triangle.a = line.a;
    triangle.b = line.b;
}

void drawTriangleFracRec(Bitmap bitmap, Triangle triangle, int width, uint32_t color) {
    float_t threshold = 10;
    if (circumference(triangle) < threshold) return;
}

void drawTriangleFractal(Bitmap bitmap, Triangle triangle, int width, uint32_t color) {
    drawTriangle(bitmap, triangle, width, color);

    Triangle tab(triangle);
    Line ab(triangle.a, triangle.b);
    moveTriangleToLine(tab, ab);
    drawTriangle(bitmap, tab, width, color);
    
    Triangle tac(triangle);
    Line ac(triangle.a, triangle.c);
    moveTriangleToLine(tac, ac);
    drawTriangle(bitmap, tac, width, color);

    Triangle tbc(triangle);
    Line bc(triangle.b, triangle.c);
    moveTriangleToLine(tac, bc);
    drawTriangle(bitmap, tbc, width, color);
}

float_t radToDegree(float_t rad) {
    return (rad / PI) * 180.0f;
}

float_t degreeToRad(float_t degree) {
    return (degree / 180.0f) * PI;
}

Point rotateAroundRad(Point p, float_t rad, Point c) {
    // Transform to 0 space
    float_t x = p.x - c.x;
    float_t y = p.y - c.y;
    // Calc degrees
    float_t rc = cosf(rad);
    float_t rs = -sinf(rad);
    // Do fancy quantum rotation
    return Point(c.x + (x * rc - y * rs), c.y + (y * rc + x * rs));
}

Point rotateAroundDeg(Point p, float_t degree, Point c) {
    return rotateAroundRad(p, degreeToRad(degree), c);
}

void drawLineFractal(Bitmap bitmap, Line line, int width, uint32_t color) {
    float_t threshold = 2.0f;
    if (length(line) < threshold) return;
    drawLine(bitmap, line, 1, color);

    int dx = line.a.x - line.b.x;
    int dy = line.a.y - line.b.y;
    Point newEnd(line.b.x + (dx / 2), line.b.y + (dy / 2));
    Line newLine(line.b, rotateAroundDeg(newEnd, -90.0, line.b));
    // drawLine(bitmap, newLine, 1, color);
    drawLineFractal(bitmap, newLine, width, color);
}

float_t lagrange_term(float_t t, Point p, Point p1, Point p2, Point p3) {
    return (float_t)((t - p1.x) * (t - p2.x) * (t - p3.x) * (float_t) p.y) / (float_t)((p.x - p1.x) * (p.x - p2.x) * (p.x - p3.x));
}

void drawCubicPolynomialFromPoints(Bitmap bitmap, Point p0, Point p1, Point p2, Point p3, uint32_t steps) {
    // First we do Lagrange's interpolation to get the function
    int dx = p3.x - p0.x;

    int x = p0.x;
    int y = p0.y;
    for (size_t i = 0; i < steps; i++)
    {
        float_t t = i / (steps-1);
        int x_p = x + dx * t;

        float_t ft = lagrange_term(t, p0, p1, p2, p3) + lagrange_term(t, p1, p0, p2, p3) + lagrange_term(t, p2, p0, p1, p3) + lagrange_term(t, p3, p0, p1, p2);

        int y_p = ft;

        drawLine(bitmap, Line(Point(x, y), Point(x_p, y_p)), 1, 0xFFFFFFFF);
    }
    // Then sample the function for "steps" points

    // Then we draw lines between each sample
}

void drawCubicPolynomialFromBlossomPoints(Bitmap bitmap, Point p0, Point p1, Point cp0, Point cp1, uint32_t steps) {
    // Use bernstein polynomials to sample points
    auto b0 = [](float_t t) { return (1 - t) * (1 - t) * (1 - t);};
    auto b1 = [](float_t t) { return 3*t*(1-t)*(1-t);};
    auto b2 = [](float_t t) { return 3*t*t*(1-t);};
    auto b3 = [](float_t t) { return t*t*t; };

    for (size_t i = 0; i < steps; i++)
    {
        float_t t = i / (steps-1);
        float_t b0_t = b0(t);
        float_t b1_t = b1(t);
        float_t b2_t = b2(t);
        float_t b3_t = b3(t);

    }
    
 
}

void drawCubitPolynomialFromHermite(Bitmap bitmap, Point p0, Point p1, Point v0, Point v1, uint32_t steps) {
    // TODO: Do this using some fancy linear algebra multiplication
    auto h0 = [](int t) { return 2*t*t*t - 3*t*t + 1;};
    auto h1 = [](int t) { return -2*t*t*t + 3*t*t;};
    auto h2 = [](int t) { return t*t*t - 2*t*t + t;};
    auto h3 = [](int t) { return t*t*t - t*t;};
    
    // Then sample the function for "steps" points

}
