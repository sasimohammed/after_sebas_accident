// filling.cpp  –  All filling algorithms for the 2D Drawing Package
// ─────────────────────────────────────────────────────────────────────────────
#include "filling.h"
#include <cmath>
#include <vector>
#include <stack>
#include <algorithm>
#include <climits>

using namespace std;

#ifndef PI
#define PI 3.14159265358979323846
#endif

// =============================================================================
//  a)  Fill circle with lines  (one quarter)
//
//  SCREEN COORDINATE NOTE: Y increases downward on screen.
//  "Top" visually means smaller Y values, "Bottom" means larger Y values.
//
//  We draw a spoke from every boundary point to the centre.
//  The quarter selection restricts which boundary arc we sweep.
//
//  Mapping (angles are standard maths angles, but y is negated for screen):
//    Quarter 1 = Top-Right    → x >= cx, y <= cy  → angles  0  .. PI/2
//    Quarter 2 = Top-Left     → x <= cx, y <= cy  → angles PI/2 .. PI
//    Quarter 3 = Bottom-Left  → x <= cx, y >= cy  → angles PI  .. 3PI/2
//    Quarter 4 = Bottom-Right → x >= cx, y >= cy  → angles 3PI/2 .. 2PI
// =============================================================================
void FillCircleWithLines(HDC hdc, int cx, int cy, int r, int quarter)
{
    double a1, a2;
    switch (quarter)
    {
    case 1:  a1 = 0;              a2 = PI / 2;       break;  // Top-Right
    case 2:  a1 = PI / 2;         a2 = PI;            break;  // Top-Left
    case 3:  a1 = PI;             a2 = 3.0 * PI / 2;  break;  // Bottom-Left
    default: a1 = 3.0 * PI / 2;   a2 = 2.0 * PI;      break;  // Bottom-Right
    }

    HPEN pen = CreatePen(PS_SOLID, 1, FILL_COLOR);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    for (double a = a1; a <= a2; a += 0.005)
    {
        // cos gives x offset; -sin gives y offset because screen Y is inverted
        int x = cx + (int)(r * cos(a));
        int y = cy - (int)(r * sin(a));
        MoveToEx(hdc, x, y, NULL);
        LineTo(hdc, cx, cy);   // spoke: rim → centre
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

// =============================================================================
//  b)  Fill circle with concentric circles  (one quarter)
//
//  SCREEN COORDINATE FIX:
//  The original code had the clipping rectangles wrong for Top/Bottom quarters
//  because it didn't account for screen Y going downward.
//
//  Correct quarter clip regions (all relative to circle centre cx, cy):
//    Quarter 1 Top-Right    → x in [cx, cx+r],  y in [cy-r, cy]
//    Quarter 2 Top-Left     → x in [cx-r, cx],  y in [cy-r, cy]
//    Quarter 3 Bottom-Left  → x in [cx-r, cx],  y in [cy,   cy+r]
//    Quarter 4 Bottom-Right → x in [cx,   cx+r], y in [cy,  cy+r]
// =============================================================================
void FillCircleWithCircles(HDC hdc, int cx, int cy, int r, int quarter)
{
    HPEN   pen = CreatePen(PS_SOLID, 1, FILL_COLOR);
    HPEN   oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    for (int rr = 4; rr <= r; rr += 6)
    {
        HRGN clip = nullptr;
        switch (quarter)
        {
        case 1:  clip = CreateRectRgn(cx, cy - r, cx + r, cy); break; // Top-Right
        case 2:  clip = CreateRectRgn(cx - r, cy - r, cx, cy); break; // Top-Left
        case 3:  clip = CreateRectRgn(cx - r, cy, cx, cy + r); break; // Bottom-Left
        default: clip = CreateRectRgn(cx, cy, cx + r, cy + r); break; // Bottom-Right
        }
        SelectClipRgn(hdc, clip);
        Ellipse(hdc, cx - rr, cy - rr, cx + rr, cy + rr);
        SelectClipRgn(hdc, NULL);
        if (clip) DeleteObject(clip);
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

// =============================================================================
//  c)  Fill square with Hermit curves  (vertical strokes, alternating tangent)
// =============================================================================
static void DrawHermitCurve(HDC hdc,
    int x0, int y0, int x1, int y1,
    int tx0, int ty0, int tx1, int ty1,
    COLORREF color)
{
    HPEN pen = CreatePen(PS_SOLID, 2, color);
    HPEN old = (HPEN)SelectObject(hdc, pen);

    int px = x0, py = y0;
    for (int i = 1; i <= 120; i++)
    {
        double t = i / 120.0;
        double t2 = t * t;
        double t3 = t2 * t;

        // Hermite basis functions
        double h00 = 2 * t3 - 3 * t2 + 1;
        double h10 = t3 - 2 * t2 + t;
        double h01 = -2 * t3 + 3 * t2;
        double h11 = t3 - t2;

        int x = (int)(h00 * x0 + h10 * tx0 + h01 * x1 + h11 * tx1);
        int y = (int)(h00 * y0 + h10 * ty0 + h01 * y1 + h11 * ty1);

        MoveToEx(hdc, px, py, NULL);
        LineTo(hdc, x, y);
        px = x; py = y;
    }

    SelectObject(hdc, old);
    DeleteObject(pen);
}

void FillSquareWithHermit(HDC hdc, int left, int top, int right, int bottom)
{
    if (left > right)  swap(left, right);
    if (top > bottom) swap(top, bottom);

    int h = bottom - top;
    int step = 14;   // spacing between vertical strokes

    for (int x = left; x <= right; x += step)
    {
        // Alternate tangent direction for a wave-like fill
        int sign = (((x - left) / step) % 2 == 0) ? 1 : -1;
        DrawHermitCurve(hdc,
            x, top, x, bottom,
            sign * h / 2, 0,
            -sign * h / 2, 0,
            FILL_COLOR);
    }
}

// =============================================================================
//  d)  Fill rectangle with Bezier curves  (vertical strokes sweeping inward)
// =============================================================================
static void DrawBezierStroke(HDC hdc,
    int x1, int y1, int x2, int y2,
    int x3, int y3, int x4, int y4,
    COLORREF color)
{
    for (double t = 0.0; t <= 1.0; t += 0.005)
    {
        double mt = 1.0 - t;
        int x = (int)(mt * mt * mt * x1 + 3 * mt * mt * t * x2 + 3 * mt * t * t * x3 + t * t * t * x4);
        int y = (int)(mt * mt * mt * y1 + 3 * mt * mt * t * y2 + 3 * mt * t * t * y3 + t * t * t * y4);
        SetPixel(hdc, x, y, color);
    }
}

void FillRectWithBezier(HDC hdc, int left, int top, int right, int bottom)
{
    if (left > right)  swap(left, right);
    if (top > bottom) swap(top, bottom);

    int w = right - left;
    int h = bottom - top;
    int steps = 14;

    for (int i = 0; i <= steps; i++)
    {
        double t = (double)i / steps;

        // Left-side vertical curve sweeping right
        DrawBezierStroke(hdc,
            left + (int)(w * t), top,
            left + (int)(w * t * 0.7), top + (int)(h * 0.33),
            left + (int)(w * t * 0.3), bottom - (int)(h * 0.33),
            left + (int)(w * t), bottom,
            FILL_COLOR);

        // Right-side vertical curve sweeping left (mirror)
        DrawBezierStroke(hdc,
            right - (int)(w * t), top,
            right - (int)(w * t * 0.7), top + (int)(h * 0.33),
            right - (int)(w * t * 0.3), bottom - (int)(h * 0.33),
            right - (int)(w * t), bottom,
            FILL_COLOR);
    }

    // Draw bounding rectangle outline so the region is visible
    HPEN bp = CreatePen(PS_SOLID, 1, BOUND_COLOR);
    HPEN op = (HPEN)SelectObject(hdc, bp);
    MoveToEx(hdc, left, top, NULL);
    LineTo(hdc, right, top);
    LineTo(hdc, right, bottom);
    LineTo(hdc, left, bottom);
    LineTo(hdc, left, top);
    SelectObject(hdc, op);
    DeleteObject(bp);
}

// =============================================================================
//  e-1)  Convex polygon fill  (simple scan-line with xmin/xmax per row)
// =============================================================================
struct ScanEntry { int xmin, xmax; };

static void InitScanTable(ScanEntry table[], int size)
{
    for (int i = 0; i < size; i++)
    {
        table[i].xmin = INT_MAX;
        table[i].xmax = INT_MIN;
    }
}

static void ScanEdge(POINT v1, POINT v2, ScanEntry table[], int tableSize)
{
    if (v1.y == v2.y) return;           // horizontal edge – skip
    if (v1.y > v2.y)  swap(v1, v2);    // ensure v1 is topmost (smallest y)

    double dx = (double)(v2.x - v1.x) / (v2.y - v1.y);
    double x = v1.x;

    for (int y = v1.y; y < v2.y; y++, x += dx)
    {
        if (y < 0 || y >= tableSize) continue;
        int xi = (int)x;
        table[y].xmin = min(table[y].xmin, xi);
        table[y].xmax = max(table[y].xmax, xi);
    }
}

void ConvexFill(HDC hdc, POINT pts[], int n, COLORREF color)
{
    if (n < 3) return;

    ScanEntry* table = new ScanEntry[MAXENTRIES];
    InitScanTable(table, MAXENTRIES);

    POINT prev = pts[n - 1];
    for (int i = 0; i < n; i++)
    {
        ScanEdge(prev, pts[i], table, MAXENTRIES);
        prev = pts[i];
    }

    for (int y = 0; y < MAXENTRIES; y++)
    {
        if (table[y].xmin > table[y].xmax) continue;
        for (int x = table[y].xmin; x <= table[y].xmax; x++)
            SetPixel(hdc, x, y, color);
    }

    delete[] table;
}

// =============================================================================
//  e-2)  General (non-convex) polygon fill  (active edge table / AET)
// =============================================================================
struct EdgeRecord
{
    double x;    // current x intercept
    double dx;   // inverse slope (Δx per scan line)
    int    ymax;
};

void GeneralPolygonFill(HDC hdc, POINT pts[], int n, COLORREF color)
{
    if (n < 3) return;

    // Build edge table indexed by ymin of each edge
    vector<vector<EdgeRecord>> ET(MAXENTRIES);

    POINT v1 = pts[n - 1];
    for (int i = 0; i < n; i++)
    {
        POINT v2 = pts[i];
        if (v1.y != v2.y)
        {
            POINT lo = v1, hi = v2;
            if (lo.y > hi.y) swap(lo, hi);

            EdgeRecord e;
            e.x = lo.x;
            e.dx = (double)(hi.x - lo.x) / (hi.y - lo.y);
            e.ymax = hi.y;

            if (lo.y >= 0 && lo.y < MAXENTRIES)
                ET[lo.y].push_back(e);
        }
        v1 = pts[i];
    }

    // Sweep scan lines
    vector<EdgeRecord> AET;

    for (int y = 0; y < MAXENTRIES; y++)
    {
        // Add new edges that start at this scan line
        for (auto& e : ET[y]) AET.push_back(e);

        // Remove edges that have ended
        AET.erase(
            remove_if(AET.begin(), AET.end(),
                [y](const EdgeRecord& e) { return e.ymax <= y; }),
            AET.end());

        // Sort by current x intercept
        sort(AET.begin(), AET.end(),
            [](const EdgeRecord& a, const EdgeRecord& b) { return a.x < b.x; });

        // Fill between pairs of intersections
        for (size_t i = 0; i + 1 < AET.size(); i += 2)
        {
            int xStart = (int)AET[i].x;
            int xEnd = (int)AET[i + 1].x;
            for (int x = xStart; x <= xEnd; x++)
                SetPixel(hdc, x, y, color);
        }

        // Advance x intercepts to next scan line
        for (auto& e : AET) e.x += e.dx;
    }
}

// =============================================================================
//  f-1)  Recursive flood fill
//         WARNING: may cause stack overflow on large regions.
//         Use the stack-based version below for production / large canvases.
// =============================================================================
void FloodFillRecursive(HDC hdc, int x, int y, COLORREF boundary, COLORREF fillColor)
{
    if (x < 0 || y < 0 || x >= MAXENTRIES || y >= MAXENTRIES) return;

    COLORREF cur = GetPixel(hdc, x, y);
    if (cur == boundary || cur == fillColor) return;

    SetPixel(hdc, x, y, fillColor);

    FloodFillRecursive(hdc, x + 1, y, boundary, fillColor);
    FloodFillRecursive(hdc, x - 1, y, boundary, fillColor);
    FloodFillRecursive(hdc, x, y + 1, boundary, fillColor);
    FloodFillRecursive(hdc, x, y - 1, boundary, fillColor);
}

// =============================================================================
//  f-2)  Non-recursive (stack-based) flood fill  – preferred
// =============================================================================
void FloodFillStack(HDC hdc, int x, int y, COLORREF boundary, COLORREF fillColor)
{
    struct Pt { int x, y; };
    stack<Pt> st;
    st.push({ x, y });

    while (!st.empty())
    {
        Pt p = st.top();
        st.pop();

        if (p.x < 0 || p.y < 0 || p.x >= MAXENTRIES || p.y >= MAXENTRIES) continue;

        COLORREF cur = GetPixel(hdc, p.x, p.y);
        if (cur == boundary || cur == fillColor) continue;

        SetPixel(hdc, p.x, p.y, fillColor);

        st.push({ p.x + 1, p.y });
        st.push({ p.x - 1, p.y });
        st.push({ p.x,     p.y + 1 });
        st.push({ p.x,     p.y - 1 });
    }
}