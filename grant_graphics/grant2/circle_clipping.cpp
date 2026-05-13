#include "circle_clipping.h"
#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;

// ================= CIRCLE CLIPPING =================
bool PointInsideCircle(int xc, int yc, int r, int x, int y)
{
    int dx = x - xc;
    int dy = y - yc;
    return (dx * dx + dy * dy <= r * r);
}

void CirclePointClipping(HDC hdc, int xc, int yc, int r, int x, int y)
{
    if (PointInsideCircle(xc, yc, r, x, y))
    {
        // Draw a big RED dot (8x8 pixel)
        HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));

        HPEN oldPen = (HPEN)SelectObject(hdc, redPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, redBrush);

        Ellipse(hdc, x - 6, y - 6, x + 6, y + 6);

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

void GetPoint(double x1, double y1, double dx, double dy, double t, int& x, int& y)
{
    x = (int)(x1 + t * dx);
    y = (int)(y1 + t * dy);
}

void CircleLineClipping(HDC hdc, int xc, int yc, int r, int x1, int y1, int x2, int y2)
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

// ================= RECTANGLE CLIPPING =================
bool PointInsideRect(int left, int top, int right, int bottom, int x, int y)
{
    return (x >= left && x <= right && y >= top && y <= bottom);
}

void RectPointClipping(HDC hdc, int left, int top, int right, int bottom, int x, int y)
{
    if (PointInsideRect(left, top, right, bottom, x, y))
    {
        // Draw a big BLUE dot (8x8 pixel)
        HPEN bluePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));

        HPEN oldPen = (HPEN)SelectObject(hdc, bluePen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, blueBrush);

        Ellipse(hdc, x - 6, y - 6, x + 6, y + 6);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);

        DeleteObject(bluePen);
        DeleteObject(blueBrush);
    }
}

// Liang-Barsky line clipping algorithm for rectangle
void RectLineClipping(HDC hdc, int left, int top, int right, int bottom, int x1, int y1, int x2, int y2)
{
    double t0 = 0.0, t1 = 1.0;
    double dx = x2 - x1;
    double dy = y2 - y1;

    double p[4], q[4];
    p[0] = -dx;  q[0] = x1 - left;
    p[1] = dx;  q[1] = right - x1;
    p[2] = -dy;  q[2] = y1 - top;
    p[3] = dy;  q[3] = bottom - y1;

    for (int i = 0; i < 4; i++)
    {
        if (p[i] == 0)
        {
            if (q[i] < 0) return;
        }
        else
        {
            double t = q[i] / p[i];
            if (p[i] < 0)
            {
                if (t > t0) t0 = t;
            }
            else
            {
                if (t < t1) t1 = t;
            }
        }
    }

    if (t0 <= t1)
    {
        int sx = (int)(x1 + t0 * dx);
        int sy = (int)(y1 + t0 * dy);
        int ex = (int)(x1 + t1 * dx);
        int ey = (int)(y1 + t1 * dy);

        MoveToEx(hdc, sx, sy, NULL);
        LineTo(hdc, ex, ey);
    }
}

