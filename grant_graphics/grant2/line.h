
#include <windows.h>
#include "shape.h"

void DrawLine_DDA(HDC hdc, Point p1, Point p2, COLORREF color);
void DrawLine_Midpoint(HDC hdc, Point p1, Point p2, COLORREF color);
void DrawLine_Parametric(HDC hdc, Point p1, Point p2, COLORREF color);