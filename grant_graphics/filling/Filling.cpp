#include "Filling.h"
#include "Globals.h"
#include <cmath>
#include <stack>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;

#ifndef PI
#define PI 3.14159265358979323846
#endif

// ═══════════════════════════════════════════════
// 7a Circle Fill (Lines)
// ═══════════════════════════════════════════════

void FillCircleWithLines(int cx, int cy, int r, int quarter)
{
    if (!g_hdc) return;

    double a1, a2;

    switch (quarter)
    {
    case 1: a1 = 0; a2 = PI / 2; break;
    case 2: a1 = PI / 2; a2 = PI; break;
    case 3: a1 = PI; a2 = 3 * PI / 2; break;
    default: a1 = 3 * PI / 2; a2 = 2 * PI; break;
    }

    HPEN pen = CreatePen(PS_SOLID, 1, C_FILL);
    HPEN oldPen = (HPEN)SelectObject(g_hdc, pen);

    for (double a = a1; a <= a2; a += 0.01)
    {
        int x = cx + (int)(r * cos(a));
        int y = cy - (int)(r * sin(a));

        MoveToEx(g_hdc, x, y, NULL);
        LineTo(g_hdc, cx, cy);
    }

    SelectObject(g_hdc, oldPen);
    DeleteObject(pen);
}

// ═══════════════════════════════════════════════
// 7b Circle Fill (Circles)
// ═══════════════════════════════════════════════

