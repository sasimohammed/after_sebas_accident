#define _USE_MATH_DEFINES
#include "ellipse.h"
#include <cmath>

void drawPoints(HDC hdc, int xc, int yc, int x, int y, COLORREF color)
{
    SetPixel(hdc, xc + x, yc + y, color);
    SetPixel(hdc, xc - x, yc + y, color);
    SetPixel(hdc, xc + x, yc - y, color);
    SetPixel(hdc, xc - x, yc - y, color);
}

void drawEllipseDirect(HDC hdc, int xc, int yc, int a, int b, COLORREF color)
{
    int a2 = a * a;
    int b2 = b * b;

    for (int x = 0; x <= a; x++)
    {
        double check = (double)(x * x) / a2;
        if (check > 1.0) break;
        int y = (int)round(b * sqrt(1.0 - check));
        drawPoints(hdc, xc, yc, x, y, color);
    }

    for (int y = 0; y <= b; y++)
    {
        double check = (double)(y * y) / b2;
        if (check > 1.0) break;
        int x = (int)round(a * sqrt(1.0 - check));
        drawPoints(hdc, xc, yc, x, y, color);
    }
}

void drawEllipsePolar(HDC hdc, int xc, int yc, int a, int b, COLORREF color)
{
    double step = 1.0 / (double)(a > b ? a : b);
    const double PI = 3.14159265358979323846;

    for (double t = 0.0; t <= (PI / 2.0 + step); t += step)
    {
        int x = (int)round(a * cos(t));
        int y = (int)round(b * sin(t));
        drawPoints(hdc, xc, yc, x, y, color);
    }
}

void drawEllipseMidpoint(HDC hdc, int xc, int yc, int a, int b, COLORREF color)
{
    long long a2 = (long long)a * a;
    long long b2 = (long long)b * b;

    int x = 0;
    int y = b;
    long long d1 = b2 - a2 * b + (a2 / 4);
    long long dx = 2 * b2 * x;
    long long dy = 2 * a2 * y;

    while (dx < dy)
    {
        drawPoints(hdc, xc, yc, x, y, color);
        if (d1 < 0)
        {
            x++;
            dx += 2 * b2;
            d1 += dx + b2;
        }
        else
        {
            x++;
            y--;
            dx += 2 * b2;
            dy -= 2 * a2;
            d1 += dx - dy + b2;
        }
    }

    long long d2 = b2 * ((long long)x * x + x) + a2 * ((long long)(y - 1) * (y - 1)) - a2 * b2 + (b2 / 4);
    dx = 2 * b2 * x;
    dy = 2 * a2 * y;

    while (y >= 0)
    {
        drawPoints(hdc, xc, yc, x, y, color);
        if (d2 > 0)
        {
            y--;
            dy -= 2 * a2;
            d2 += a2 - dy;
        }
        else
        {
            x++;
            y--;
            dx += 2 * b2;
            dy -= 2 * a2;
            d2 += dx - dy + a2;
        }
    }
}