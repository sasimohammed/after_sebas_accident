#include "circle_clipping.h"
#include <cmath>
#include <algorithm>

// ================= POINT TEST =================
bool PointInsideCircle(int xc, int yc, int r, int x, int y)
{
    int dx = x - xc;
    int dy = y - yc;
    return (dx * dx + dy * dy <= r * r);
}

// ================= POINT CLIPPING =================
void CirclePointClipping(HDC hdc,
    int xc, int yc, int r,
    int x, int y)
{
    if (PointInsideCircle(xc, yc, r, x, y))
    {
        HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));

        HPEN oldPen = (HPEN)SelectObject(hdc, redPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, redBrush);

        Ellipse(hdc, x - 5, y - 5, x + 5, y + 5);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);

        DeleteObject(redPen);
        DeleteObject(redBrush);
    }
}




bool SolveQuadratic(double A, double B, double C, double& t1, double& t2)
{
    double D = B * B - 4 * A * C;
    if (D < 0) return false;

    double sqrtD = sqrt(D);

    t1 = (-B - sqrtD) / (2 * A);
    t2 = (-B + sqrtD) / (2 * A);

    if (t1 > t2)
        std::swap(t1, t2);

    return true;
}


void GetPoint(double x1, double y1, double dx, double dy, double t,
    int& x, int& y)
{
    x = (int)(x1 + t * dx);
    y = (int)(y1 + t * dy);
}

void CircleLineClipping(HDC hdc,
    int xc, int yc, int r,
    int x1, int y1,
    int x2, int y2)
{
   
    double dx = x2 - x1;
    double dy = y2 - y1;

    
    double fx = x1 - xc;
    double fy = y1 - yc;

    
    double A = dx * dx + dy * dy;
    double B = 2 * (fx * dx + fy * dy);
    double C = fx * fx + fy * fy - r * r;

    double t1, t2;

 
    if (!SolveQuadratic(A, B, C, t1, t2))
        return;

  
    t1 = max(0.0, t1);
    t2 = min(1.0, t2);

    if (t1 > t2)
        return;

    int sx, sy, ex, ey;

    GetPoint(x1, y1, dx, dy, t1, sx, sy);
    GetPoint(x1, y1, dx, dy, t2, ex, ey);

    MoveToEx(hdc, sx, sy, NULL);
    LineTo(hdc, ex, ey);
}

