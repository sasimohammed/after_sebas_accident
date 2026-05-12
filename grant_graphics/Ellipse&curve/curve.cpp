#include "curve.h"
#include <cmath>

void DrawHermiteSegment(HDC hdc, POINT P0, POINT T0, POINT P1, POINT T1, COLORREF color) {
    for (double t = 0; t <= 1; t += 0.001) {
        double h00 =  2 * pow(t, 3) - 3 * pow(t, 2) + 1;
        double h01 = -2 * pow(t, 3) + 3 * pow(t, 2);
        double h10 =      pow(t, 3) - 2 * pow(t, 2) + t;
        double h11 =      pow(t, 3) -     pow(t, 2);

        double x = h00 * P0.x + h01 * P1.x + h10 * T0.x + h11 * T1.x;
        double y = h00 * P0.y + h01 * P1.y + h10 * T0.y + h11 * T1.y;

        SetPixel(hdc, (int)x, (int)y, color);
    }
}
void DrawCardinalSpline(HDC hdc, POINT p[], int n, COLORREF color) {
    if (n < 2) return;

    double c  = 0.2;
    double c1 = 1 - c;     

    for (int i = 0; i < n - 1; i++) {

        POINT Pprev;
        if (i == 0) {
            Pprev.x = 2 * p[0].x - p[1].x;
            Pprev.y = 2 * p[0].y - p[1].y;
        } else {
            Pprev = p[i - 1];
        }

        POINT Pnext;
        if (i == n - 2) {
            Pnext.x = 2 * p[n-1].x - p[n-2].x;
            Pnext.y = 2 * p[n-1].y - p[n-2].y;
        } else {
            Pnext = p[i + 2];
        }
        POINT T0, T1;
        T0.x = (LONG)(c1 * (p[i+1].x - Pprev.x) / 2);
        T0.y = (LONG)(c1 * (p[i+1].y - Pprev.y) / 2);
        T1.x = (LONG)(c1 * (Pnext.x  - p[i].x)  / 2);
        T1.y = (LONG)(c1 * (Pnext.y  - p[i].y)  / 2);

        DrawHermiteSegment(hdc, p[i], T0, p[i+1], T1, color);
    }
}