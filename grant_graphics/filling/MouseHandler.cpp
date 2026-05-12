#include "MouseHandler.h"
#include "Globals.h"
#include "Filling.h"
#include "Clipping.h"
#include "core_data.h"
#include <cmath>
#include <vector>

using namespace std;

extern CoreData core;
extern void RedrawAll();

static void recordPoint(int mx, int my)
{
    if (g_clickCount >= 0 && g_clickCount < 20) {
        g_pts[g_clickCount][0] = mx;
        g_pts[g_clickCount][1] = my;
    }
    g_clickCount++;
}

static int radius2pts(int x0, int y0, int x1, int y1)
{
    return (int)sqrt(pow(x1 - x0, 2.0) + pow(y1 - y0, 2.0));
}

void DrawClipWindowOutline()
{
    HPEN pen = CreatePen(PS_DASH, 2, C_CLIPWIN);
    HPEN old = (HPEN)SelectObject(g_hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(g_hdc, GetStockObject(NULL_BRUSH));

    Rectangle(g_hdc, g_clipLeft, g_clipTop, g_clipRight, g_clipBottom);

    SelectObject(g_hdc, oldBrush);
    SelectObject(g_hdc, old);
    DeleteObject(pen);
}

void OnLeftClick(int mx, int my)
{
    recordPoint(mx, my);

    if (g_menu == 7)
    {
        // Circle with Lines
        if (g_sub == 1 && g_clickCount == 2) {
            int cx = g_pts[0][0], cy = g_pts[0][1];
            int r = radius2pts(cx, cy, mx, my);
            FillCircleWithLines(cx, cy, r, g_quarter);
            g_clickCount = 0;
        }
        // Circle with Circles
        else if (g_sub == 2 && g_clickCount == 2) {
            int cx = g_pts[0][0], cy = g_pts[0][1];
            int r = radius2pts(cx, cy, mx, my);
            FillCircleWithCircles(cx, cy, r, g_quarter);
            g_clickCount = 0;
        }
        // Square Hermit
        else if (g_sub == 3 && g_clickCount == 2) {
            int left = min(g_pts[0][0], mx);
            int top = min(g_pts[0][1], my);
            int side = min(abs(mx - g_pts[0][0]), abs(my - g_pts[0][1]));
            FillSquareWithHermit(left, top, left + side, top + side);
            g_clickCount = 0;
        }
        // Rectangle Bezier
        else if (g_sub == 4 && g_clickCount == 2) {
            int left = min(g_pts[0][0], mx);
            int top = min(g_pts[0][1], my);
            int right = max(g_pts[0][0], mx);
            int bottom = max(g_pts[0][1], my);
            FillRectWithBezier(left, top, right, bottom);
            g_clickCount = 0;
        }
        // Polygon fill - accumulate vertices
        else if (g_sub == 5) {
            POINT p = { mx, my };
            g_poly.push_back(p);
            SetPixel(g_hdc, mx, my, C_BOUND);
            if (g_poly.size() > 1) {
                int n = (int)g_poly.size();
                MoveToEx(g_hdc, g_poly[n - 2].x, g_poly[n - 2].y, NULL);
                LineTo(g_hdc, mx, my);
            }
        }
        // Flood fill - accumulate vertices
        else if (g_sub == 6) {
            POINT p = { mx, my };
            g_floodPoly.push_back(p);
            SetPixel(g_hdc, mx, my, C_BOUND);
            if (g_floodPoly.size() > 1) {
                int n = (int)g_floodPoly.size();
                MoveToEx(g_hdc, g_floodPoly[n - 2].x, g_floodPoly[n - 2].y, NULL);
                LineTo(g_hdc, mx, my);
            }
        }
    }
}

void OnRightClick(int mx, int my)
{
    // Polygon fill (Convex/Non-Convex)
    if (g_menu == 7 && g_sub == 5 && g_poly.size() >= 3)
    {
        MoveToEx(g_hdc, g_poly.back().x, g_poly.back().y, NULL);
        LineTo(g_hdc, g_poly.front().x, g_poly.front().y);

        POINT* arr = g_poly.data();
        int n = (int)g_poly.size();
        if (g_quarter == 1) ConvexFill(arr, n, C_FILL);
        else GeneralPolygonFill(arr, n, C_FILL);

        g_poly.clear();
        g_clickCount = 0;
        return;
    }

    // Flood fill
    if (g_menu == 7 && g_sub == 6 && g_floodPoly.size() >= 3)
    {
        MoveToEx(g_hdc, g_floodPoly.back().x, g_floodPoly.back().y, NULL);
        LineTo(g_hdc, g_floodPoly.front().x, g_floodPoly.front().y);

        if (g_quarter == 1)
            FloodFillPolygon(g_floodPoly, C_FLOOD);
        else
            FloodFillNonRecursive(g_floodPoly[0].x, g_floodPoly[0].y, C_BOUND, C_FLOOD);

        g_floodPoly.clear();
        g_clickCount = 0;
        return;
    }
}