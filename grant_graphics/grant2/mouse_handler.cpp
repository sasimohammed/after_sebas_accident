// mouse_handler.cpp
// ─────────────────────────────────────────────────────────────────────────────
// ROOT CAUSE OF FILL DISAPPEARING:
//   Old code called GetDC() and drew fills directly, then Refresh() triggered
//   WM_PAINT which re-filled the canvas white — erasing everything.
//
// THE FIX:
//   NEVER draw with GetDC() here. Only save the shape to core, then call
//   Refresh(). WM_PAINT redraws everything from core onto the double-buffer
//   memDC, so fills persist permanently.
//
// FLOOD FILL EXCEPTION:
//   Flood fill is pixel-based (reads existing pixel colors), so it must run
//   on the actual screen HDC. But we call it AFTER WM_PAINT has finished
//   via a posted message, not during the click handler.
// ─────────────────────────────────────────────────────────────────────────────
#include "mouse_handler.h"
#include "line.h"
#include "filling.h"
#include "menu.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
//  Private state
// ─────────────────────────────────────────────────────────────────────────────
static Point lineStart = { 0, 0 };
static bool  lineDrawing = false;

static int           fillMode = -1;
static int           fillQuarter = 1;    // 1=Top-Right  2=Top-Left  3=Bot-Left  4=Bot-Right
static int           fillStep = 0;    // 0 = waiting 1st click, 1 = waiting 2nd
static Point         fillP1 = { 0, 0 };
static vector<Point> fillPolygon;
static bool          fillActive = false;

static Point previewPt = { 0, 0 };

// Flood fill pending state
static bool  pendingFlood = false;
static int   pendingFloodX = 0;
static int   pendingFloodY = 0;

