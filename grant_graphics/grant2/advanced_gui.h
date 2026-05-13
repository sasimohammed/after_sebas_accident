#pragma once
// =============================================================================
//  advanced_gui.h  –  SCROLLABLE SIDEBAR with ALL options visible
// =============================================================================
#pragma comment(lib, "dwmapi.lib")

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include "menu.h"

namespace GUI
{
    // ── Layout constants ──────────────────────────────────────────────────────
    constexpr int SIDEBAR_W = 230;
    constexpr int TOOLBAR_H = 50;
    constexpr int STATUSBAR_H = 28;
    constexpr int BTN_H = 30;
    constexpr int BTN_MARGIN = 8;
    constexpr int SECTION_H = 20;
    constexpr int SCROLL_W = 12;   // slim scrollbar strip

    // ── Color palette ─────────────────────────────────────────────────────────
    constexpr COLORREF CLR_BG = RGB(10, 10, 15);
    constexpr COLORREF CLR_SIDEBAR = RGB(18, 18, 26);
    constexpr COLORREF CLR_PANEL = RGB(24, 24, 36);
    constexpr COLORREF CLR_TOOLBAR = RGB(14, 14, 20);
    constexpr COLORREF CLR_STATUSBAR = RGB(10, 10, 16);
    constexpr COLORREF CLR_BORDER = RGB(45, 45, 65);
    constexpr COLORREF CLR_BORDER_GLOW = RGB(70, 70, 110);
    constexpr COLORREF CLR_ACCENT = RGB(0, 220, 255);   // cyan
    constexpr COLORREF CLR_ACCENT2 = RGB(200, 80, 255);   // violet
    constexpr COLORREF CLR_ACCENT3 = RGB(0, 230, 130);   // green
    constexpr COLORREF CLR_ACCENT4 = RGB(255, 160, 40);   // orange
    constexpr COLORREF CLR_TEXT = RGB(235, 235, 245);
    constexpr COLORREF CLR_TEXT_DIM = RGB(130, 130, 165);
    constexpr COLORREF CLR_TEXT_GLOW = RGB(200, 220, 255);
    constexpr COLORREF CLR_BTN_HOVER = RGB(32, 32, 52);
    constexpr COLORREF CLR_BTN_ACTIVE = RGB(40, 35, 105);
    constexpr COLORREF CLR_BTN_NORMAL = RGB(24, 24, 38);
    constexpr COLORREF CLR_SUCCESS = RGB(0, 220, 110);
    constexpr COLORREF CLR_WARNING = RGB(255, 200, 50);
    constexpr COLORREF CLR_DANGER = RGB(255, 65, 65);
    constexpr COLORREF CLR_INFO = RGB(0, 190, 255);
    constexpr COLORREF CLR_PURPLE = RGB(170, 90, 255);
    constexpr COLORREF CLR_BLUE = RGB(50, 130, 255);
    constexpr COLORREF CLR_GREEN = RGB(30, 200, 80);
    constexpr COLORREF CLR_SCROLL_TRACK = RGB(22, 22, 32);
    constexpr COLORREF CLR_SCROLL_THUMB = RGB(60, 60, 90);

    // ── Data types ────────────────────────────────────────────────────────────
    struct BtnDef { int id; std::wstring label, icon; COLORREF accent; };
    struct Section { std::wstring title; COLORREF titleColor; std::vector<BtnDef> btns; };
    struct BtnState { RECT rc{}; int id = -1; bool hovered = false, active = false; };

