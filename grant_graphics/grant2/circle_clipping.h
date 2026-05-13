#pragma once
#include <windows.h>
#include <vector>

// ================= CIRCLE CLIPPING =================
bool PointInsideCircle(int xc, int yc, int r, int x, int y);
void CirclePointClipping(HDC hdc, int xc, int yc, int r, int x, int y);
void CircleLineClipping(HDC hdc, int xc, int yc, int r, int x1, int y1, int x2, int y2);

// ================= RECTANGLE CLIPPING =================
bool PointInsideRect(int left, int top, int right, int bottom, int x, int y);
void RectPointClipping(HDC hdc, int left, int top, int right, int bottom, int x, int y);
void RectLineClipping(HDC hdc, int left, int top, int right, int bottom, int x1, int y1, int x2, int y2);
void RectPolygonClipping(HDC hdc, int left, int top, int right, int bottom, int x1, int y1, int x2, int y2, int x3, int y3);

// ================= SQUARE CLIPPING =================
bool PointInsideSquare(int cx, int cy, int side, int x, int y);
void SquarePointClipping(HDC hdc, int cx, int cy, int side, int x, int y);
void SquareLineClipping(HDC hdc, int cx, int cy, int side, int x1, int y1, int x2, int y2);