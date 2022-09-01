#ifndef __RENDERER__

#include <stdint.h>

struct Bitmap {
    void *memory;
    int height;
    int width;
};

struct Point
{
    float x;
    float y;

    Point(float x, float y) : x(x), y(y){}
};

struct Line
{
    Point a;
    Point b;

    Line(float x0, float y0, float x1, float y1) : a(Point(x0, y0)), b(Point(x1, y1)) {}
    Line(Point p0, Point p1) : a(p0), b(p1) {}
};

struct Circle
{
    Point center;
    float radius;
};

struct Triangle
{
    Point a;
    Point b;
    Point c;

    Triangle(Point a, Point b, Point c): a(a), b(b), c(c) {}
};

struct Rectangle
{
    Point topLeft;
    Point bottomLeft;
};

void drawDot(Bitmap bitmap, Point position, float radius, uint32_t color);
void drawLine(Bitmap bitmap, Point start, Point end, float width, uint32_t color);
void drawLine(Bitmap bitmap, Line line, float width, uint32_t color);
void fillRect(Bitmap bitmap, Point topLeft, Point bottomRight, uint32_t color);
void fillRect(Bitmap bitmap, float minX, float minY, float maxX, float maxY, uint32_t color);
void drawRect(Bitmap bitmap, Point topLeft, Point bottomRight, float width, uint32_t color);
void drawRect(Bitmap bitmap, float minX, float minY, float maxX, float maxY, float width, uint32_t color);
void drawTriangle(Bitmap bitmap, Triangle triangle, float width, uint32_t color);
void drawTriangleFracRec(Bitmap bitmap, Triangle triangle, float width, uint32_t color);
void drawTriangleFractal(Bitmap bitmap, Triangle triangle, float width, uint32_t color);
void drawLineFractal(Bitmap bitmap, Line line, float width, uint32_t color);
void drawCubicPolynomialFromPoints(Bitmap bitmap, Point p0, Point p1, Point p2, Point p3, uint32_t steps);
void drawCubicPolynomialFromBlossomPoints(Bitmap bitmap, Point p0, Point p1, Point cp0, Point cp1, uint32_t steps);
void drawCubitPolynomialFromHermite(Bitmap bitmap, Point p0, Point p1, Point v0, Point v1, uint32_t steps);

#define __RENDERER__
#endif