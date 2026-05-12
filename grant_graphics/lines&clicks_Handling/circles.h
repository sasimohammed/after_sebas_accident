
#include <windows.h>
#include "shape.h"

void CircleDirect(HDC hdc, int xc, int yc, int R, COLORREF color);
void CirclePolar(HDC hdc, int xc, int yc, int R, COLORREF color);
void CircleIterativePolar(HDC hdc, int xc, int yc, int R, COLORREF color);
void CircleMidPoint(HDC hdc, int xc, int yc, int R, COLORREF color);
void CircleModifiedMidPoint(HDC hdc, int xc, int yc, int R, COLORREF color);