    // ── ALL sections with ALL required options ────────────────────────────────
    static const std::vector<Section> g_sections =
    {
        { L"  FILE", CLR_DANGER, {
            { ID_FILE_CLEAR, L"Clear Canvas",    L"\U0001F5D1", CLR_DANGER  },
            { ID_FILE_SAVE,  L"Save Project",    L"\U0001F4BE", CLR_SUCCESS },
            { ID_FILE_LOAD,  L"Load Project",    L"\U0001F4C2", CLR_ACCENT3 },
        }},
        { L"  PREFERENCES", CLR_ACCENT4, {
            { ID_PREF_BG,     L"White Background", L"\u25A1", CLR_ACCENT4 },
            { ID_PREF_CURSOR, L"Change Cursor",    L"\u25B7", CLR_ACCENT4 },
            { ID_PREF_COLOR,  L"Color = Red",      L"\u25CF", CLR_DANGER  },
        }},

        
        { L"  LINES", CLR_ACCENT, {
            { ID_LINE_DDA,   L"DDA Algorithm",  L"/", CLR_ACCENT },
            { ID_LINE_MID,   L"Midpoint Line",  L"/", CLR_ACCENT },
            { ID_LINE_PARAM, L"Parametric Line",L"/", CLR_ACCENT },
        }},
        { L"  CIRCLES", CLR_ACCENT2, {
            { ID_CIRC_DIRECT,     L"Direct Circle",        L"\u25CB", CLR_ACCENT2 },
            { ID_CIRC_POLAR,      L"Polar Circle",         L"\u25CB", CLR_ACCENT2 },
            { ID_CIRC_ITER_POLAR, L"Iterative Polar",      L"\u25CB", CLR_ACCENT2 },
            { ID_CIRC_MID,        L"Midpoint Circle",      L"\u25CB", CLR_ACCENT2 },
            { ID_CIRC_MOD_MID,    L"Modified Midpoint",    L"\u25CB", CLR_ACCENT2 },
        }},
        { L"  ELLIPSES", CLR_ACCENT3, {
            { ID_ELLIPSE_DIR, L"Direct Ellipse",   L"\u25CF", CLR_ACCENT3 },
            { ID_ELLIPSE_POL, L"Polar Ellipse",    L"\u25CF", CLR_ACCENT3 },
            { ID_ELLIPSE_MID, L"Midpoint Ellipse", L"\u25CF", CLR_ACCENT3 },
        }},
        { L"  CURVES", CLR_ACCENT4, {
            { ID_CARDINAL_SPLINE, L"Cardinal Spline", L"~", CLR_ACCENT4 },
        }},
        { L"  FILLING", CLR_WARNING, {
            { ID_FILL_CIRC_LINE,   L"Circle w/ Lines",   L"\u25D0", CLR_WARNING },
            { ID_FILL_CIRC_CIRC,   L"Circle w/ Circles", L"\u25D1", CLR_WARNING },
            { ID_FILL_SQ_HERMIT,   L"Square w/ Hermit",  L"\u25A3", CLR_WARNING },
            { ID_FILL_RECT_BEZIER, L"Rect w/ Bezier",    L"\u25A4", CLR_WARNING },
            { ID_FILL_FLOOD,       L"Flood Fill",        L"\u25A0", CLR_WARNING },
            { ID_FILL_CONVEX,      L"Convex/Non-Convex", L"\u25B2", CLR_WARNING },
        }},
        { L"  CLIP: RECTANGLE", CLR_BLUE, {
            { ID_CLIP_RECT_POINT, L"Rect - Point",   L"\u25AA", CLR_BLUE },
            { ID_CLIP_RECT_LINE,  L"Rect - Line",    L"\u25AC", CLR_BLUE },
            { ID_CLIP_RECT_POLY,  L"Rect - Polygon", L"\u25AD", CLR_BLUE },
        }},
        { L"  CLIP: SQUARE", CLR_GREEN, {
            { ID_CLIP_SQUARE_POINT, L"Square - Point", L"\u25AA", CLR_GREEN },
            { ID_CLIP_SQUARE_LINE,  L"Square - Line",  L"\u25AC", CLR_GREEN },
        }},
        { L"  CLIP: CIRCLE", CLR_PURPLE, {
            { ID_CLIP_CIRCLE_POINT, L"Circle - Point", L"\u25CF", CLR_PURPLE },
            { ID_CLIP_CIRCLE_LINE,  L"Circle - Line",  L"\u25CB", CLR_PURPLE },
        }},
        { L"  SMILEY FACES", CLR_WARNING, {
            { ID_SMILE_HAPPY, L"Happy Face", L":)", CLR_SUCCESS },
            { ID_SMILE_SAD,   L"Sad Face",   L":(", CLR_DANGER  },
        }},
    };

