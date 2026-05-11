// lines.cpp
#include <windows.h>   
#include <cmath>       
#include "shape.h"
#include "lines.h"

using namespace std;

void DrawLine_DDA(HDC hdc, Point p1, Point p2, COLORREF color)
{
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;
    
    int dx = x2 - x1;
    int dy = y2 - y1;

    SetPixel(hdc, x1, y1, color);

    if (abs(dx) >= abs(dy))
    {
        int x = x1, xinc = dx > 0 ? 1 : -1;
        double y = y1, yinc = (double)dy / dx * xinc;
        while (x != x2)
        {
            x += xinc;
            y += yinc;
            SetPixel(hdc, x, (int)round(y), color);
        }
    }
    else
    {
        int y = y1, yinc = dy > 0 ? 1 : -1;
        double x = x1, xinc = (double)dx / dy * yinc;
        while (y != y2)
        {
            x += xinc;
            y += yinc;
            SetPixel(hdc, (int)round(x), y, color);
        }
    }
}

void DrawLine_Midpoint(HDC hdc, Point p1, Point p2, COLORREF color)
{
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;
    
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    if (dy <= dx)                              
    {
        if (x1 > x2) { 
            swap(x1, x2); 
            swap(y1, y2); 
        }
        dx = x2 - x1;  dy = y2 - y1;

        int d = 2*dy - dx;
        int change1 = 2*dy - 2*dx;
        int change2 = 2*dy;

        int x = x1, y = y1;
        SetPixel(hdc, x, y, color);

        while (x < x2)
        {
            if (d < 0)
                d += change2;
            else
            {
                d += change1;
                y++;
            }
            x++;
            SetPixel(hdc, x, y, color);
        }
    }
    else
    {
        if (y1 > y2) { 
            swap(x1, x2); 
            swap(y1, y2); 
        }
        dx = x2 - x1;  dy = y2 - y1;

        int d = 2*dx - dy;
        int change1 = 2*dx - 2*dy;
        int change2 = 2*dx;

        int x = x1, y = y1;
        SetPixel(hdc, x, y, color);

        while (y < y2)
        {
            if (d > 0)
            {
                d += change1;
                x++;
            }
            else
                d += change2;
            y++;
            SetPixel(hdc, x, y, color);
        }
    }
}

void DrawLine_Parametric(HDC hdc, Point p1, Point p2, COLORREF color)
{
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;
    
    int dx = x2 - x1;
    int dy = y2 - y1;

    int N = max(abs(dx), abs(dy));

    if (N == 0)
    {
        SetPixel(hdc, x1, y1, color);
        return;
    }

    for (int i = 0; i <= N; i++)
    {
        double t = (double)i / N;
        int x = (int)round(x1 + t * dx);
        int y = (int)round(y1 + t * dy);
        SetPixel(hdc, x, y, color);
    }
}