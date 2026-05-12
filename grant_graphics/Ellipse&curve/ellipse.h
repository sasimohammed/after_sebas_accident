#pragma once
#include <windows.h>

void drawPoints(HDC hdc, int xc, int yc, int x, int y, COLORREF color);
void drawEllipseDirect(HDC hdc, int xc, int yc, int a, int b, COLORREF color);
void drawEllipsePolar(HDC hdc, int xc, int yc, int a, int b, COLORREF color);
void drawEllipseMidpoint(HDC hdc, int xc, int yc, int a, int b, COLORREF color);