// Sutherland-Hodgman polygon clipping for rectangle
void RectPolygonClipping(HDC hdc, int left, int top, int right, int bottom, int x1, int y1, int x2, int y2, int x3, int y3)
{
    vector<POINT> input;
    vector<POINT> output;

    // Start with the triangle vertices
    input.push_back({ x1, y1 });
    input.push_back({ x2, y2 });
    input.push_back({ x3, y3 });

    // Clip against left edge
    output.clear();
    for (size_t i = 0; i < input.size(); i++)
    {
        POINT curr = input[i];
        POINT prev = input[(i + input.size() - 1) % input.size()];

        bool currInside = curr.x >= left;
        bool prevInside = prev.x >= left;

        if (currInside)
        {
            if (!prevInside)
            {
                int x = left;
                int y = prev.y + (curr.y - prev.y) * (left - prev.x) / (curr.x - prev.x);
                output.push_back({ x, y });
            }
            output.push_back(curr);
        }
        else if (prevInside)
        {
            int x = left;
            int y = prev.y + (curr.y - prev.y) * (left - prev.x) / (curr.x - prev.x);
            output.push_back({ x, y });
        }
    }
    input = output;

    // Clip against right edge
    output.clear();
    for (size_t i = 0; i < input.size(); i++)
    {
        POINT curr = input[i];
        POINT prev = input[(i + input.size() - 1) % input.size()];

        bool currInside = curr.x <= right;
        bool prevInside = prev.x <= right;

        if (currInside)
        {
            if (!prevInside)
            {
                int x = right;
                int y = prev.y + (curr.y - prev.y) * (right - prev.x) / (curr.x - prev.x);
                output.push_back({ x, y });
            }
            output.push_back(curr);
        }
        else if (prevInside)
        {
            int x = right;
            int y = prev.y + (curr.y - prev.y) * (right - prev.x) / (curr.x - prev.x);
            output.push_back({ x, y });
        }
    }
    input = output;

    // Clip against top edge
    output.clear();
    for (size_t i = 0; i < input.size(); i++)
    {
        POINT curr = input[i];
        POINT prev = input[(i + input.size() - 1) % input.size()];

        bool currInside = curr.y >= top;
        bool prevInside = prev.y >= top;

        if (currInside)
        {
            if (!prevInside)
            {
                int y = top;
                int x = prev.x + (curr.x - prev.x) * (top - prev.y) / (curr.y - prev.y);
                output.push_back({ x, y });
            }
            output.push_back(curr);
        }
        else if (prevInside)
        {
            int y = top;
            int x = prev.x + (curr.x - prev.x) * (top - prev.y) / (curr.y - prev.y);
            output.push_back({ x, y });
        }
    }
    input = output;

    // Clip against bottom edge
    output.clear();
    for (size_t i = 0; i < input.size(); i++)
    {
        POINT curr = input[i];
        POINT prev = input[(i + input.size() - 1) % input.size()];

        bool currInside = curr.y <= bottom;
        bool prevInside = prev.y <= bottom;

        if (currInside)
        {
            if (!prevInside)
            {
                int y = bottom;
                int x = prev.x + (curr.x - prev.x) * (bottom - prev.y) / (curr.y - prev.y);
                output.push_back({ x, y });
            }
            output.push_back(curr);
        }
        else if (prevInside)
        {
            int y = bottom;
            int x = prev.x + (curr.x - prev.x) * (bottom - prev.y) / (curr.y - prev.y);
            output.push_back({ x, y });
        }
    }

    // Draw the clipped polygon
    if (output.size() >= 3)
    {
        HPEN orangePen = CreatePen(PS_SOLID, 2, RGB(255, 128, 0));
        HBRUSH orangeBrush = CreateSolidBrush(RGB(255, 128, 0));

        HPEN oldPen = (HPEN)SelectObject(hdc, orangePen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, orangeBrush);

        POINT* pts = output.data();
        Polygon(hdc, pts, (int)output.size());

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(orangePen);
        DeleteObject(orangeBrush);
    }
}

// ================= SQUARE CLIPPING =================
bool PointInsideSquare(int cx, int cy, int side, int x, int y)
{
    int half = side / 2;
    int left = cx - half;
    int right = cx + half;
    int top = cy - half;
    int bottom = cy + half;
    return PointInsideRect(left, top, right, bottom, x, y);
}

void SquarePointClipping(HDC hdc, int cx, int cy, int side, int x, int y)
{
    int half = side / 2;
    int left = cx - half;
    int right = cx + half;
    int top = cy - half;
    int bottom = cy + half;

    if (PointInsideRect(left, top, right, bottom, x, y))
    {
        // Draw a big GREEN dot (8x8 pixel)
        HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));

        HPEN oldPen = (HPEN)SelectObject(hdc, greenPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, greenBrush);

        Ellipse(hdc, x - 6, y - 6, x + 6, y + 6);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);

        DeleteObject(greenPen);
        DeleteObject(greenBrush);
    }
}

void SquareLineClipping(HDC hdc, int cx, int cy, int side, int x1, int y1, int x2, int y2)
{
    int half = side / 2;
    int left = cx - half;
    int right = cx + half;
    int top = cy - half;
    int bottom = cy + half;

    RectLineClipping(hdc, left, top, right, bottom, x1, y1, x2, y2);
}