// ─────────────────────────────────────────────────────────────────────────────
static inline void Refresh(HWND hwnd)
{
    InvalidateRect(hwnd, NULL, FALSE);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Public API
// ─────────────────────────────────────────────────────────────────────────────
void InitMouseHandler()
{
    lineDrawing = false;
    fillActive = false;
    fillMode = -1;
    fillStep = 0;
    pendingFlood = false;
    fillPolygon.clear();
    cout << "[MouseHandler] Initialized\n";
}

void SetFillMode(HWND hwnd, int menuId)
{
    fillMode = menuId;
    fillActive = true;
    fillStep = 0;
    lineDrawing = false;
    pendingFlood = false;
    fillPolygon.clear();
    currentMode = menuId;

    switch (menuId)
    {
    case ID_FILL_CIRC_LINE:   cout << "[FILL] Circle-Lines  – click CENTER then RADIUS  (quarter " << fillQuarter << ")\n"; break;
    case ID_FILL_CIRC_CIRC:   cout << "[FILL] Circle-Circles – click CENTER then RADIUS  (quarter " << fillQuarter << ")\n"; break;
    case ID_FILL_SQ_HERMIT:   cout << "[FILL] Square-Hermit – click & drag\n"; break;
    case ID_FILL_RECT_BEZIER: cout << "[FILL] Rect-Bezier – click & drag\n"; break;
    case ID_FILL_CONVEX:      cout << "[FILL] Convex – click vertices, right-click to fill\n"; break;
    case ID_FILL_FLOOD:
        cout << "[FILL] Flood Fill – click inside a closed boundary\n";
        fillStep = 1;
        break;
    }
    Refresh(hwnd);
}

void SetFillQuarter(int quarter)
{
    if (quarter < 1 || quarter > 4) return;
    fillQuarter = quarter;
    const char* names[] = { "", "Top-Right", "Top-Left", "Bottom-Left", "Bottom-Right" };
    cout << "[FILL] Quarter = " << names[quarter] << "\n";
}

void HandleLineDrawing(HWND hwnd, int x, int y)
{
    if (!lineDrawing)
    {
        lineStart = { x, y };
        lineDrawing = true;
        cout << "LINE: start (" << x << "," << y << ")\n";
        Refresh(hwnd);
    }
    else
    {
        Point end = { x, y };
        if (lineStart.x == end.x && lineStart.y == end.y)
        {
            // Same point clicked twice – draw a visible dot
            Shape s;
            s.type = POINT_DOT;
            s.algorithm = currentMode;
            s.color = pref.getCurrentColor();
            s.radius = 4;
            s.points.push_back(lineStart);
            core.addShape(s);
            cout << "POINT: (" << x << "," << y << ")\n";
        }
        else
        {
            Shape s;
            s.type = LINE;
            s.algorithm = currentMode;
            s.color = pref.getCurrentColor();
            s.points.push_back(lineStart);
            s.points.push_back(end);
            core.addShape(s);
            cout << "LINE: (" << lineStart.x << "," << lineStart.y << ") -> (" << x << "," << y << ")\n";
        }
        lineDrawing = false;
        Refresh(hwnd);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  HandleFillClick
//
//  KEY RULE: do NOT call GetDC / draw anything here for non-flood fills.
//  Just save the Shape to core and call Refresh().
//  WM_PAINT will redraw the shape from core every frame — that is why it persists.
// ─────────────────────────────────────────────────────────────────────────────
bool HandleFillClick(HWND hwnd, int x, int y)
{
    if (!fillActive) return false;

    // ── a & b: Circle fills ───────────────────────────────────────────────────
    if (fillMode == ID_FILL_CIRC_LINE || fillMode == ID_FILL_CIRC_CIRC)
    {
        if (fillStep == 0)
        {
            fillP1 = { x, y };
            fillStep = 1;
            cout << "[FILL] Circle CENTER (" << x << "," << y << ") – now click RADIUS point\n";
            Refresh(hwnd);
            return true;
        }
        else
        {
            // Build and store the shape. WM_PAINT will call FillCircleWith* from core.
            Shape s;
            s.type = FILLED_SHAPE;
            s.algorithm = fillMode;
            s.color = pref.getCurrentColor();
            s.radius = fillQuarter;        // quarter stored here
            s.points.push_back(fillP1);       // [0] = centre
            s.points.push_back({ x, y });     // [1] = radius endpoint
            core.addShape(s);

            int r = (int)sqrt(pow((double)(x - fillP1.x), 2.0) +
                pow((double)(y - fillP1.y), 2.0));
            cout << "✓ Circle fill saved (r=" << r << ", quarter=" << fillQuarter << ")\n";

            fillActive = false;
            fillStep = 0;
            Refresh(hwnd);   // WM_PAINT redraws fill from core – persists forever
            return false;
        }
    }

    // ── c & d: Drag-based fills (mouse-down starts, mouse-up finishes) ─────────
    if (fillMode == ID_FILL_SQ_HERMIT || fillMode == ID_FILL_RECT_BEZIER)
    {
        if (fillStep == 0)
        {
            fillP1 = { x, y };
            fillStep = 1;
            cout << "[FILL] Drag start (" << x << "," << y << ") – release to finish\n";
            Refresh(hwnd);
            return true;
        }
        return true;
    }

    // ── f: Flood fill ──────────────────────────────────────────────────────────
    // Special case: flood fill is pixel-based so it MUST read existing screen pixels.
    // We store the seed point and set a pending flag.
    // WM_PAINT checks the flag AFTER blitting the double-buffer and runs the flood
    // directly on the real HDC (which already has all shapes visible).
    if (fillMode == ID_FILL_FLOOD && fillStep == 1)
    {
        pendingFloodX = x;
        pendingFloodY = y;
        pendingFlood = true;

        fillActive = false;
        fillStep = 0;

        // Trigger one paint so the canvas is fully up to date before flooding
        Refresh(hwnd);
        cout << "✓ Flood fill pending at (" << x << "," << y << ")\n";
        return false;
    }

    // ── e: Convex / general polygon ────────────────────────────────────────────
    if (fillMode == ID_FILL_CONVEX)
    {
        fillPolygon.push_back({ x, y });
        cout << "[FILL] Polygon vertex " << fillPolygon.size() << " (" << x << "," << y << ") – right-click to fill\n";
        Refresh(hwnd);
        return true;
    }

    return false;
}

void HandleFillRightClick(HWND hwnd)
{
    if (!fillActive || fillMode != ID_FILL_CONVEX) return;
    if ((int)fillPolygon.size() < 3) { cout << "[FILL] Need >= 3 vertices\n"; return; }

    Shape s;
    s.type = FILLED_SHAPE;
    s.algorithm = ID_FILL_CONVEX;
    s.color = pref.getCurrentColor();
    for (auto& p : fillPolygon) s.points.push_back(p);
    core.addShape(s);

    fillPolygon.clear();
    fillActive = false;
    Refresh(hwnd);   // WM_PAINT calls GeneralPolygonFill from core
    cout << "✓ Polygon fill saved\n";
}

void HandleFillMouseUp(HWND hwnd, int x, int y)
{
    if (!fillActive) return;
    if (fillMode != ID_FILL_SQ_HERMIT && fillMode != ID_FILL_RECT_BEZIER) return;
    if (fillStep != 1) return;

    Shape s;
    s.type = FILLED_SHAPE;
    s.algorithm = fillMode;
    s.color = pref.getCurrentColor();
    s.points.push_back(fillP1);
    s.points.push_back({ x, y });
    core.addShape(s);

    fillActive = false;
    fillStep = 0;
    Refresh(hwnd);   // WM_PAINT calls FillSquareWithHermit / FillRectWithBezier from core
    cout << "✓ " << (fillMode == ID_FILL_SQ_HERMIT ? "Hermit" : "Bezier") << " fill saved\n";
}

void HandleFillMouseMove(HWND hwnd, int x, int y)
{
    previewPt = { x, y };
    if (!fillActive) return;

    bool need =
        (fillStep == 1 &&
            (fillMode == ID_FILL_CIRC_LINE ||
                fillMode == ID_FILL_CIRC_CIRC ||
                fillMode == ID_FILL_SQ_HERMIT ||
                fillMode == ID_FILL_RECT_BEZIER)) ||
        (!fillPolygon.empty() && fillMode == ID_FILL_CONVEX);

    if (need) Refresh(hwnd);
}

// Called from WM_PAINT after the BitBlt to the real screen HDC.
// This is the correct moment: the canvas pixels are fully rendered,
// so flood fill reads the right boundary colors.
void ExecutePendingFloodFill(HDC screenHDC)
{
    if (!pendingFlood) return;
    pendingFlood = false;
    FloodFillStack(screenHDC, pendingFloodX, pendingFloodY, BOUND_COLOR, FILL_COLOR);
    cout << "✓ Flood fill executed at (" << pendingFloodX << "," << pendingFloodY << ")\n";
}

bool HasPendingFloodFill() { return pendingFlood; }

// ─────────────────────────────────────────────────────────────────────────────
//  DrawFillPreview  – rubber-band guides shown during point placement
//  Called on the double-buffer memDC inside WM_PAINT.
// ─────────────────────────────────────────────────────────────────────────────
void DrawFillPreview(HDC hdc)
{
    if (!fillActive) return;

    auto DashedCircle = [&](int cx, int cy, int r, COLORREF c)
        {
            if (r <= 0) return;
            HPEN p = CreatePen(PS_DASH, 1, c);
            HPEN op = (HPEN)SelectObject(hdc, p);
            HBRUSH ob = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Ellipse(hdc, cx - r, cy - r, cx + r, cy + r);
            SelectObject(hdc, op); SelectObject(hdc, ob); DeleteObject(p);
        };
    auto DashedRect = [&](int l, int t, int r2, int b, COLORREF c)
        {
            HPEN p = CreatePen(PS_DASH, 1, c);
            HPEN op = (HPEN)SelectObject(hdc, p);
            HBRUSH ob = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, l, t, r2, b);
            SelectObject(hdc, op); SelectObject(hdc, ob); DeleteObject(p);
        };
    auto FilledDot = [&](int cx, int cy, COLORREF c)
        {
            HBRUSH b = CreateSolidBrush(c);
            HPEN   np = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
            HBRUSH ob = (HBRUSH)SelectObject(hdc, b);
            Ellipse(hdc, cx - 5, cy - 5, cx + 5, cy + 5);
            SelectObject(hdc, ob); SelectObject(hdc, np); DeleteObject(b);
        };

    // a & b: circle preview after centre is set
    if ((fillMode == ID_FILL_CIRC_LINE || fillMode == ID_FILL_CIRC_CIRC) && fillStep == 1)
    {
        int r = (int)sqrt(pow((double)(previewPt.x - fillP1.x), 2.0) +
            pow((double)(previewPt.y - fillP1.y), 2.0));
        DashedCircle(fillP1.x, fillP1.y, r, RGB(0, 150, 220));
        FilledDot(fillP1.x, fillP1.y, RGB(0, 150, 220));
    }

    // c & d: drag rectangle preview
    if ((fillMode == ID_FILL_SQ_HERMIT || fillMode == ID_FILL_RECT_BEZIER) && fillStep == 1)
    {
        COLORREF col = (fillMode == ID_FILL_SQ_HERMIT) ? RGB(0, 200, 100) : RGB(220, 130, 0);
        DashedRect(min(fillP1.x, previewPt.x), min(fillP1.y, previewPt.y),
            max(fillP1.x, previewPt.x), max(fillP1.y, previewPt.y), col);
    }

    // e: polygon vertices + rubber-band edge
    if (fillMode == ID_FILL_CONVEX && !fillPolygon.empty())
    {
        HPEN   p = CreatePen(PS_DASH, 1, RGB(0, 200, 150));
        HPEN   op = (HPEN)SelectObject(hdc, p);
        HBRUSH b = CreateSolidBrush(RGB(0, 200, 150));
        HBRUSH ob = (HBRUSH)SelectObject(hdc, b);
        for (auto& pt : fillPolygon) Ellipse(hdc, pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3);
        MoveToEx(hdc, fillPolygon.back().x, fillPolygon.back().y, NULL);
        LineTo(hdc, previewPt.x, previewPt.y);
        SelectObject(hdc, op); SelectObject(hdc, ob); DeleteObject(p); DeleteObject(b);
    }
}

void ResetDrawingState()
{
    lineDrawing = false;
    fillActive = false;
    fillMode = -1;
    fillStep = 0;
    pendingFlood = false;
    fillPolygon.clear();
    cout << "[MouseHandler] State reset\n";
}

void DrawPreview(HDC hdc, int x, int y)
{
    if (!lineDrawing) return;
    HPEN p = CreatePen(PS_DOT, 1, RGB(128, 128, 128));
    HPEN op = (HPEN)SelectObject(hdc, p);
    MoveToEx(hdc, lineStart.x, lineStart.y, NULL);
    LineTo(hdc, x, y);
    SelectObject(hdc, op); DeleteObject(p);
}

bool  IsFilling() { return fillActive; }
int   GetFillMode() { return fillMode; }
int   GetFillQuarter() { return fillQuarter; }