void FillCircleWithCircles(int cx, int cy, int r, int quarter)
{
    if (!g_hdc) return;

    HPEN pen = CreatePen(PS_SOLID, 1, C_FILL);
    HPEN oldPen = (HPEN)SelectObject(g_hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(g_hdc, GetStockObject(NULL_BRUSH));

    for (int rr = 5; rr <= r; rr += 8)
    {
        int left = cx - rr;
        int top = cy - rr;
        int right = cx + rr;
        int bottom = cy + rr;

        HRGN clipRegion = NULL;

        switch (quarter)
        {
        case 1:
            clipRegion = CreateRectRgn(cx, cy - r, cx + r, cy);
            break;
        case 2:
            clipRegion = CreateRectRgn(cx - r, cy - r, cx, cy);
            break;
        case 3:
            clipRegion = CreateRectRgn(cx - r, cy, cx, cy + r);
            break;
        case 4:
            clipRegion = CreateRectRgn(cx, cy, cx + r, cy + r);
            break;
        default:
            clipRegion = CreateRectRgn(0, 0, 2000, 2000);
            break;
        }

        SelectClipRgn(g_hdc, clipRegion);
        Ellipse(g_hdc, left, top, right, bottom);
        SelectClipRgn(g_hdc, NULL);

        if (clipRegion)
            DeleteObject(clipRegion);
    }

    SelectObject(g_hdc, oldBrush);
    SelectObject(g_hdc, oldPen);
    DeleteObject(pen);
}

// ═══════════════════════════════════════════════
// 7c Hermit Square Fill
// ═══════════════════════════════════════════════

static void DrawHermit(HDC hdc,
    int x0, int y0, int x1, int y1,
    int tx0, int ty0, int tx1, int ty1,
    COLORREF color)
{
    HPEN pen = CreatePen(PS_SOLID, 2, color);
    HPEN old = (HPEN)SelectObject(hdc, pen);

    int prevX = x0, prevY = y0;
    const int N = 120;
    for (int i = 1; i <= N; i++)
    {
        double t = (double)i / N;
        double t2 = t * t, t3 = t2 * t;
        double h00 = 2 * t3 - 3 * t2 + 1;
        double h10 = t3 - 2 * t2 + t;
        double h01 = -2 * t3 + 3 * t2;
        double h11 = t3 - t2;

        int x = (int)(h00 * x0 + h10 * tx0 + h01 * x1 + h11 * tx1);
        int y = (int)(h00 * y0 + h10 * ty0 + h01 * y1 + h11 * ty1);

        MoveToEx(hdc, prevX, prevY, NULL);
        LineTo(hdc, x, y);
        prevX = x; prevY = y;
    }

    SelectObject(hdc, old);
    DeleteObject(pen);
}

void FillSquareWithHermit(int left, int top, int right, int bottom)
{
    if (!g_hdc) return;

    if (left > right) swap(left, right);
    if (top > bottom) swap(top, bottom);

    int height = bottom - top;
    const int spacing = 12;

    for (int x = left; x <= right; x += spacing)
    {
        int sign = ((x - left) / spacing % 2 == 0) ? 1 : -1;
        int tx = sign * height / 2;
        DrawHermit(g_hdc, x, top, x, bottom, tx, 0, -tx, 0, C_FILL);
    }
}

// ═══════════════════════════════════════════════
// 7d Bezier Rectangle Fill
// ═══════════════════════════════════════════════

void DrawBezierCurve(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, COLORREF color)
{
    if (!g_hdc) return;

    for (double t = 0; t <= 1; t += 0.01)
    {
        double t2 = t * t;
        double t3 = t2 * t;
        double mt = 1 - t;
        double mt2 = mt * mt;
        double mt3 = mt2 * mt;

        int x = (int)(mt3 * x1 + 3 * mt2 * t * x2 + 3 * mt * t2 * x3 + t3 * x4);
        int y = (int)(mt3 * y1 + 3 * mt2 * t * y2 + 3 * mt * t2 * y3 + t3 * y4);

        SetPixel(g_hdc, x, y, color);
    }
}

void FillRectWithBezier(int left, int top, int right, int bottom)
{
    if (!g_hdc) return;

    if (left > right) swap(left, right);
    if (top > bottom) swap(top, bottom);

    int width = right - left;
    int height = bottom - top;

    for (int i = 0; i <= 10; i++)
    {
        double t = i / 10.0;

        int x1 = left + (int)(width * t);
        int y1 = top;
        int x2 = left + (int)(width * t * 0.7);
        int y2 = top + (int)(height * 0.3);
        int x3 = left + (int)(width * t * 0.3);
        int y3 = bottom - (int)(height * 0.3);
        int x4 = left + (int)(width * t);
        int y4 = bottom;

        DrawBezierCurve(x1, y1, x2, y2, x3, y3, x4, y4, C_FILL);

        x1 = right - (int)(width * t);
        y1 = top;
        x2 = right - (int)(width * t * 0.7);
        y2 = top + (int)(height * 0.3);
        x3 = right - (int)(width * t * 0.3);
        y3 = bottom - (int)(height * 0.3);
        x4 = right - (int)(width * t);
        y4 = bottom;

        DrawBezierCurve(x1, y1, x2, y2, x3, y3, x4, y4, C_FILL);
    }

    HPEN borderPen = CreatePen(PS_SOLID, 1, C_BOUND);
    HPEN oldPen = (HPEN)SelectObject(g_hdc, borderPen);

    MoveToEx(g_hdc, left, top, NULL);
    LineTo(g_hdc, right, top);
    LineTo(g_hdc, right, bottom);
    LineTo(g_hdc, left, bottom);
    LineTo(g_hdc, left, top);

    SelectObject(g_hdc, oldPen);
    DeleteObject(borderPen);
}

// ═══════════════════════════════════════════════
// 7e CONVEX FILL
// ═══════════════════════════════════════════════

struct Entry
{
    int xmin, xmax;
};

void InitEntries(Entry table[])
{
    for (int i = 0; i < MAXENTRIES; i++)
    {
        table[i].xmin = INT_MAX;
        table[i].xmax = -INT_MAX;
    }
}

void ScanEdge(POINT v1, POINT v2, Entry table[])
{
    if (v1.y == v2.y) return;

    if (v1.y > v2.y) swap(v1, v2);

    double m = (double)(v2.x - v1.x) / (v2.y - v1.y);
    double x = v1.x;

    for (int y = v1.y; y < v2.y; y++, x += m)
    {
        if (y < 0 || y >= MAXENTRIES) continue;

        int xi = (int)x;

        table[y].xmin = min(table[y].xmin, xi);
        table[y].xmax = max(table[y].xmax, xi);
    }
}

void ConvexFill(POINT p[], int n, COLORREF c)
{
    if (!g_hdc || n < 3) return;

    Entry* table = new Entry[MAXENTRIES];
    InitEntries(table);

    POINT v = p[n - 1];

    for (int i = 0; i < n; i++)
    {
        ScanEdge(v, p[i], table);
        v = p[i];
    }

    for (int y = 0; y < MAXENTRIES; y++)
    {
        if (table[y].xmin <= table[y].xmax)
        {
            for (int x = table[y].xmin; x <= table[y].xmax; x++)
            {
                SetPixel(g_hdc, x, y, c);
            }
        }
    }

    delete[] table;
}

// ═══════════════════════════════════════════════
// 7e NON-CONVEX FILL
// ═══════════════════════════════════════════════

struct EdgeRec
{
    double x, dx;
    int ymax;
};

void GeneralPolygonFill(POINT p[], int n, COLORREF c)
{
    if (!g_hdc || n < 3) return;

    vector<vector<EdgeRec>> table(MAXENTRIES);

    POINT v1 = p[n - 1];

    for (int i = 0; i < n; i++)
    {
        POINT v2 = p[i];

        if (v1.y != v2.y)
        {
            if (v1.y > v2.y) swap(v1, v2);

            EdgeRec e;
            e.x = v1.x;
            e.dx = (double)(v2.x - v1.x) / (v2.y - v1.y);
            e.ymax = v2.y;

            table[v1.y].push_back(e);
        }

        v1 = p[i];
    }

    vector<EdgeRec> active;

    for (int y = 0; y < MAXENTRIES; y++)
    {
        for (auto& e : table[y])
            active.push_back(e);

        active.erase(remove_if(active.begin(), active.end(),
            [y](EdgeRec e) { return e.ymax <= y; }),
            active.end());

        sort(active.begin(), active.end(),
            [](EdgeRec a, EdgeRec b) { return a.x < b.x; });

        for (size_t i = 0; i + 1 < active.size(); i += 2)
        {
            int x1 = (int)active[i].x;
            int x2 = (int)active[i + 1].x;

            for (int x = x1; x <= x2; x++)
                SetPixel(g_hdc, x, y, c);
        }

        for (auto& e : active)
            e.x += e.dx;
    }
}

// ═══════════════════════════════════════════════
// 7f FLOOD FILL
// ═══════════════════════════════════════════════

void DrawSolidBoundary(const vector<POINT>& poly)
{
    if (poly.size() < 3) return;

    HPEN boundPen = CreatePen(PS_SOLID, 1, C_BOUND);
    HPEN oldPen = (HPEN)SelectObject(g_hdc, boundPen);

    for (size_t i = 0; i < poly.size(); i++)
    {
        const POINT& p1 = poly[i];
        const POINT& p2 = poly[(i + 1) % poly.size()];

        MoveToEx(g_hdc, p1.x, p1.y, NULL);
        LineTo(g_hdc, p2.x, p2.y);
    }

    SelectObject(g_hdc, oldPen);
    DeleteObject(boundPen);
}

void FloodFillRecursive(int x, int y, COLORREF boundaryColor, COLORREF fillColor)
{
    if (x < 0 || y < 0 || x > 2000 || y > 2000 || !g_hdc)
        return;

    COLORREF current = GetPixel(g_hdc, x, y);

    if (current == boundaryColor || current == fillColor)
        return;

    SetPixel(g_hdc, x, y, fillColor);

    FloodFillRecursive(x + 1, y, boundaryColor, fillColor);
    FloodFillRecursive(x - 1, y, boundaryColor, fillColor);
    FloodFillRecursive(x, y + 1, boundaryColor, fillColor);
    FloodFillRecursive(x, y - 1, boundaryColor, fillColor);
}

void FloodFillPolygon(const vector<POINT>& poly, COLORREF fillColor)
{
    if (poly.size() < 3) return;

    DrawSolidBoundary(poly);

    int sumX = 0, sumY = 0;
    for (const auto& p : poly)
    {
        sumX += p.x;
        sumY += p.y;
    }
    int seedX = sumX / (int)poly.size();
    int seedY = sumY / (int)poly.size();

    FloodFillRecursive(seedX, seedY, C_BOUND, fillColor);
}

void FloodFillNonRecursive(int x, int y, COLORREF boundaryColor, COLORREF fillColor)
{
    if (!g_hdc) return;

    stack<pair<int, int>> st;
    st.push({ x, y });

    while (!st.empty())
    {
        auto [cx, cy] = st.top();
        st.pop();

        if (cx < 0 || cy < 0 || cx > 2000 || cy > 2000)
            continue;

        COLORREF current = GetPixel(g_hdc, cx, cy);

        if (current == boundaryColor || current == fillColor)
            continue;

        SetPixel(g_hdc, cx, cy, fillColor);

        st.push({ cx + 1, cy });
        st.push({ cx - 1, cy });
        st.push({ cx, cy + 1 });
        st.push({ cx, cy - 1 });
    }
}