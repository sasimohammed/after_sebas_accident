// src/mouse_handler.cpp
#include "mouse_handler.h"
#include "lines.h"
#include <iostream>
using namespace std;

// Static variables for drawing state (only accessible inside this file)
static Point startPoint;
static bool isDrawing = false;

void InitMouseHandler()
{
    isDrawing = false;
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

void ResetDrawingState()
{
    isDrawing = false;
    cout << "[MouseHandler] Drawing state reset\n";
}

void DrawPreview(HDC hdc, int x, int y)
{
    // Optional: draw preview line while dragging
    if (isDrawing)
    {
        COLORREF color;
        string prefColor = pref.getCurrentColor();
        if (prefColor == "RED") color = RGB(255,0,0);
        else color = RGB(0,0,0);
        
        // Draw temporary line (dotted or different style)
        HPEN hPen = CreatePen(PS_DOT, 1, RGB(128,128,128));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        
        MoveToEx(hdc, startPoint.x, startPoint.y, NULL);
        LineTo(hdc, x, y);
        
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
}