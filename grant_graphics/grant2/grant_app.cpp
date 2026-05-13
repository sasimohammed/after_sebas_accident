#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "core_data.h"
#include "file_system.h"
#include "preferences.h"
#include "advanced_gui.h"
#include "circle_clipping.h"
#include "smiley.h"
#include "line.h"
#include "ellipse.h"
#include "circles.h"
#include "curve.h"
#include "filling.h"
#include "mouse_handler.h"
#include "menu.h"

using namespace std;

#ifndef PI
#define PI 3.14159265358979323846
#endif

// =============================================================================
//  GLOBALS
// =============================================================================
CoreData    core;
Preferences pref;
int         currentMode = -1;

static bool  isDrawing = false;
static Point drawingStart = { 0, 0 };
static Point drawingEnd = { 0, 0 };
static int   drawingAlgorithm = -1;

static bool  isDrawingEllipse = false;
static Point ellipseCenter = { 0, 0 };

static bool  isDrawingCircle = false;
static Point circleCenter = { 0, 0 };
static int   circleAlgorithm = -1;

static bool          isDrawingCurve = false;
static vector<Point> curvePoints;

// ── Clipping ──────────────────────────────────────────────────────────────────
static bool  hasClippingCircle = false;
static Point clippingCircleCenter = { 0, 0 };
static int   clippingCircleRadius = 0;

static bool  hasClippingRect = false;
static int   clippingRectLeft, clippingRectTop, clippingRectRight, clippingRectBottom;

static bool  hasClippingSquare = false;
static Point clippingSquareCenter = { 0, 0 };
static int   clippingSquareSide = 0;

static bool  clippingEnabled = false;
static bool  isDrawingClippingShape = false;
static bool  isPointClippingMode = false;
static bool  isLineClippingMode = false;
static bool  isPolygonClippingMode = false;
static int   clippingType = 0;   // 0=circle 1=rect 2=square

static Point clippingPreviewEnd = { 0, 0 };

// =============================================================================
//  HELPER
// =============================================================================
static void InvalidateCanvas(HWND hwnd)
{
    RECT rc = GetGUI().GetCanvasRect(hwnd);
    InvalidateRect(hwnd, &rc, FALSE);
}

