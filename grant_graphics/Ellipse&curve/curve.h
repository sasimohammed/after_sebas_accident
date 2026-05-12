#pragma once
#include <Windows.h>
#include <vector>

void DrawHermiteSegment(HDC hdc, POINT P0, POINT T0, POINT P1, POINT T1, COLORREF color);
void DrawCardinalSpline(HDC hdc, POINT p[], int n, COLORREF color);