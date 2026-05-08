#pragma once
#include <windows.h>

// File
#define ID_FILE_CLEAR   1
#define ID_FILE_SAVE    2
#define ID_FILE_LOAD    3

// Preferences
#define ID_PREF_BG      10
#define ID_PREF_CURSOR  11
#define ID_PREF_COLOR   12

// Lines
#define ID_LINE_DDA     20
#define ID_LINE_MID     21
#define ID_LINE_PARAM   22

// Circles
#define ID_CIRC_DIRECT  30
#define ID_CIRC_POLAR   31
#define ID_CIRC_MID     32

// Ellipse
#define ID_ELLIPSE_DIR  40
#define ID_ELLIPSE_POL  41
#define ID_ELLIPSE_MID  42

// Filling
#define ID_FILL_FLOOD   50
#define ID_FILL_CONVEX  51

// Clipping
#define ID_CLIP_RECT    60
#define ID_CLIP_POINT   61

void CreateAppMenu(HWND hwnd);