// =============================================================================
//  DrawShapeWithClipping
//
//  Called every WM_PAINT from the shape list.  All rendering goes through here.
//  Fills are redrawn from core data every frame — that is what makes them persist.
// =============================================================================
void DrawShapeWithClipping(HDC hdc, const Shape& s, bool applyClipping)
{
    switch (s.type)
    {
        // ── LINE ─────────────────────────────────────────────────────────────────
    case LINE:
    {
        if (s.points.size() < 2) break;
        COLORREF color = (s.color == "RED") ? RGB(255, 0, 0) : RGB(0, 0, 0);
        bool clipped = false;

        if (applyClipping && clippingEnabled)
        {
           /* if (clippingType == 0 && hasClippingCircle && isLineClippingMode)
            {
                CircleLineClipping(hdc, clippingCircleCenter.x, clippingCircleCenter.y, clippingCircleRadius, s.points[0].x, s.points[0].y, s.points[1].x, s.points[1].y); clipped = true;
            }
            else if (clippingType == 1 && hasClippingRect && isLineClippingMode)
            {
                RectLineClipping(hdc, clippingRectLeft, clippingRectTop, clippingRectRight, clippingRectBottom, s.points[0].x, s.points[0].y, s.points[1].x, s.points[1].y); clipped = true;
            }
            else if (clippingType == 2 && hasClippingSquare && isLineClippingMode)
            {
                SquareLineClipping(hdc, clippingSquareCenter.x, clippingSquareCenter.y, clippingSquareSide, s.points[0].x, s.points[0].y, s.points[1].x, s.points[1].y); clipped = true;
            }
        }
        if (!clipped)
        {
            switch (s.algorithm)
            {
            case ID_LINE_DDA:   DrawLine_DDA(hdc, s.points[0], s.points[1], color); break;
            case ID_LINE_MID:   DrawLine_Midpoint(hdc, s.points[0], s.points[1], color); break;
            case ID_LINE_PARAM: DrawLine_Parametric(hdc, s.points[0], s.points[1], color); break;
            default:            DrawLine_DDA(hdc, s.points[0], s.points[1], color); break;
            }
        }
        break;*/
    }

    // ── CIRCLE ───────────────────────────────────────────────────────────────
    case CIRCLE:
    {
        if (s.points.size() < 1) break;
        COLORREF color = (s.color == "RED") ? RGB(255, 0, 0) : RGB(0, 0, 0);
        switch (s.algorithm)
        {
        case ID_CIRC_DIRECT:     CircleDirect(hdc, s.points[0].x, s.points[0].y, s.radius, color); break;
        case ID_CIRC_POLAR:      CirclePolar(hdc, s.points[0].x, s.points[0].y, s.radius, color); break;
        case ID_CIRC_ITER_POLAR: CircleIterativePolar(hdc, s.points[0].x, s.points[0].y, s.radius, color); break;
        case ID_CIRC_MID:        CircleMidPoint(hdc, s.points[0].x, s.points[0].y, s.radius, color); break;
        case ID_CIRC_MOD_MID:    CircleModifiedMidPoint(hdc, s.points[0].x, s.points[0].y, s.radius, color); break;
        default:                 CircleMidPoint(hdc, s.points[0].x, s.points[0].y, s.radius, color); break;
        }
        break;
    }

    // ── ELLIPSE ──────────────────────────────────────────────────────────────
    case ELLIPSE:
    {
        if (s.points.size() < 2) break;
        COLORREF color = (s.color == "RED") ? RGB(255, 0, 0) : RGB(0, 0, 0);
        int a = abs(s.points[1].x - s.points[0].x);
        int b = abs(s.points[1].y - s.points[0].y);
        switch (s.algorithm)
        {
        case ID_ELLIPSE_DIR: drawEllipseDirect(hdc, s.points[0].x, s.points[0].y, a, b, color); break;
        case ID_ELLIPSE_POL: drawEllipsePolar(hdc, s.points[0].x, s.points[0].y, a, b, color); break;
        case ID_ELLIPSE_MID: drawEllipseMidpoint(hdc, s.points[0].x, s.points[0].y, a, b, color); break;
        default:             drawEllipseMidpoint(hdc, s.points[0].x, s.points[0].y, a, b, color); break;
        }
        break;
    }

    // ── CURVE ────────────────────────────────────────────────────────────────
    case CURVE:
    {
        if (s.points.size() < 2) break;
        COLORREF color = (s.color == "RED") ? RGB(255, 0, 0) : RGB(0, 0, 0);
        int n = (int)s.points.size();
        POINT* pts = new POINT[n];
        for (int i = 0; i < n; i++) { pts[i].x = s.points[i].x; pts[i].y = s.points[i].y; }
        DrawCardinalSpline(hdc, pts, n, color);
        delete[] pts;
        break;
    }

    // ── FILLED SHAPE ─────────────────────────────────────────────────────────
    // Each fill algorithm is redrawn from the stored shape data every WM_PAINT.
    // This is the mechanism that makes fills persist on screen.
    case FILLED_SHAPE:
    {
        switch (s.algorithm)
        {
            // a) Circle filled with lines
        case ID_FILL_CIRC_LINE:
            if (s.points.size() >= 2)
            {
                int r = (int)sqrt(pow((double)(s.points[1].x - s.points[0].x), 2.0) +
                    pow((double)(s.points[1].y - s.points[0].y), 2.0));
                FillCircleWithLines(hdc, s.points[0].x, s.points[0].y, r, s.radius);
            }
            break;

            // b) Circle filled with circles
        case ID_FILL_CIRC_CIRC:
            if (s.points.size() >= 2)
            {
                int r = (int)sqrt(pow((double)(s.points[1].x - s.points[0].x), 2.0) +
                    pow((double)(s.points[1].y - s.points[0].y), 2.0));
                FillCircleWithCircles(hdc, s.points[0].x, s.points[0].y, r, s.radius);
            }
            break;

            // c) Square with Hermit curves
        case ID_FILL_SQ_HERMIT:
            if (s.points.size() >= 2)
                FillSquareWithHermit(hdc, s.points[0].x, s.points[0].y, s.points[1].x, s.points[1].y);
            break;

            // d) Rectangle with Bezier curves
        case ID_FILL_RECT_BEZIER:
            if (s.points.size() >= 2)
                FillRectWithBezier(hdc, s.points[0].x, s.points[0].y, s.points[1].x, s.points[1].y);
            break;

            // e) Convex / general polygon
        case ID_FILL_CONVEX:
            if (s.points.size() >= 3)
            {
                int n = (int)s.points.size();
                POINT* pts = new POINT[n];
                for (int i = 0; i < n; i++) { pts[i].x = s.points[i].x; pts[i].y = s.points[i].y; }
                GeneralPolygonFill(hdc, pts, n, FILL_COLOR);
                delete[] pts;
            }
            break;

            // f) Flood fill – pixel-based, baked into screen after BitBlt.
            //    Nothing to redraw from data; ExecutePendingFloodFill handles it.
        case ID_FILL_FLOOD:
            break;

        default: break;
        }
        break;
    }

    // ── SMILEY ───────────────────────────────────────────────────────────────
    case SMILEY_HAPPY:
        if (s.points.size() >= 1) DrawHappyFace(hdc, s.points[0].x, s.points[0].y, s.radius);
        break;

    case SMILEY_SAD:
        if (s.points.size() >= 1) DrawSadFace(hdc, s.points[0].x, s.points[0].y, s.radius);
        break;

        // ── POINT DOT ────────────────────────────────────────────────────────────
        // Used by point-clipping test and single-click line points.
        // Draws a small filled circle in the appropriate colour.
        // NOT a smiley face.
    case POINT_DOT:
    {
        if (s.points.size() < 1) break;
        COLORREF color =
            (s.color == "RED") ? RGB(255, 0, 0) :
            (s.color == "BLUE") ? RGB(0, 0, 255) :
            (s.color == "GREEN") ? RGB(0, 200, 0) :
            RGB(0, 0, 0);
        int r = (s.radius > 0) ? s.radius : 5;

        HPEN   pen = CreatePen(PS_SOLID, 1, color);
        HBRUSH brush = CreateSolidBrush(color);
        HPEN   oldPen = (HPEN)SelectObject(hdc, pen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

        Ellipse(hdc,
            s.points[0].x - r, s.points[0].y - r,
            s.points[0].x + r, s.points[0].y + r);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(pen);
        DeleteObject(brush);
        break;
    }

    default:
        break;
    }
}

// =============================================================================
//  INIT
// =============================================================================
void Init()
{
    pref.setCurrentColor("BLACK");
    pref.setBackground("WHITE");
    clippingEnabled = hasClippingCircle = hasClippingRect = hasClippingSquare = false;
    isDrawingClippingShape = isPointClippingMode = isLineClippingMode = isPolygonClippingMode = false;
    isDrawingEllipse = isDrawingCircle = isDrawingCurve = false;
    curvePoints.clear();
    clippingType = 0;
    InitMouseHandler();
    cout << "[LOG] Initialized\n";
}

// =============================================================================
//  WNDPROC
// =============================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND: return 1;

    case WM_SIZE:
        GetGUI().OnSize(LOWORD(lParam), HIWORD(lParam));
        InvalidateRect(hwnd, NULL, FALSE);
        break;

    case WM_MOUSEWHEEL:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hwnd, &pt);
        GetGUI().OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), pt.x, pt.y);
        break;
    }

    // ─────────────────────────────────────────────────────────────────────────
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_FILE_CLEAR:
            currentMode = -1;
            isDrawing = isDrawingEllipse = isDrawingCircle = isDrawingCurve = false;
            hasClippingCircle = hasClippingRect = hasClippingSquare = false;
            clippingEnabled = isDrawingClippingShape = false;
            curvePoints.clear();
            ResetDrawingState();
            GetGUI().SetActiveMode(-1);
            FileSystem::clearScreen(hwnd, core);
            InvalidateCanvas(hwnd);
            cout << "[LOG] CLEARED\n";
            break;

        case ID_FILE_SAVE: FileSystem::save(core); cout << "[LOG] SAVED\n"; break;
        case ID_FILE_LOAD: FileSystem::load(core); InvalidateCanvas(hwnd); cout << "[LOG] LOADED\n"; break;

        case ID_PREF_BG:     pref.setBackground("WHITE"); InvalidateCanvas(hwnd); break;
        case ID_PREF_CURSOR: SetClassLongPtr(hwnd, GCLP_HCURSOR, (LONG_PTR)LoadCursor(NULL, IDC_HAND)); break;
        case ID_PREF_COLOR:  pref.setCurrentColor("RED"); break;

        case ID_LINE_DDA: case ID_LINE_MID: case ID_LINE_PARAM:
            currentMode = drawingAlgorithm = LOWORD(wParam);
            isDrawing = isDrawingEllipse = isDrawingCircle = isDrawingCurve = isDrawingClippingShape = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode); InvalidateCanvas(hwnd);
            cout << "[LOG] Line mode\n"; break;

        case ID_CIRC_DIRECT: case ID_CIRC_POLAR: case ID_CIRC_ITER_POLAR:
        case ID_CIRC_MID:    case ID_CIRC_MOD_MID:
            currentMode = LOWORD(wParam); circleAlgorithm = currentMode; isDrawingCircle = true;
            isDrawing = isDrawingEllipse = isDrawingCurve = isDrawingClippingShape = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[LOG] Circle mode\n"; break;

        case ID_ELLIPSE_DIR: case ID_ELLIPSE_POL: case ID_ELLIPSE_MID:
            currentMode = drawingAlgorithm = LOWORD(wParam); isDrawingEllipse = true;
            isDrawing = isDrawingCircle = isDrawingCurve = isDrawingClippingShape = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[LOG] Ellipse mode\n"; break;

        case ID_CARDINAL_SPLINE:
            currentMode = LOWORD(wParam); isDrawingCurve = true;
            isDrawing = isDrawingEllipse = isDrawingCircle = isDrawingClippingShape = false;
            curvePoints.clear(); ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[LOG] Curve mode – click points, right-click finish\n"; break;

        case ID_QUARTER_1: SetFillQuarter(1); break;
        case ID_QUARTER_2: SetFillQuarter(2); break;
        case ID_QUARTER_3: SetFillQuarter(3); break;
        case ID_QUARTER_4: SetFillQuarter(4); break;

        case ID_FILL_CIRC_LINE: case ID_FILL_CIRC_CIRC:
        case ID_FILL_SQ_HERMIT: case ID_FILL_RECT_BEZIER:
        case ID_FILL_CONVEX:    case ID_FILL_FLOOD:
            isDrawing = isDrawingEllipse = isDrawingCircle = isDrawingCurve = isDrawingClippingShape = false;
            GetGUI().SetActiveMode(LOWORD(wParam));
            SetFillMode(hwnd, LOWORD(wParam));
            break;

        case ID_CLIP_RECT_POINT:
            currentMode = LOWORD(wParam); clippingType = 1;
            isPointClippingMode = true; isLineClippingMode = isPolygonClippingMode = false;
            isDrawingClippingShape = true; hasClippingRect = clippingEnabled = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[CLIP] Rect Point\n"; break;

        case ID_CLIP_RECT_LINE:
            currentMode = LOWORD(wParam); clippingType = 1;
            isLineClippingMode = true; isPointClippingMode = isPolygonClippingMode = false;
            isDrawingClippingShape = true; hasClippingRect = clippingEnabled = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[CLIP] Rect Line\n"; break;

        case ID_CLIP_RECT_POLY:
            currentMode = LOWORD(wParam); clippingType = 1;
            isPolygonClippingMode = true; isPointClippingMode = isLineClippingMode = false;
            isDrawingClippingShape = true; hasClippingRect = clippingEnabled = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[CLIP] Rect Polygon\n"; break;

        case ID_CLIP_SQUARE_POINT:
            currentMode = LOWORD(wParam); clippingType = 2;
            isPointClippingMode = true; isLineClippingMode = isPolygonClippingMode = false;
            isDrawingClippingShape = true; hasClippingSquare = clippingEnabled = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[CLIP] Square Point\n"; break;

        case ID_CLIP_SQUARE_LINE:
            currentMode = LOWORD(wParam); clippingType = 2;
            isLineClippingMode = true; isPointClippingMode = isPolygonClippingMode = false;
            isDrawingClippingShape = true; hasClippingSquare = clippingEnabled = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[CLIP] Square Line\n"; break;

        case ID_CLIP_CIRCLE_POINT:
            currentMode = LOWORD(wParam); clippingType = 0;
            isPointClippingMode = true; isLineClippingMode = isPolygonClippingMode = false;
            isDrawingClippingShape = true; hasClippingCircle = clippingEnabled = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[CLIP] Circle Point\n"; break;

        case ID_CLIP_CIRCLE_LINE:
            currentMode = LOWORD(wParam); clippingType = 0;
            isLineClippingMode = true; isPointClippingMode = isPolygonClippingMode = false;
            isDrawingClippingShape = true; hasClippingCircle = clippingEnabled = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode);
            cout << "[CLIP] Circle Line\n"; break;

        case ID_SMILE_HAPPY: case ID_SMILE_SAD:
            currentMode = LOWORD(wParam);
            isDrawing = isDrawingEllipse = isDrawingCircle = isDrawingCurve = isDrawingClippingShape = false;
            ResetDrawingState(); GetGUI().SetActiveMode(currentMode); break;
        }
        break;

        // ─────────────────────────────────────────────────────────────────────────
    case WM_RBUTTONDOWN:
        if (isDrawingCurve && curvePoints.size() >= 2)
        {
            Shape s; s.type = CURVE; s.algorithm = ID_CARDINAL_SPLINE;
            s.color = pref.getCurrentColor(); s.points = curvePoints;
            core.addShape(s); isDrawingCurve = false; curvePoints.clear();
            InvalidateCanvas(hwnd); cout << "✓ Curve added\n";
        }
        else if (IsFilling())
        {
            HandleFillRightClick(hwnd);
        }
        break;

        // ─────────────────────────────────────────────────────────────────────────
    case WM_LBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        if (GetGUI().OnLButtonDown(x, y)) break;
        cout << "Click: (" << x << "," << y << ")\n";

        if (IsFilling()) { HandleFillClick(hwnd, x, y); break; }

        if (isDrawingCurve)
        {
            curvePoints.push_back({ x,y }); cout << "  Curve pt " << curvePoints.size() << "\n"; InvalidateCanvas(hwnd); break;
        }

        if (isDrawingClippingShape && (clippingType == 1 || clippingType == 2))
        {
            if (!clippingEnabled) { drawingStart = { x,y }; isDrawing = true; } break;
        }

        if (isDrawingClippingShape && clippingType == 0)
        {
            if (!hasClippingCircle)
            {
                clippingCircleCenter = { x,y }; hasClippingCircle = true; cout << "  Clip circle CENTER\n"; InvalidateCanvas(hwnd);
            }
            else
            {
                int dx = x - clippingCircleCenter.x, dy = y - clippingCircleCenter.y; clippingCircleRadius = (int)sqrt((double)(dx * dx + dy * dy)); isDrawingClippingShape = false; clippingEnabled = true; cout << "✓ Circle clip r=" << clippingCircleRadius << "\n"; InvalidateCanvas(hwnd);
            }
            break;
        }

        // ── Point clipping test ───────────────────────────────────────────────
        // Draws a small POINT_DOT (coloured filled circle), NOT a smiley face.
        if (clippingEnabled && isPointClippingMode && !isDrawingClippingShape)
        {
            bool inside = false;

            Shape dot;
            dot.type = POINT_DOT;   // ← small coloured dot, not smiley
            dot.radius = 5;
            dot.points.push_back({ x, y });

            if (clippingType == 0 && hasClippingCircle)
            {
                inside = PointInsideCircle(clippingCircleCenter.x, clippingCircleCenter.y, clippingCircleRadius, x, y);
                dot.color = "RED";
            }
            else if (clippingType == 1 && hasClippingRect)
            {
                inside = PointInsideRect(clippingRectLeft, clippingRectTop, clippingRectRight, clippingRectBottom, x, y);
                dot.color = "BLUE";
            }
            else if (clippingType == 2 && hasClippingSquare)
            {
                inside = PointInsideSquare(clippingSquareCenter.x, clippingSquareCenter.y, clippingSquareSide, x, y);
                dot.color = "GREEN";
            }

            if (inside)
            {
                core.addShape(dot);          // persists via WM_PAINT
                InvalidateCanvas(hwnd);
                cout << "✓ Point (" << x << "," << y << ") INSIDE\n";
            }
            else
            {
                cout << "✗ Point (" << x << "," << y << ") OUTSIDE\n";
            }
            break;
        }

        if (isDrawingCircle)
        {
            if (!isDrawing) { circleCenter = { x,y }; isDrawing = true; cout << "  Circle CENTER\n"; }
            else
            {
                int r = (int)sqrt(pow(x - circleCenter.x, 2.0) + pow(y - circleCenter.y, 2.0)); Shape s; s.type = CIRCLE; s.algorithm = circleAlgorithm; s.color = pref.getCurrentColor(); s.radius = r; s.points.push_back(circleCenter); core.addShape(s); isDrawing = false; isDrawingCircle = false; InvalidateCanvas(hwnd); cout << "✓ Circle r=" << r << "\n";
            }
            break;
        }

        if (isDrawingEllipse)
        {
            if (!isDrawing) { ellipseCenter = { x,y }; isDrawing = true; cout << "  Ellipse CENTER\n"; }
            else
            {
                Shape s; s.type = ELLIPSE; s.algorithm = drawingAlgorithm; s.color = pref.getCurrentColor(); s.points.push_back(ellipseCenter); s.points.push_back({ x,y }); core.addShape(s); isDrawing = false; isDrawingEllipse = false; InvalidateCanvas(hwnd); cout << "✓ Ellipse\n";
            }
            break;
        }

        if (currentMode == ID_SMILE_HAPPY || currentMode == ID_SMILE_SAD)
        {
            Shape s; s.type = (currentMode == ID_SMILE_HAPPY) ? SMILEY_HAPPY : SMILEY_SAD;
            s.radius = 60; s.points.push_back({ x,y });
            core.addShape(s); InvalidateCanvas(hwnd); cout << "✓ Smiley\n"; break;
        }

        if (currentMode == ID_LINE_DDA || currentMode == ID_LINE_MID || currentMode == ID_LINE_PARAM)
        {
            HandleLineDrawing(hwnd, x, y); break;
        }
    }
    break;

    // ─────────────────────────────────────────────────────────────────────────
    case WM_LBUTTONUP:
    {
        GetGUI().OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);

        if (isDrawing && isDrawingClippingShape && (clippingType == 1 || clippingType == 2))
        {
            if (clippingType == 1)
            {
                clippingRectLeft = min(drawingStart.x, x); clippingRectRight = max(drawingStart.x, x); clippingRectTop = min(drawingStart.y, y); clippingRectBottom = max(drawingStart.y, y); hasClippingRect = true; cout << "✓ Rect clip set\n";
            }
            else
            {
                int side = max(abs(x - drawingStart.x), abs(y - drawingStart.y)); clippingSquareCenter = { drawingStart.x + (x - drawingStart.x) / 2,drawingStart.y + (y - drawingStart.y) / 2 }; clippingSquareSide = side; hasClippingSquare = true; cout << "✓ Square clip\n";
            }
            isDrawingClippingShape = false; clippingEnabled = true; isDrawing = false;
            InvalidateCanvas(hwnd); break;
        }

        if (IsFilling()) HandleFillMouseUp(hwnd, x, y);
    }
    break;

    // ─────────────────────────────────────────────────────────────────────────
    case WM_MOUSEMOVE:
    {
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        GetGUI().OnMouseMove(x, y);

        static int lx = -9, ly = -9;
        if (abs(x - lx) <= 2 && abs(y - ly) <= 2) break;
        lx = x; ly = y;

        clippingPreviewEnd = { x, y };
        drawingEnd = { x, y };
        HandleFillMouseMove(hwnd, x, y);

        bool need =
            (isDrawing && isDrawingClippingShape) ||
            (isDrawingClippingShape && clippingType == 0 && hasClippingCircle && !clippingEnabled) ||
            (isDrawing && isDrawingCircle) ||
            (isDrawing && isDrawingEllipse) ||
            (isDrawingCurve && !curvePoints.empty()) ||
            (isDrawing && (currentMode == ID_LINE_DDA || currentMode == ID_LINE_MID || currentMode == ID_LINE_PARAM));

        if (need) InvalidateCanvas(hwnd);
    }
    break;

    // ─────────────────────────────────────────────────────────────────────────
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT fullRc; GetClientRect(hwnd, &fullRc);

        // Double-buffer
        HDC     memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, fullRc.right, fullRc.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

        // 1. Dark background
        HBRUSH db = CreateSolidBrush(RGB(14, 14, 18));
        FillRect(memDC, &fullRc, db); DeleteObject(db);

        // 2. GUI chrome
        GetGUI().Draw(memDC);

        // 3. White canvas
        RECT rc = GetGUI().GetCanvasRect(hwnd);
        FillRect(memDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

        // 4. Clip to canvas
        HRGN canvasRgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
        SelectClipRgn(memDC, canvasRgn);

        // 5. Redraw every shape from core (fills are redrawn here every frame)
        for (const auto& s : core.getAllShapes())
            DrawShapeWithClipping(memDC, s, clippingEnabled && (isLineClippingMode || isPolygonClippingMode));

        // ── Helpers ──────────────────────────────────────────────────────────
        auto DashedRect = [&](int l, int t, int r2, int b, COLORREF col)
            {
                HPEN p = CreatePen(PS_DASH, 2, col); HPEN op = (HPEN)SelectObject(memDC, p);
                HBRUSH ob = (HBRUSH)SelectObject(memDC, GetStockObject(NULL_BRUSH));
                Rectangle(memDC, l, t, r2, b);
                SelectObject(memDC, op); SelectObject(memDC, ob); DeleteObject(p);
            };
        auto DashedEllipse = [&](int cx, int cy, int r, COLORREF col)
            {
                HPEN p = CreatePen(PS_DASH, 2, col); HPEN op = (HPEN)SelectObject(memDC, p);
                HBRUSH ob = (HBRUSH)SelectObject(memDC, GetStockObject(NULL_BRUSH));
                Ellipse(memDC, cx - r, cy - r, cx + r, cy + r);
                SelectObject(memDC, op); SelectObject(memDC, ob); DeleteObject(p);
            };

        // 6. Clipping overlays
        if (hasClippingCircle && clippingType == 0)
        {
            if (isDrawingClippingShape && !clippingEnabled)
            {
                int dx = clippingPreviewEnd.x - clippingCircleCenter.x, dy = clippingPreviewEnd.y - clippingCircleCenter.y; DashedEllipse(clippingCircleCenter.x, clippingCircleCenter.y, (int)sqrt((double)(dx * dx + dy * dy)), RGB(255, 80, 80));
            }
            else DashedEllipse(clippingCircleCenter.x, clippingCircleCenter.y, clippingCircleRadius, RGB(255, 80, 80));
        }
        if (clippingType == 1)
        {
            if (isDrawingClippingShape && isDrawing) DashedRect(min(drawingStart.x, clippingPreviewEnd.x), min(drawingStart.y, clippingPreviewEnd.y), max(drawingStart.x, clippingPreviewEnd.x), max(drawingStart.y, clippingPreviewEnd.y), RGB(80, 80, 255));
            else if (hasClippingRect) DashedRect(clippingRectLeft, clippingRectTop, clippingRectRight, clippingRectBottom, RGB(80, 80, 255));
        }
        if (clippingType == 2)
        {
            if (isDrawingClippingShape && isDrawing)
            {
                int side = max(abs(clippingPreviewEnd.x - drawingStart.x), abs(clippingPreviewEnd.y - drawingStart.y)); int cx = drawingStart.x + (clippingPreviewEnd.x - drawingStart.x) / 2, cy = drawingStart.y + (clippingPreviewEnd.y - drawingStart.y) / 2; DashedRect(cx - side / 2, cy - side / 2, cx + side / 2, cy + side / 2, RGB(0, 200, 0));
            }
            else if (hasClippingSquare) { int h = clippingSquareSide / 2; DashedRect(clippingSquareCenter.x - h, clippingSquareCenter.y - h, clippingSquareCenter.x + h, clippingSquareCenter.y + h, RGB(0, 200, 0)); }
        }

        // 7. Fill rubber-band preview
        DrawFillPreview(memDC);

        // 8. Curve preview
        if (isDrawingCurve && !curvePoints.empty())
        {
            HPEN p = CreatePen(PS_DASH, 2, RGB(255, 0, 255)); HPEN op = (HPEN)SelectObject(memDC, p);
            HBRUSH b = CreateSolidBrush(RGB(255, 0, 255)); HBRUSH ob = (HBRUSH)SelectObject(memDC, b);
            for (auto& pt : curvePoints) Ellipse(memDC, pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3);
            MoveToEx(memDC, curvePoints.back().x, curvePoints.back().y, NULL); LineTo(memDC, drawingEnd.x, drawingEnd.y);
            SelectObject(memDC, op); SelectObject(memDC, ob); DeleteObject(p); DeleteObject(b);
        }

        // 9. Circle draw preview
        if (isDrawing && isDrawingCircle)
        {
            int r = (int)sqrt(pow(drawingEnd.x - circleCenter.x, 2.0) + pow(drawingEnd.y - circleCenter.y, 2.0)); DashedEllipse(circleCenter.x, circleCenter.y, r, RGB(255, 100, 0));
        }

        // 10. Ellipse draw preview
        if (isDrawing && isDrawingEllipse)
        {
            int rx = abs(drawingEnd.x - ellipseCenter.x), ry = abs(drawingEnd.y - ellipseCenter.y);
            HPEN p = CreatePen(PS_DASH, 2, RGB(0, 100, 255)); HPEN op = (HPEN)SelectObject(memDC, p);
            HBRUSH ob = (HBRUSH)SelectObject(memDC, GetStockObject(NULL_BRUSH));
            Ellipse(memDC, ellipseCenter.x - rx, ellipseCenter.y - ry, ellipseCenter.x + rx, ellipseCenter.y + ry);
            SelectObject(memDC, op); SelectObject(memDC, ob); DeleteObject(p);
        }

        // 11. Line preview
        if (!IsFilling() && !isDrawingEllipse && !isDrawingCircle && !isDrawingClippingShape && !isDrawingCurve &&
            (currentMode == ID_LINE_DDA || currentMode == ID_LINE_MID || currentMode == ID_LINE_PARAM))
            DrawPreview(memDC, drawingEnd.x, drawingEnd.y);

        // 12. Hint text
        SetBkMode(memDC, TRANSPARENT);
        HFONT font = CreateFont(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT oldFont = (HFONT)SelectObject(memDC, font);
        auto Hint = [&](const char* m, COLORREF c) { SetTextColor(memDC, c); TextOutA(memDC, rc.left + 10, rc.top + 10, m, (int)strlen(m)); };

        if (isDrawingCurve) Hint("Click to add points  |  Right-click to finish curve", RGB(255, 0, 255));
        else if (IsFilling())
        {
            int fm = GetFillMode();
            if (fm == ID_FILL_CONVEX) Hint("Click vertices  |  Right-click to fill", RGB(0, 200, 150));
            else if (fm == ID_FILL_CIRC_LINE || fm == ID_FILL_CIRC_CIRC)
            {
                static char buf[160]; const char* qn[] = { "","Top-Right","Top-Left","Bottom-Left","Bottom-Right" }; int q = GetFillQuarter(); sprintf(buf, "Click CENTER then RADIUS  |  Quarter: %s  (use menu to change)", (q >= 1 && q <= 4) ? qn[q] : "?"); Hint(buf, RGB(0, 150, 220));
            }
            else if (fm == ID_FILL_SQ_HERMIT || fm == ID_FILL_RECT_BEZIER) Hint("Click and drag to define fill area", RGB(0, 200, 100));
            else if (fm == ID_FILL_FLOOD) Hint("Click INSIDE a closed shape to flood fill", RGB(255, 180, 0));
        }
        else if (isDrawingClippingShape && clippingType == 0 && !hasClippingCircle) Hint("Click to set clipping circle CENTER", RGB(255, 100, 100));
        else if (isDrawingClippingShape && clippingType == 0 && hasClippingCircle && !clippingEnabled) Hint("Click to set clipping circle RADIUS", RGB(255, 100, 100));
        else if (isDrawingClippingShape && (clippingType == 1 || clippingType == 2)) Hint("Click and drag to draw the clipping window", RGB(100, 150, 255));
        else if (isPointClippingMode && clippingEnabled) Hint("Click points – coloured dot appears if INSIDE", RGB(0, 200, 180));
        else if (isLineClippingMode && clippingEnabled)  Hint("Draw lines – clipped to window", RGB(0, 200, 0));
        else if (isPolygonClippingMode && clippingEnabled) Hint("Draw polygons – clipped", RGB(255, 160, 0));
        else if (isDrawing && isDrawingCircle)  Hint("Click to set circle RADIUS", RGB(255, 100, 0));
        else if (isDrawing && isDrawingEllipse) Hint("Click to set ellipse end point", RGB(0, 100, 255));
        else if (isDrawing)                   Hint("Click to finish the line", RGB(150, 180, 255));

        SelectObject(memDC, oldFont); DeleteObject(font);

        // 13. Blit to screen
        SelectClipRgn(memDC, NULL); DeleteObject(canvasRgn);
        BitBlt(hdc, 0, 0, fullRc.right, fullRc.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp); DeleteObject(memBmp); DeleteDC(memDC);

        // 14. Flood fill AFTER BitBlt
        // The real screen HDC now has all shapes visible.
        // FloodFillStack reads existing pixel colours so it MUST run here,
        // not earlier (when the canvas was still being built in the memDC).
        if (HasPendingFloodFill())
            ExecutePendingFloodFill(hdc);

        // 15. Status bar
        if (isDrawingCurve)
        {
            wchar_t b[64]; swprintf(b, 64, L"Points: %d", (int)curvePoints.size()); GetGUI().SetStatus(L"Click | Right-click finish", L"Cardinal Spline", b);
        }
        else if (IsFilling())
        {
            int fm = GetFillMode(); const wchar_t* qn[] = { L"",L"Top-Right",L"Top-Left",L"Bottom-Left",L"Bottom-Right" }; int q = GetFillQuarter();
            if (fm == ID_FILL_CIRC_LINE || fm == ID_FILL_CIRC_CIRC) GetGUI().SetStatus(L"Click CENTER then RADIUS", L"Circle Fill", (q >= 1 && q <= 4) ? qn[q] : L"");
            else if (fm == ID_FILL_FLOOD) GetGUI().SetStatus(L"Click inside shape", L"Flood Fill", L"");
            else if (fm == ID_FILL_CONVEX) GetGUI().SetStatus(L"Click vertices | Right-click", L"Polygon Fill", L"");
            else GetGUI().SetStatus(L"Click & drag", L"Fill", L"");
        }
        else if (clippingEnabled && isPointClippingMode)   GetGUI().SetStatus(L"Click to test points", L"Point Clipping", L"ACTIVE");
        else if (clippingEnabled && isLineClippingMode)    GetGUI().SetStatus(L"Draw clipped lines", L"Line Clipping", L"ACTIVE");
        else if (clippingEnabled && isPolygonClippingMode) GetGUI().SetStatus(L"Draw clipped polygons", L"Polygon Clipping", L"ACTIVE");
        else if (isDrawingClippingShape) GetGUI().SetStatus(L"Draw clipping window", L"Clipping Setup", L"...");
        else if (isDrawing)              GetGUI().SetStatus(L"Click to finish", L"", L"Drawing");
        else                             GetGUI().SetStatus(L"Ready", L"", L"No clipping");

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        GetGUI().Destroy();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// =============================================================================
//  WinMain
// =============================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    MessageBoxW(NULL, L"NEW BUILD LOADED", L"DEBUG", MB_OK);
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GraphicsApp";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"GraphicsApp", L"3D Drawing Package – Complete",
        WS_OVERLAPPEDWINDOW, 100, 100, 1280, 820, NULL, NULL, hInstance, NULL);

    GetGUI().Setup(hwnd);
    CreateAppMenu(hwnd);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); DispatchMessage(&msg);
    }
    return 0;
}