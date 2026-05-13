#pragma once
#include <windows.h>
#include <vector>

using namespace std;

// Global HDC and HWND
extern HDC g_hdc;
extern HWND g_hwnd;

// Menu state
extern int g_menu;
extern int g_sub;
extern int g_quarter;

// Click tracking
extern int g_clickCount;
extern int g_pts[20][2];

// Polygon storage
extern vector<POINT> g_poly;
extern vector<POINT> g_floodPoly;
extern bool g_shapeClosed;

// Clipping globals
extern vector<POINT> g_clipPoly;
extern int g_clipObjectType;
extern bool g_clipModeEnabled;
extern bool g_isCircleClip;
extern bool g_isRectClip;
extern bool g_isSquareClip;
extern int g_clipCenterX;
extern int g_clipCenterY;
extern int g_clipRadius;
extern int g_clipSize;
extern int g_clipLeft;
extern int g_clipTop;
extern int g_clipRight;
extern int g_clipBottom;

// Color constants
#define C_FILL RGB(0, 120, 220)
#define C_BOUND RGB(0, 0, 0)
#define C_FLOOD RGB(0, 120, 220)
#define C_CLIPWIN RGB(255, 100, 100)