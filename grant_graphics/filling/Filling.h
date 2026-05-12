#pragma once
#include "Globals.h"

void FillCircleWithLines(int cx, int cy, int r, int quarter);
void FillCircleWithCircles(int cx, int cy, int r, int quarter);
void FillSquareWithHermit(int left, int top, int right, int bottom);
void FillRectWithBezier(int left, int top, int right, int bottom);
void ConvexFill(POINT p[], int n, COLORREF color);
void GeneralPolygonFill(POINT polygon[], int n, COLORREF color);
void FloodFillRecursive(int x, int y, COLORREF cb, COLORREF cf);
void FloodFillNonRecursive(int x, int y, COLORREF cb, COLORREF cf);
void FloodFillPolygon(const std::vector<POINT>& poly, COLORREF fillColor);
void DrawSolidBoundary(const std::vector<POINT>& poly);