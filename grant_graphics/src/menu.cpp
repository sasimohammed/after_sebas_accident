#include "menu.h"

void CreateAppMenu(HWND hwnd)
{
    HMENU mainMenu = CreateMenu();

    // ================= FILE =================
    HMENU fileMenu = CreateMenu();
    AppendMenu(fileMenu, MF_STRING, ID_FILE_CLEAR, L"Clear Screen");
    AppendMenu(fileMenu, MF_STRING, ID_FILE_SAVE, L"Save");
    AppendMenu(fileMenu, MF_STRING, ID_FILE_LOAD, L"Load");

    // ================= PREF =================
    HMENU prefMenu = CreateMenu();
    AppendMenu(prefMenu, MF_STRING, ID_PREF_BG, L"White Background");
    AppendMenu(prefMenu, MF_STRING, ID_PREF_CURSOR, L"Change Cursor");
    AppendMenu(prefMenu, MF_STRING, ID_PREF_COLOR, L"Choose Color");

    // ================= LINES =================
    HMENU lineMenu = CreateMenu();
    AppendMenu(lineMenu, MF_STRING, ID_LINE_DDA, L"DDA Line");
    AppendMenu(lineMenu, MF_STRING, ID_LINE_MID, L"Midpoint Line");
    AppendMenu(lineMenu, MF_STRING, ID_LINE_PARAM, L"Parametric Line");

    // ================= CIRCLES =================
    HMENU circleMenu = CreateMenu();
    AppendMenu(circleMenu, MF_STRING, ID_CIRC_DIRECT, L"Direct Circle");
    AppendMenu(circleMenu, MF_STRING, ID_CIRC_POLAR, L"Polar Circle");
    AppendMenu(circleMenu, MF_STRING, ID_CIRC_MID, L"Midpoint Circle");

    // ================= ELLIPSE =================
    HMENU ellipseMenu = CreateMenu();
    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_DIR, L"Direct Ellipse");
    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_POL, L"Polar Ellipse");
    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_MID, L"Midpoint Ellipse");

    // ================= FILLING =================
    HMENU fillMenu = CreateMenu();
    AppendMenu(fillMenu, MF_STRING, ID_FILL_FLOOD, L"Flood Fill");
    AppendMenu(fillMenu, MF_STRING, ID_FILL_CONVEX, L"Convex Fill");

    // ================= CLIPPING =================
    HMENU clipMenu = CreateMenu();
    AppendMenu(clipMenu, MF_STRING, ID_CLIP_RECT, L"Rectangle Clipping");
    AppendMenu(clipMenu, MF_STRING, ID_CLIP_POINT, L"Point Clipping");

    // ================= ATTACH =================
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)fileMenu, L"File");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)prefMenu, L"Preferences");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)lineMenu, L"Lines");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)circleMenu, L"Circles");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)ellipseMenu, L"Ellipses");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)fillMenu, L"Filling");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)clipMenu, L"Clipping");

    SetMenu(hwnd, mainMenu);
}