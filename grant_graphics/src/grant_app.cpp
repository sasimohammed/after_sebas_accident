#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <iostream>

#include "core_data.h"
#include "file_system.h"
#include "preferences.h"
#include "menu.h"

using namespace std;

// ================= GLOBAL =================
CoreData core;
Preferences pref;

// ================= CURRENT STATE (FOR TEAM LATER) =================
int currentMode = -1;

// ================= INIT =================
void Init()
{
    pref.setCurrentColor("BLACK");
    pref.setBackground("WHITE");

    cout << "[LOG] System Initialized\n";
}

// ================= DRAW ONLY =================
void DrawShape(HDC hdc, const Shape& s)
{
    if (s.type == LINE && s.points.size() >= 2)
    {
        MoveToEx(hdc, s.points[0].x, s.points[0].y, NULL);
        LineTo(hdc, s.points[1].x, s.points[1].y);
    }
}

// ================= WNDPROC =================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // ================= MENU =================
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            // ===== FILE =====
        case ID_FILE_CLEAR:
            core.clearAll();
            cout << "Clear Screen\n";
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case ID_FILE_SAVE:
            FileSystem::save(core, "data.txt");
            cout << "Save File\n";
            break;

        case ID_FILE_LOAD:
            FileSystem::load(core, "data.txt");
            cout << "Load File\n";
            InvalidateRect(hwnd, NULL, TRUE);
            break;

            // ===== PREF =====
        case ID_PREF_BG:
            pref.setBackground("WHITE");
            cout << "Background Set\n";
            break;

        case ID_PREF_CURSOR:
            SetClassLongPtr(hwnd, GCLP_HCURSOR,
                (LONG_PTR)LoadCursor(NULL, IDC_HAND));
            cout << "Cursor Changed\n";
            break;

        case ID_PREF_COLOR:
            pref.setCurrentColor("RED");
            cout << "Color Selected\n";
            break;

            // ================= LINES (OPTIONS ONLY) =================
        case ID_LINE_DDA:
            currentMode = ID_LINE_DDA;
            cout << "Mode: DDA Line\n";
            break;

        case ID_LINE_MID:
            currentMode = ID_LINE_MID;
            cout << "Mode: Midpoint Line\n";
            break;

        case ID_LINE_PARAM:
            currentMode = ID_LINE_PARAM;
            cout << "Mode: Parametric Line\n";
            break;

            // ================= CIRCLES =================
        case ID_CIRC_DIRECT:
            currentMode = ID_CIRC_DIRECT;
            cout << "Mode: Direct Circle\n";
            break;

        case ID_CIRC_POLAR:
            currentMode = ID_CIRC_POLAR;
            cout << "Mode: Polar Circle\n";
            break;

        case ID_CIRC_MID:
            currentMode = ID_CIRC_MID;
            cout << "Mode: Midpoint Circle\n";
            break;

            // ================= ELLIPSE =================
        case ID_ELLIPSE_DIR:
            currentMode = ID_ELLIPSE_DIR;
            cout << "Mode: Direct Ellipse\n";
            break;

        case ID_ELLIPSE_POL:
            currentMode = ID_ELLIPSE_POL;
            cout << "Mode: Polar Ellipse\n";
            break;

        case ID_ELLIPSE_MID:
            currentMode = ID_ELLIPSE_MID;
            cout << "Mode: Midpoint Ellipse\n";
            break;

            // ================= FILLING =================
        case ID_FILL_FLOOD:
            currentMode = ID_FILL_FLOOD;
            cout << "Mode: Flood Fill\n";
            break;

        case ID_FILL_CONVEX:
            currentMode = ID_FILL_CONVEX;
            cout << "Mode: Convex Fill\n";
            break;

            // ================= CLIPPING =================
        case ID_CLIP_RECT:
            currentMode = ID_CLIP_RECT;
            cout << "Mode: Rectangle Clipping\n";
            break;

        case ID_CLIP_POINT:
            currentMode = ID_CLIP_POINT;
            cout << "Mode: Point Clipping\n";
            break;
        }
        break;

        // ================= NO DRAWING ON CLICK =================
    case WM_LBUTTONDOWN:
        cout << "Mouse clicked (NO DRAWING - mode only)\n";
        break;

        // ================= RENDER =================
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        for (const auto& s : core.getAllShapes())
        {
            DrawShape(hdc, s);
        }

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

// ================= WINMAIN =================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Init();

    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GraphicsApp";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)WHITE_BRUSH;

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        L"GraphicsApp",
        L"2D Drawing Package",
        WS_OVERLAPPEDWINDOW,
        100, 100, 1000, 700,
        NULL, NULL, hInstance, NULL
    );

    CreateAppMenu(hwnd);

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}