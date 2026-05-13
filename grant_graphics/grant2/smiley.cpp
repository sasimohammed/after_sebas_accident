#include "smiley.h"
#include <cmath>

void DrawCirclePoints(HDC hdc,
    int xc, int yc,
    int x, int y)
{
    SetPixel(hdc, xc + x, yc + y, RGB(0, 0, 0));
    SetPixel(hdc, xc - x, yc + y, RGB(0, 0, 0));
    SetPixel(hdc, xc + x, yc - y, RGB(0, 0, 0));
    SetPixel(hdc, xc - x, yc - y, RGB(0, 0, 0));

    SetPixel(hdc, xc + y, yc + x, RGB(0, 0, 0));
    SetPixel(hdc, xc - y, yc + x, RGB(0, 0, 0));
    SetPixel(hdc, xc + y, yc - x, RGB(0, 0, 0));
    SetPixel(hdc, xc - y, yc - x, RGB(0, 0, 0));
}

void DrawCircle(HDC hdc,
    int xc, int yc,
    int r)
{
    int x = 0;
    int y = r;

    int d = 1 - r;

    DrawCirclePoints(hdc, xc, yc, x, y);

    while (x < y)
    {
        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }

        x++;

        DrawCirclePoints(hdc, xc, yc, x, y);
    }
}

void DrawLine(HDC hdc,
    int x1, int y1,
    int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = max(abs(dx), abs(dy));

    double xInc = (double)dx / steps;
    double yInc = (double)dy / steps;

    double x = x1;
    double y = y1;

    for (int i = 0; i <= steps; i++)
    {
        SetPixel(hdc,
            round(x),
            round(y),
            RGB(0, 0, 0));

        x += xInc;
        y += yInc;
    }
}

void DrawHappyFace(HDC hdc,
    int xc, int yc,
    int r)
{
    // Face
    DrawCircle(hdc, xc, yc, r);

    // Eyes
    DrawCircle(hdc,
        xc - r / 3,
        yc - r / 3,
        r / 10);

    DrawCircle(hdc,
        xc + r / 3,
        yc - r / 3,
        r / 10);

    // Nose
    DrawLine(hdc,
        xc, yc - 10,
        xc, yc + 15);

    // Smile
    for (double theta = 0.2; theta < 2.9; theta += 0.01)
    {
        int x = xc + 35 * cos(theta);
        int y = yc + 20 * sin(theta) + 25;

        if (y > yc)
            SetPixel(hdc, x, y, RGB(0, 0, 0));
    }
}

void DrawSadFace(HDC hdc,
    int xc, int yc,
    int r)
{
    // Face
    DrawCircle(hdc, xc, yc, r);

    // Eyes
    DrawCircle(hdc,
        xc - r / 3,
        yc - r / 3,
        r / 10);

    DrawCircle(hdc,
        xc + r / 3,
        yc - r / 3,
        r / 10);

    // Nose
    DrawLine(hdc,
        xc, yc - 10,
        xc, yc + 15);

    // Sad mouth
    for (double theta = 0.2; theta < 2.9; theta += 0.01)
    {
        int x = xc + 35 * cos(theta);
        int y = yc - 20 * sin(theta) + 55;

        if (y > yc)
            SetPixel(hdc, x, y, RGB(0, 0, 0));
    }
}