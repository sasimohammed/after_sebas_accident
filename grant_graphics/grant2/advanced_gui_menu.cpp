// advanced_gui_menu.cpp
// =============================================================================
//  Creates the Windows menu bar AND hooks WM_MOUSEWHEEL / WM_LBUTTONUP
//  into the GUI's sidebar scrollbar.
// =============================================================================
#include "advanced_gui.h"
#include "menu.h"

void CreateAppMenu(HWND hwnd)
{
    HMENU mainMenu = CreateMenu();

    // ── File ──────────────────────────────────────────────────────────────────
    HMENU fileMenu = CreateMenu();
    AppendMenu(fileMenu, MF_STRING, ID_FILE_CLEAR, L"Clear Screen\tCtrl+N");
    AppendMenu(fileMenu, MF_STRING, ID_FILE_SAVE, L"Save\tCtrl+S");
    AppendMenu(fileMenu, MF_STRING, ID_FILE_LOAD, L"Load\tCtrl+O");

    // ── Preferences ───────────────────────────────────────────────────────────
    HMENU prefMenu = CreateMenu();
    AppendMenu(prefMenu, MF_STRING, ID_PREF_BG, L"White Background");
    AppendMenu(prefMenu, MF_STRING, ID_PREF_CURSOR, L"Change Cursor");
    AppendMenu(prefMenu, MF_STRING, ID_PREF_COLOR, L"Choose Color (Red)");

    // ── Points ────────────────────────────────────────────────────────────────
    HMENU pointMenu = CreateMenu();  // Fixed: Define pointMenu
    AppendMenu(pointMenu, MF_STRING, ID_POINT_CIRCLE, L"Circle Point");
    AppendMenu(pointMenu, MF_STRING, ID_POINT_SQUARE, L"Square Point");
    AppendMenu(pointMenu, MF_STRING, ID_POINT_CROSSHAIR, L"Crosshair Point");
    AppendMenu(pointMenu, MF_STRING, ID_POINT_DIRECT, L"Direct Point (Pixel)");

    // ── Lines ─────────────────────────────────────────────────────────────────
    HMENU lineMenu = CreateMenu();
    AppendMenu(lineMenu, MF_STRING, ID_LINE_DDA, L"DDA Line");
    AppendMenu(lineMenu, MF_STRING, ID_LINE_MID, L"Midpoint Line");
    AppendMenu(lineMenu, MF_STRING, ID_LINE_PARAM, L"Parametric Line");

    // ── Circles ───────────────────────────────────────────────────────────────
    HMENU circleMenu = CreateMenu();
    AppendMenu(circleMenu, MF_STRING, ID_CIRC_DIRECT, L"Direct Circle");
    AppendMenu(circleMenu, MF_STRING, ID_CIRC_POLAR, L"Polar Circle");
    AppendMenu(circleMenu, MF_STRING, ID_CIRC_ITER_POLAR, L"Iterative Polar Circle");
    AppendMenu(circleMenu, MF_STRING, ID_CIRC_MID, L"Midpoint Circle");
    AppendMenu(circleMenu, MF_STRING, ID_CIRC_MOD_MID, L"Modified Midpoint Circle");

    // ── Ellipses ──────────────────────────────────────────────────────────────
    HMENU ellipseMenu = CreateMenu();
    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_DIR, L"Direct Ellipse");
    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_POL, L"Polar Ellipse");
    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_MID, L"Midpoint Ellipse");

    // ── Curves ────────────────────────────────────────────────────────────────
    HMENU curveMenu = CreateMenu();
    AppendMenu(curveMenu, MF_STRING, ID_CARDINAL_SPLINE, L"Cardinal Spline Curve");

    // ── Filling ───────────────────────────────────────────────────────────────
    HMENU fillMenu = CreateMenu();
    AppendMenu(fillMenu, MF_STRING, ID_FILL_CIRC_LINE, L"Circle w/ Lines (choose quarter)");
    AppendMenu(fillMenu, MF_STRING, ID_FILL_CIRC_CIRC, L"Circle w/ Circles (choose quarter)");
    AppendMenu(fillMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(fillMenu, MF_STRING, ID_FILL_SQ_HERMIT, L"Square w/ Hermit Curve [Vertical]");
    AppendMenu(fillMenu, MF_STRING, ID_FILL_RECT_BEZIER, L"Rectangle w/ Bezier Curve [Horizontal]");
    AppendMenu(fillMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(fillMenu, MF_STRING, ID_FILL_CONVEX, L"Convex / Non-Convex Fill");
    AppendMenu(fillMenu, MF_STRING, ID_FILL_FLOOD, L"Recursive / Non-Recursive Flood Fill");

    // Add quarter selection submenu to the fill menu
    HMENU quarterMenu = CreateMenu();
    AppendMenu(quarterMenu, MF_STRING, ID_QUARTER_1, L"1st Quarter (Top-Right)");
    AppendMenu(quarterMenu, MF_STRING, ID_QUARTER_2, L"2nd Quarter (Top-Left)");
    AppendMenu(quarterMenu, MF_STRING, ID_QUARTER_3, L"3rd Quarter (Bottom-Left)");
    AppendMenu(quarterMenu, MF_STRING, ID_QUARTER_4, L"4th Quarter (Bottom-Right)");
    AppendMenu(fillMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(fillMenu, MF_POPUP, (UINT_PTR)quarterMenu, L"Select Quarter for Circle Fill");

    // ── Clipping ──────────────────────────────────────────────────────────────
    HMENU clipMenu = CreateMenu();

    // Rectangle
    HMENU clipRectMenu = CreateMenu();
    AppendMenu(clipRectMenu, MF_STRING, ID_CLIP_RECT_POINT, L"Point Clipping");
    AppendMenu(clipRectMenu, MF_STRING, ID_CLIP_RECT_LINE, L"Line Clipping");
    AppendMenu(clipRectMenu, MF_STRING, ID_CLIP_RECT_POLY, L"Polygon Clipping");
    AppendMenu(clipMenu, MF_POPUP, (UINT_PTR)clipRectMenu, L"Rectangle Window");

    // Square
    HMENU clipSqMenu = CreateMenu();
    AppendMenu(clipSqMenu, MF_STRING, ID_CLIP_SQUARE_POINT, L"Point Clipping");
    AppendMenu(clipSqMenu, MF_STRING, ID_CLIP_SQUARE_LINE, L"Line Clipping");
    AppendMenu(clipMenu, MF_POPUP, (UINT_PTR)clipSqMenu, L"Square Window");

    // Circle (Bonus)
    AppendMenu(clipMenu, MF_SEPARATOR, 0, NULL);
    HMENU clipCircMenu = CreateMenu();
    AppendMenu(clipCircMenu, MF_STRING, ID_CLIP_CIRCLE_POINT, L"Point Clipping");
    AppendMenu(clipCircMenu, MF_STRING, ID_CLIP_CIRCLE_LINE, L"Line Clipping");
    AppendMenu(clipMenu, MF_POPUP, (UINT_PTR)clipCircMenu, L"Circle Window (Bonus)");

    // ── Smiley (Bonus) ────────────────────────────────────────────────────────
    HMENU smileMenu = CreateMenu();
    AppendMenu(smileMenu, MF_STRING, ID_SMILE_HAPPY, L"Happy Face :)");
    AppendMenu(smileMenu, MF_STRING, ID_SMILE_SAD, L"Sad Face :(");

    // ── Attach everything ─────────────────────────────────────────────────────
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)fileMenu, L"&File");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)prefMenu, L"&Preferences");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)pointMenu, L"&Points");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)lineMenu, L"&Lines");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)circleMenu, L"&Circles");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)ellipseMenu, L"&Ellipses");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)curveMenu, L"&Curves");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)fillMenu, L"&Filling");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)clipMenu, L"&Clipping");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)smileMenu, L"&Smiley");

    SetMenu(hwnd, mainMenu);
    DrawMenuBar(hwnd);
}