#pragma once
#include <windows.h>
#include "core_data.h"
#include "preferences.h"

// Global variables for drawing (external)
extern CoreData core;
extern Preferences pref;
extern int currentMode;

// initialize mouse handler
void InitMouseHandler();

// handle mouse clicks for LINE drawing
void HandleLineDrawing(HWND hwnd, int x, int y);

// reset drawing state (called when mode changes)
void ResetDrawingState();

// draw preview while dragging (optional)
void DrawPreview(HDC hdc, int x, int y);