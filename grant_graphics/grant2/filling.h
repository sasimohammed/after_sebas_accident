#pragma once
#include <windows.h>
#include "core_data.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Colour constants (shared with main.cpp)
// ─────────────────────────────────────────────────────────────────────────────
static const COLORREF FILL_COLOR = RGB(0, 120, 220);
static const COLORREF BOUND_COLOR = RGB(0, 0, 0);

#ifndef MAXENTRIES
#define MAXENTRIES 2000
#endif

// ─────────────────────────────────────────────────────────────────────────────
//  a) Fill circle with lines (one quarter)
//     quarter: 1=Top-Right  2=Top-Left  3=Bottom-Left  4=Bottom-Right
// ─────────────────────────────────────────────────────────────────────────────
void FillCircleWithLines(HDC hdc, int cx, int cy, int r, int quarter);

// ─────────────────────────────────────────────────────────────────────────────
//  b) Fill circle with concentric circles (one quarter)
// ─────────────────────────────────────────────────────────────────────────────
void FillCircleWithCircles(HDC hdc, int cx, int cy, int r, int quarter);

// ─────────────────────────────────────────────────────────────────────────────
//  c) Fill square with Hermit curves (vertical strokes)
// ─────────────────────────────────────────────────────────────────────────────
void FillSquareWithHermit(HDC hdc, int left, int top, int right, int bottom);

// ─────────────────────────────────────────────────────────────────────────────
//  d) Fill rectangle with Bezier curves (horizontal strokes)
// ─────────────────────────────────────────────────────────────────────────────
void FillRectWithBezier(HDC hdc, int left, int top, int right, int bottom);

// ─────────────────────────────────────────────────────────────────────────────
//  e-1) Convex polygon fill (scan-line, works for convex shapes)
// ─────────────────────────────────────────────────────────────────────────────
void ConvexFill(HDC hdc, POINT pts[], int n, COLORREF color);

// ─────────────────────────────────────────────────────────────────────────────
//  e-2) General (non-convex) polygon fill (active edge table)
// ─────────────────────────────────────────────────────────────────────────────
void GeneralPolygonFill(HDC hdc, POINT pts[], int n, COLORREF color);

// ─────────────────────────────────────────────────────────────────────────────
//  f-1) Recursive flood fill  (use with care – may stack-overflow on large areas)
// ─────────────────────────────────────────────────────────────────────────────
void FloodFillRecursive(HDC hdc, int x, int y, COLORREF boundary, COLORREF fillColor);

// ─────────────────────────────────────────────────────────────────────────────
//  f-2) Non-recursive (stack-based) flood fill  – preferred
// ─────────────────────────────────────────────────────────────────────────────
void FloodFillStack(HDC hdc, int x, int y, COLORREF boundary, COLORREF fillColor);