    // =========================================================================
    class AppGUI
    {
    public:
        HWND hwnd = nullptr;
        int  activeId = -1;

        std::wstring statusLeft = L"Ready";
        std::wstring statusCenter = L"";
        std::wstring statusRight = L"No clipping";

        // Fonts
        HFONT hFontUI = nullptr;
        HFONT hFontIcon = nullptr;
        HFONT hFontSmall = nullptr;
        HFONT hFontSec = nullptr;
        HFONT hFontMono = nullptr;
        HFONT hFontTitle = nullptr;

        // Button hit-test state
        std::vector<BtnState> sidebarBtns;

        // Scrollbar state
        int  scrollOffset = 0;   // pixels scrolled from top of content
        int  contentHeight = 0;   // total pixel height of sidebar content
        bool thumbDragging = false;
        int  dragStartY = 0;
        int  dragStartOff = 0;

        // Chrome double-buffer
        HDC     chromeDC = nullptr;
        HBITMAP chromeBM = nullptr;
        bool    chromeDirty = true;
        int     cachedW = 0, cachedH = 0;

        // ── Public API ────────────────────────────────────────────────────────
        void Setup(HWND h)
        {
            hwnd = h;
            MakeFonts();
            chromeDirty = true;

            BOOL dark = TRUE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
        }

        void Destroy()
        {
            auto del = [](HFONT& f) { if (f) { DeleteObject(f); f = nullptr; } };
            del(hFontUI); del(hFontIcon); del(hFontSmall);
            del(hFontSec); del(hFontMono); del(hFontTitle);
            FreeChromeBuf();
        }

        void SetStatus(const std::wstring& l,
            const std::wstring& c = L"",
            const std::wstring& r = L"")
        {
            if (statusLeft == l && statusCenter == c && statusRight == r) return;
            statusLeft = l; statusCenter = c; statusRight = r;
            chromeDirty = true;
            InvalidateChromeOnly();
        }

        void SetActiveMode(int id)
        {
            activeId = id;
            for (auto& b : sidebarBtns) b.active = (b.id == id);
            chromeDirty = true;
            InvalidateChromeOnly();
        }

        RECT GetCanvasRect(HWND h) const
        {
            RECT rc; GetClientRect(h, &rc);
            rc.left = SIDEBAR_W;
            rc.top = TOOLBAR_H;
            rc.bottom -= STATUSBAR_H;
            return rc;
        }

        void OnSize(int /*w*/, int /*h*/)
        {
            FreeChromeBuf();
            chromeDirty = true;
        }

        // Returns true if click consumed by chrome
        bool OnLButtonDown(int x, int y)
        {
            // Scrollbar thumb?
            if (x >= SIDEBAR_W - SCROLL_W && x < SIDEBAR_W)
            {
                int sbH = SidebarContentH();
                RECT thumb = ThumbRect(sbH);
                POINT pt = { x,y };
                if (PtInRect(&thumb, pt))
                {
                    thumbDragging = true;
                    dragStartY = y;
                    dragStartOff = scrollOffset;
                    SetCapture(hwnd);
                }
                return true;
            }

            // Sidebar button?
            if (x >= 0 && x < SIDEBAR_W)
            {
                POINT pt = { x,y };
                for (auto& b : sidebarBtns)
                    if (PtInRect(&b.rc, pt))
                    {
                        SetActiveMode(b.id);
                        SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(b.id, 0), 0);
                        return true;
                    }
                return true; // consumed even if no button hit (sidebar area)
            }

