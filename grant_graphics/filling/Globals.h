#pragma once
#include <windows.h>
#include <cmath>
#include <vector>
#include <list>
#include <stack>
#include <iostream>
#include <climits>
#include <string>
using namespace std;

#define MAXENTRIES   2000
#define PI           3.14159265358979323846
#define Round(x)     ((int)((x) + 0.5))

extern HDC    g_hdc;
extern HWND   g_hwnd;

extern int    g_menu;
extern int    g_sub;
extern int    g_quarter;
extern int    g_clickCount;
extern int    g_pts[20][2];

extern int    g_clipLeft, g_clipTop, g_clipRight, g_clipBottom;
extern bool   g_shapeClosed;
extern vector<POINT> g_poly;
extern std::vector<POINT> g_floodPoly;
extern std::vector<POINT> g_clipPoly;
extern int g_clipObjectType;
extern bool g_clipModeEnabled;

// Clipping type flags
extern bool g_isCircleClip;
extern bool g_isRectClip;
extern bool g_isSquareClip;

// Circle clip data
extern int g_clipCenterX, g_clipCenterY, g_clipRadius;

// Square clip data
extern int g_clipSize;

#define C_DRAW    RGB(0,   0,   200)
#define C_FILL    RGB(0,   160,  60)
#define C_FLOOD   RGB(220,  80,   0)
#define C_BOUND   RGB(0,    0,    0)
#define C_CLIPWIN RGB(200,   0,    0)
#define C_CLIPOUT RGB(0,   255,  0)