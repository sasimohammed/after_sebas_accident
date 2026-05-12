// lines.cpp
#include <windows.h>   
#include <cmath>       
#include "shape.h"
#include "circles.h"

using namespace std;

void Draw8Points(HDC hdc,int xc,int yc, int a, int b,COLORREF color)
{
SetPixel(hdc, xc+a, yc+b, color);
SetPixel(hdc, xc-a, yc+b, color);
SetPixel(hdc, xc-a, yc-b, color);
SetPixel(hdc, xc+a, yc-b, color);
SetPixel(hdc, xc+b, yc+a, color);
SetPixel(hdc, xc-b, yc+a, color);
SetPixel(hdc, xc-b, yc-a, color);
SetPixel(hdc, xc+b, yc-a, color);
}

void CircleDirect(HDC hdc, int xc, int yc, int R, COLORREF color)

{
    int x = 0, y = R;
    int R2 = R * R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y)
    {
        x++;
        y = round(sqrt((double)(R2 - x * x)));
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

void CirclePolar(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    int x = R, y = 0;
    double theta = 0, dtheta = 1.0 / R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x > y)
    {
        theta += dtheta;
        x = round(R * cos(theta));
        y = round(R * sin(theta));
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

void CircleIterativePolar(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    double x = R, y = 0;
    double dtheta = 1.0 / R;
    double cdtheta = cos(dtheta), sdtheta = sin(dtheta);
    Draw8Points(hdc, xc, yc, R, 0, color);
    while (x > y)
    {
        double x1 = x * cdtheta - y * sdtheta;
        y = x * sdtheta + y * cdtheta;
        x = x1;
        Draw8Points(hdc, xc, yc, round(x), round(y), color);
    }
}

void CircleMidPoint(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    int x = 0, y = R;
    int d = 1 - R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y)
    {
        if (d < 0)
            d += 2 * x + 2;
        else
        {

            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

void CircleModifiedMidPoint(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    int x = 0, y = R;
    int d = 1 - R;
    int c1 = 3, c2 = 5 - 2 * R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y)
    {
        if (d < 0)
        {
            d += c1;
            c2 += 2;
        }
        else
        {

            d += c2;
            c2 += 4;
            y--;
        }
        c1 += 2;
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}