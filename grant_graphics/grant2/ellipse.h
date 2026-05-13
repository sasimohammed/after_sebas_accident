#pragma once
#include <windows.h>

void drawEllipseDirect(HDC hdc, int xc, int yc, int a, int b, COLORREF color);
void drawEllipsePolar(HDC hdc, int xc, int yc, int a, int b, COLORREF color);
void drawEllipseMidpoint(HDC hdc, int xc, int yc, int a, int b, COLORREF color);