#pragma once
#include <windows.h>
#include "core_data.h"
#include "preferences.h"
#include "filling.h"
#include "menu.h"

extern CoreData    core;
extern Preferences pref;
extern int         currentMode;

// Startup
void InitMouseHandler();

// Fill mode / quarter (called from WM_COMMAND)
void SetFillMode(HWND hwnd, int menuId);
void SetFillQuarter(int quarter);   // 1=Top-Right  2=Top-Left  3=Bot-Left  4=Bot-Right

// Line drawing
void HandleLineDrawing(HWND hwnd, int x, int y);

// Fill interaction
bool HandleFillClick(HWND hwnd, int x, int y);      // WM_LBUTTONDOWN
void HandleFillRightClick(HWND hwnd);               // WM_RBUTTONDOWN (polygon finish)
void HandleFillMouseUp(HWND hwnd, int x, int y);    // WM_LBUTTONUP   (drag fills)
void HandleFillMouseMove(HWND hwnd, int x, int y);  // WM_MOUSEMOVE   (preview)

// Paint helpers
void DrawFillPreview(HDC hdc);                      // rubber-band during WM_PAINT
void DrawPreview(HDC hdc, int x, int y);            // line drag preview

// Flood fill: executed on the real screen HDC AFTER BitBlt in WM_PAINT
bool HasPendingFloodFill();
void ExecutePendingFloodFill(HDC screenHDC);

// Reset (mode change / clear)
void ResetDrawingState();

// Query
bool IsFilling();
int  GetFillMode();
int  GetFillQuarter();