            // Toolbar button?
            if (y < TOOLBAR_H)
            {
                for (auto& b : tbBtns)
                    if (b.id != -1)
                    {
                        POINT pt = { x,y };
                        if (PtInRect(&b.rc, pt))
                        {
                            SetActiveMode(b.id);
                            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(b.id, 0), 0);
                            return true;
                        }
                    }
            }
            return false;
        }

        bool OnLButtonUp(int x, int y)
        {
            if (thumbDragging) { thumbDragging = false; ReleaseCapture(); return true; }
            return false;
        }

        bool OnMouseMove(int x, int y)
        {
            if (thumbDragging)
            {
                int sbH = SidebarContentH();
                int track = sbH - ThumbH(sbH);
                int maxOff = max(0, contentHeight - sbH);
                if (track > 0)
                {
                    int dy = y - dragStartY;
                    scrollOffset = dragStartOff + (int)((float)dy / track * maxOff);
                    scrollOffset = max(0, min(scrollOffset, maxOff));
                }
                chromeDirty = true;
                InvalidateChromeOnly();
                return true;
            }

            bool changed = false;
            POINT pt = { x,y };

            for (auto& b : sidebarBtns)
            {
                bool h = PtInRect(&b.rc, pt) != 0;
                if (h != b.hovered) { b.hovered = h; changed = true; }
            }
            for (auto& b : tbBtns)
            {
                bool h = (b.id != -1) && PtInRect(&b.rc, pt) != 0;
                if (h != b.hovered) { b.hovered = h; changed = true; }
            }
            if (changed) { chromeDirty = true; InvalidateChromeOnly(); }
            return false;
        }

        // Scroll with mouse wheel over sidebar
        bool OnMouseWheel(int delta, int x, int /*y*/)
        {
            if (x >= SIDEBAR_W) return false;
            int sbH = SidebarContentH();
            int maxOff = max(0, contentHeight - sbH);
            scrollOffset -= delta / 2;
            scrollOffset = max(0, min(scrollOffset, maxOff));
            chromeDirty = true;
            InvalidateChromeOnly();
            return true;
        }

        // Draw ALL chrome (toolbar + sidebar + statusbar) into hdc
        void Draw(HDC hdc)
        {
            RECT cr; GetClientRect(hwnd, &cr);
            int W = cr.right, H = cr.bottom;

            if (!chromeDC || W != cachedW || H != cachedH)
            {
                FreeChromeBuf();
                chromeDC = CreateCompatibleDC(hdc);
                chromeBM = CreateCompatibleBitmap(hdc, W, H);
                SelectObject(chromeDC, chromeBM);
                cachedW = W; cachedH = H;
                chromeDirty = true;
            }

            if (chromeDirty)
            {
                PaintToolbar(chromeDC, cr);
                PaintSidebar(chromeDC, cr);
                PaintStatusBar(chromeDC, cr);
                chromeDirty = false;
            }

            // Blit toolbar
            BitBlt(hdc, 0, 0, W, TOOLBAR_H, chromeDC, 0, 0, SRCCOPY);
            // Blit sidebar (including scrollbar strip)
            BitBlt(hdc, 0, TOOLBAR_H, SIDEBAR_W, H - TOOLBAR_H - STATUSBAR_H,
                chromeDC, 0, TOOLBAR_H, SRCCOPY);
            // Blit status bar
            int sbTop = H - STATUSBAR_H;
            BitBlt(hdc, 0, sbTop, W, STATUSBAR_H, chromeDC, 0, sbTop, SRCCOPY);
        }

    private:
        // Toolbar button state (built once per layout)
        struct TbBtn {
            int id; RECT rc; std::wstring icon, label; COLORREF col;
            bool isSep = false, hovered = false, active = false;
        };
        std::vector<TbBtn> tbBtns;

        // ── Chrome buffer helpers ──────────────────────────────────────────────
        void FreeChromeBuf()
        {
            if (chromeBM) { DeleteObject(chromeBM); chromeBM = nullptr; }
            if (chromeDC) { DeleteDC(chromeDC);     chromeDC = nullptr; }
        }

        void InvalidateChromeOnly()
        {
            if (!hwnd) return;
            RECT cr; GetClientRect(hwnd, &cr);
            RECT tb = { 0,0,cr.right,TOOLBAR_H };
            RECT sb = { 0,TOOLBAR_H,SIDEBAR_W,cr.bottom - STATUSBAR_H };
            RECT stb = { 0,cr.bottom - STATUSBAR_H,cr.right,cr.bottom };
            InvalidateRect(hwnd, &tb, FALSE);
            InvalidateRect(hwnd, &sb, FALSE);
            InvalidateRect(hwnd, &stb, FALSE);
        }

        // ── Font creation ─────────────────────────────────────────────────────
        void MakeFonts()
        {
            auto MF = [](int h, bool bold, const wchar_t* face)->HFONT {
                return CreateFont(h, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL,
                    FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, face);
                };
            hFontUI = MF(12, false, L"Segoe UI");
            hFontIcon = MF(14, false, L"Segoe UI Emoji");
            hFontSmall = MF(10, false, L"Segoe UI");
            hFontSec = MF(10, true, L"Segoe UI");
            hFontMono = MF(10, false, L"Consolas");
            hFontTitle = MF(14, true, L"Segoe UI");
        }

        // ── Drawing primitives ────────────────────────────────────────────────
        void FillRC(HDC hdc, RECT rc, COLORREF c)
        {
            HBRUSH b = CreateSolidBrush(c);
            FillRect(hdc, &rc, b);
            DeleteObject(b);
        }

        void Line_(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c, int w = 1)
        {
            HPEN p = CreatePen(PS_SOLID, w, c);
            HPEN o = (HPEN)SelectObject(hdc, p);
            MoveToEx(hdc, x1, y1, nullptr); LineTo(hdc, x2, y2);
            SelectObject(hdc, o); DeleteObject(p);
        }

        void RndRect(HDC hdc, RECT rc, int r, COLORREF fill, COLORREF border)
        {
            HBRUSH b = CreateSolidBrush(fill);
            HPEN   p = (border == fill) ? (HPEN)GetStockObject(NULL_PEN)
                : CreatePen(PS_SOLID, 1, border);
            HBRUSH ob = (HBRUSH)SelectObject(hdc, b);
            HPEN   op = (HPEN)SelectObject(hdc, p);
            RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, r, r);
            SelectObject(hdc, ob); SelectObject(hdc, op);
            DeleteObject(b);
            if (border != fill) DeleteObject(p);
        }

        void TxtOut(HDC hdc, RECT rc, const std::wstring& s, COLORREF c, HFONT f,
            UINT fmt = DT_LEFT | DT_VCENTER | DT_SINGLELINE)
        {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, c);
            SelectObject(hdc, f);
            DrawTextW(hdc, s.c_str(), -1, &rc, fmt);
        }

        // ── Scrollbar geometry ────────────────────────────────────────────────
        int SidebarContentH() const
        {
            RECT cr; GetClientRect(hwnd, &cr);
            return cr.bottom - TOOLBAR_H - STATUSBAR_H;
        }

        int ThumbH(int sbH) const
        {
            if (contentHeight <= 0) return sbH;
            int th = (int)((float)sbH / contentHeight * sbH);
            return max(30, min(th, sbH));
        }

        RECT ThumbRect(int sbH) const
        {
            int maxOff = max(1, contentHeight - sbH);
            int track = sbH - ThumbH(sbH);
            int thumbY = TOOLBAR_H + (int)((float)scrollOffset / maxOff * track);
            int th = ThumbH(sbH);
            return { SIDEBAR_W - SCROLL_W, thumbY, SIDEBAR_W, thumbY + th };
        }

        // ── Toolbar ───────────────────────────────────────────────────────────
        void BuildToolbar()
        {
            tbBtns.clear();
            struct Def { int id; const wchar_t* icon; const wchar_t* lbl; COLORREF col; bool sep; };
            static const Def defs[] = {
                {ID_FILE_CLEAR,       L"\U0001F5D1", L"Clear",   CLR_DANGER,  false},
                {ID_FILE_SAVE,        L"\U0001F4BE", L"Save",    CLR_SUCCESS, false},
                {ID_FILE_LOAD,        L"\U0001F4C2", L"Load",    CLR_ACCENT3, false},
                {-1,L"|",L"",CLR_BORDER,true},
                {ID_LINE_DDA,         L"/",          L"Line",    CLR_ACCENT,  false},
                {ID_CIRC_MID,         L"\u25CB",     L"Circle",  CLR_ACCENT2, false},
                {ID_ELLIPSE_MID,      L"\u25CF",     L"Ellipse", CLR_ACCENT3, false},
                {ID_CARDINAL_SPLINE,  L"~",          L"Curve",   CLR_ACCENT4, false},
                {-1,L"|",L"",CLR_BORDER,true},
                {ID_FILL_FLOOD,       L"\u25A0",     L"Flood",   CLR_WARNING, false},
                {ID_FILL_CONVEX,      L"\u25B2",     L"Convex",  CLR_WARNING, false},
                {-1,L"|",L"",CLR_BORDER,true},
                {ID_CLIP_RECT_POINT,  L"\u25AA",     L"R-Pt",    CLR_BLUE,    false},
                {ID_CLIP_RECT_LINE,   L"\u25AC",     L"R-Ln",    CLR_BLUE,    false},
                {ID_CLIP_RECT_POLY,   L"\u25AD",     L"R-Poly",  CLR_BLUE,    false},
                {-1,L"|",L"",CLR_BORDER,true},
                {ID_CLIP_SQUARE_POINT,L"\u25AA",     L"S-Pt",    CLR_GREEN,   false},
                {ID_CLIP_SQUARE_LINE, L"\u25AC",     L"S-Ln",    CLR_GREEN,   false},
                {-1,L"|",L"",CLR_BORDER,true},
                {ID_CLIP_CIRCLE_POINT,L"\u25CF",     L"C-Pt",    CLR_PURPLE,  false},
                {ID_CLIP_CIRCLE_LINE, L"\u25CB",     L"C-Ln",    CLR_PURPLE,  false},
                {-1,L"|",L"",CLR_BORDER,true},
                {ID_SMILE_HAPPY,      L":)",         L"Happy",   CLR_SUCCESS, false},
                {ID_SMILE_SAD,        L":(",         L"Sad",     CLR_DANGER,  false},
            };

            const int BTN_W = 46, SEP_W = 16, START_X = SIDEBAR_W + 10;
            int x = START_X;
            for (auto& d : defs)
            {
                TbBtn b;
                b.id = d.id; b.icon = d.icon; b.label = d.lbl; b.col = d.col; b.isSep = d.sep;
                b.active = (d.id == activeId && d.id != -1);
                if (d.sep) {
                    b.rc = { x,8,x + SEP_W,TOOLBAR_H - 8 }; x += SEP_W;
                }
                else {
                    b.rc = { x,5,x + BTN_W,TOOLBAR_H - 5 }; x += BTN_W + 3;
                }
                tbBtns.push_back(b);
            }
        }

        void PaintToolbar(HDC hdc, RECT& cr)
        {
            BuildToolbar();

            RECT tb = { 0,0,cr.right,TOOLBAR_H };
            FillRC(hdc, tb, CLR_TOOLBAR);
            FillRC(hdc, { 0,0,cr.right,2 }, CLR_ACCENT);
            Line_(hdc, 0, TOOLBAR_H - 1, cr.right, TOOLBAR_H - 1, CLR_BORDER_GLOW, 1);

            // Logo area
            FillRC(hdc, { 0,0,SIDEBAR_W,TOOLBAR_H }, CLR_PANEL);
            Line_(hdc, SIDEBAR_W - 1, 0, SIDEBAR_W - 1, TOOLBAR_H, CLR_BORDER_GLOW);
            TxtOut(hdc, { 14,0,SIDEBAR_W - 4,TOOLBAR_H },
                L"\u2B23 DRAW STUDIO", CLR_ACCENT, hFontTitle,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            for (auto& b : tbBtns)
            {
                if (b.isSep) {
                    int cx = (b.rc.left + b.rc.right) / 2;
                    Line_(hdc, cx, b.rc.top + 4, cx, b.rc.bottom - 4, CLR_BORDER);
                    continue;
                }
                COLORREF bg = b.active ? CLR_BTN_ACTIVE
                    : b.hovered ? CLR_BTN_HOVER : CLR_TOOLBAR;
                RndRect(hdc, b.rc, 6, bg, b.active ? b.col : CLR_BORDER);

                // Bottom accent line when active
                if (b.active)
                    FillRC(hdc, { b.rc.left + 3,b.rc.bottom - 3,b.rc.right - 3,b.rc.bottom }, b.col);

                RECT iconRc = { b.rc.left,b.rc.top,b.rc.right,b.rc.bottom - 14 };
                RECT lblRc = { b.rc.left,b.rc.bottom - 14,b.rc.right,b.rc.bottom };

                COLORREF ic = b.active ? b.col : (b.hovered ? CLR_TEXT : CLR_TEXT_DIM);
                TxtOut(hdc, iconRc, b.icon, ic, hFontIcon, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                TxtOut(hdc, lblRc, b.label, b.active ? b.col : CLR_TEXT_DIM,
                    hFontSmall, DT_CENTER | DT_TOP | DT_SINGLELINE);
            }
        }

        // ── Sidebar ───────────────────────────────────────────────────────────
        void PaintSidebar(HDC hdc, RECT& cr)
        {
            int sbTop = TOOLBAR_H;
            int sbBot = cr.bottom - STATUSBAR_H;
            int sbH = sbBot - sbTop;
            int contentW = SIDEBAR_W - SCROLL_W;

            // Background
            FillRC(hdc, { 0,sbTop,SIDEBAR_W,sbBot }, CLR_SIDEBAR);
            Line_(hdc, SIDEBAR_W - 1, sbTop, SIDEBAR_W - 1, sbBot, CLR_BORDER_GLOW);

            // ── Scrollbar track ──────────────────────────────────────────────
            FillRC(hdc, { SIDEBAR_W - SCROLL_W,sbTop,SIDEBAR_W,sbBot }, CLR_SCROLL_TRACK);
            // Thumb
            RECT thumb = ThumbRect(sbH);
            RndRect(hdc, { thumb.left + 2,thumb.top + 2,thumb.right - 2,thumb.bottom - 2 },
                4, CLR_SCROLL_THUMB, CLR_SCROLL_THUMB);

            // ── Clip to sidebar content area (excluding scrollbar) ────────────
            HRGN clipRgn = CreateRectRgn(0, sbTop, contentW, sbBot);
            SelectClipRgn(hdc, clipRgn);

            // ── Render all buttons with scroll offset applied ──────────────────
            sidebarBtns.clear();
            int y = sbTop - scrollOffset + 4;   // virtual Y position

            contentHeight = 4; // accumulate total content height

            for (const auto& sec : g_sections)
            {
                if (sec.btns.empty()) continue;

                // Section header
                int headerY = y;
                if (headerY + SECTION_H > sbTop && headerY < sbBot)
                {
                    RECT hdrBg = { 0,headerY,contentW,headerY + SECTION_H };
                    FillRC(hdc, hdrBg, CLR_PANEL);
                    // Accent left bar
                    FillRC(hdc, { 0,headerY,3,headerY + SECTION_H }, sec.titleColor);
                    TxtOut(hdc, { 8,headerY,contentW,headerY + SECTION_H },
                        sec.title, sec.titleColor, hFontSec,
                        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                }
                y += SECTION_H + 2;
                contentHeight += SECTION_H + 2;

                for (const auto& def : sec.btns)
                {
                    // Store button rect in SCREEN coordinates for hit testing
                    RECT screenRc = { BTN_MARGIN, y, contentW - BTN_MARGIN, y + BTN_H };

                    BtnState bs;
                    bs.id = def.id;
                    bs.active = (def.id == activeId);
                    bs.rc = screenRc;   // screen coords for hit-test
                    // Only register if visible
                    if (screenRc.bottom > sbTop && screenRc.top < sbBot)
                    {
                        POINT mpt; GetCursorPos(&mpt);
                        ScreenToClient(hwnd, &mpt);
                        bs.hovered = PtInRect(&screenRc, mpt) != 0;
                        sidebarBtns.push_back(bs);

                        // Draw
                        COLORREF bg = bs.active ? CLR_BTN_ACTIVE
                            : bs.hovered ? CLR_BTN_HOVER : CLR_BTN_NORMAL;
                        RndRect(hdc, screenRc, 6, bg,
                            bs.active ? def.accent : CLR_BORDER);

                        // Left stripe
                        COLORREF stripe = bs.active ? def.accent
                            : bs.hovered ? RGB(GetRValue(def.accent) / 3,
                                GetGValue(def.accent) / 3,
                                GetBValue(def.accent) / 3)
                            : CLR_BORDER;
                        FillRC(hdc, { BTN_MARGIN,screenRc.top + 5,
                                    BTN_MARGIN + 3,screenRc.bottom - 5 }, stripe);

                        // Icon
                        COLORREF ic = bs.active ? def.accent
                            : bs.hovered ? CLR_TEXT : CLR_TEXT_DIM;
                        TxtOut(hdc, { BTN_MARGIN + 7,screenRc.top,
                                    BTN_MARGIN + 24,screenRc.bottom },
                            def.icon, ic, hFontIcon,
                            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

                        // Label
                        COLORREF lc = (bs.active || bs.hovered) ? CLR_TEXT : CLR_TEXT_DIM;
                        TxtOut(hdc, { BTN_MARGIN + 28,screenRc.top,
                                    screenRc.right - 18,screenRc.bottom },
                            def.label, lc, hFontUI,
                            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

                        // Active arrow
                        if (bs.active)
                            TxtOut(hdc, { screenRc.right - 18,screenRc.top,
                                        screenRc.right - 4,screenRc.bottom },
                                L"\u25B6", def.accent, hFontSmall,
                                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    }
                    y += BTN_H + 3;
                    contentHeight += BTN_H + 3;
                }
                y += 6;
                contentHeight += 6;
            }

            // Clamp scroll if content shrank
            int maxOff = max(0, contentHeight - sbH);
            if (scrollOffset > maxOff) scrollOffset = maxOff;

            SelectClipRgn(hdc, NULL);
            DeleteObject(clipRgn);

            // Scroll hint arrows
            if (scrollOffset > 0)
            {
                TxtOut(hdc, { SIDEBAR_W - SCROLL_W,sbTop,SIDEBAR_W,sbTop + 16 },
                    L"\u25B2", CLR_TEXT_DIM, hFontSmall,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            if (scrollOffset < maxOff)
            {
                TxtOut(hdc, { SIDEBAR_W - SCROLL_W,sbBot - 16,SIDEBAR_W,sbBot },
                    L"\u25BC", CLR_TEXT_DIM, hFontSmall,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
        }

        // ── Status bar ────────────────────────────────────────────────────────
        void PaintStatusBar(HDC hdc, RECT& cr)
        {
            int top = cr.bottom - STATUSBAR_H;
            FillRC(hdc, { 0,top,cr.right,cr.bottom }, CLR_STATUSBAR);
            Line_(hdc, 0, top, cr.right, top, CLR_BORDER);
            FillRC(hdc, { 0,cr.bottom - 2,cr.right,cr.bottom }, CLR_ACCENT);

            TxtOut(hdc, { SIDEBAR_W + 10,top,cr.right / 2,cr.bottom },
                L"\u25CF  " + statusLeft, CLR_TEXT, hFontSmall,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            if (!statusCenter.empty())
                TxtOut(hdc, { SIDEBAR_W,top,cr.right,cr.bottom },
                    L"\u21BA  " + statusCenter, CLR_TEXT_DIM, hFontSmall,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            COLORREF rc_col = CLR_SUCCESS;
            if (statusRight.find(L"CLIP") != std::wstring::npos) rc_col = CLR_INFO;
            else if (statusRight.find(L"No") != std::wstring::npos) rc_col = CLR_TEXT_DIM;

            TxtOut(hdc, { SIDEBAR_W,top,cr.right - 12,cr.bottom },
                statusRight, rc_col, hFontMono,
                DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
    };

} // namespace GUI

// ── Singleton ─────────────────────────────────────────────────────────────────
static GUI::AppGUI g_gui;
inline GUI::AppGUI& GetGUI() { return g_gui; }

void CreateAppMenu(HWND hwnd);