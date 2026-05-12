// mouse_handler.cpp
#include "mouse_handler.h"
#include "lines.h"
#include <iostream>
#include <cmath>
using namespace std;

// Static variables for drawing state (only accessible inside this file)
static Point startPoint;
static bool isDrawing = false;
static bool isDrawingCircle = false;

void InitMouseHandler()
{
    isDrawing = false;
    isDrawingCircle = false;
    cout << "[MouseHandler] Initialized\n";
}

void HandleLineDrawing(HWND hwnd, int x, int y)
{
    if (!isDrawing)
    {
        // First click - store start point
        startPoint = {x, y};
        isDrawing = true;
        cout << "LINE: First point (" << x << "," << y << ")\n";
    }
    else
    {
        // Second click - create and save the line
        Point endPoint = {x, y};
        
        Shape newShape;
        newShape.type = LINE;
        newShape.points.push_back(startPoint);
        newShape.points.push_back(endPoint);
        newShape.color = pref.getCurrentColor();
        
        core.addShape(newShape);
        isDrawing = false;
        
        // Force window to redraw
        InvalidateRect(hwnd, NULL, TRUE);
        
        cout << "LINE: Drawn from (" << startPoint.x << "," << startPoint.y 
             << ") to (" << x << "," << y << ")\n";
    }
}

void HandleCircleDrawing(HWND hwnd, int x, int y)
{
    if (!isDrawingCircle)
    {
        // First click - store center point
        startPoint = {x, y};
        isDrawingCircle = true;
        cout << "CIRCLE: Center point (" << x << "," << y << ")\n";
    }
    else
    {
        // Second click - calculate radius from center to this point
        Point endPoint = {x, y};
        
        // Calculate radius (distance between center and point on circumference)
        int dx = endPoint.x - startPoint.x;
        int dy = endPoint.y - startPoint.y;
        int radius = (int)round(sqrt(dx * dx + dy * dy));
        
        Shape newShape;
        newShape.type = CIRCLE;
        newShape.points.push_back(startPoint);  // center point
        newShape.points.push_back(endPoint);     // point on circumference
        newShape.radius = radius;
        newShape.color = pref.getCurrentColor();
        
        core.addShape(newShape);
        isDrawingCircle = false;
        
        // Force window to redraw
        InvalidateRect(hwnd, NULL, TRUE);
        
        cout << "CIRCLE: Center (" << startPoint.x << "," << startPoint.y 
             << ") Radius = " << radius << "\n";
    }
}

void ResetDrawingState()
{
    isDrawing = false;
    isDrawingCircle = false;
    cout << "[MouseHandler] Drawing state reset\n";
}

void DrawPreview(HDC hdc, int x, int y)
{
    // Preview for line drawing
    if (isDrawing)
    {
        COLORREF color;
        string prefColor = pref.getCurrentColor();
        if (prefColor == "RED") color = RGB(255,0,0);
        else color = RGB(0,0,0);
        
        // Draw temporary line (dotted style)
        HPEN hPen = CreatePen(PS_DOT, 1, RGB(128,128,128));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
        MoveToEx(hdc, startPoint.x, startPoint.y, NULL);
        LineTo(hdc, x, y);
        
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
    // Preview for circle drawing
    else if (isDrawingCircle)
    {
        COLORREF color;
        string prefColor = pref.getCurrentColor();
        if (prefColor == "RED") color = RGB(255,0,0);
        else color = RGB(0,0,0);
        
        // Calculate radius from center to current mouse position
        int dx = x - startPoint.x;
        int dy = y - startPoint.y;
        int radius = (int)round(sqrt(dx * dx + dy * dy));
        
        // Draw temporary circle outline (dotted style)
        HPEN hPen = CreatePen(PS_DOT, 1, RGB(128,128,128));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        Ellipse(hdc, startPoint.x - radius, startPoint.y - radius,
                     startPoint.x + radius, startPoint.y + radius);
        
        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
}