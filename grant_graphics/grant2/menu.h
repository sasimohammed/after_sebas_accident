#pragma once
// =============================================================================
//  menu.h  –  All command IDs for the 2D Drawing Package
// =============================================================================

// ── File ──────────────────────────────────────────────────────────────────────
#define ID_FILE_CLEAR           1001
#define ID_FILE_SAVE            1002
#define ID_FILE_LOAD            1003

// ── Preferences ───────────────────────────────────────────────────────────────
#define ID_PREF_BG              1010
#define ID_PREF_CURSOR          1011
#define ID_PREF_COLOR           1012

// ── Lines ─────────────────────────────────────────────────────────────────────
#define ID_LINE_DDA             1020
#define ID_LINE_MID             1021
#define ID_LINE_PARAM           1022

// ── Circles ───────────────────────────────────────────────────────────────────
#define ID_CIRC_DIRECT          1030
#define ID_CIRC_POLAR           1031
#define ID_CIRC_ITER_POLAR      1032
#define ID_CIRC_MID             1033
#define ID_CIRC_MOD_MID         1034

// ── Ellipses ──────────────────────────────────────────────────────────────────
#define ID_ELLIPSE_DIR          1040
#define ID_ELLIPSE_POL          1041
#define ID_ELLIPSE_MID          1042

// ── Curves ────────────────────────────────────────────────────────────────────
#define ID_CARDINAL_SPLINE      1050

// ── Filling ───────────────────────────────────────────────────────────────────
#define ID_FILL_CIRC_LINE       1060   // Fill circle with lines
#define ID_FILL_CIRC_CIRC       1061   // Fill circle with circles
#define ID_FILL_SQ_HERMIT       1062   // Fill square with Hermit curve (vertical)
#define ID_FILL_RECT_BEZIER     1063   // Fill rect with Bezier curve (horizontal)
#define ID_FILL_FLOOD           1064   // Recursive & non-recursive flood fill
#define ID_FILL_CONVEX          1065   // Convex & non-convex fill

// ── Clipping: Rectangle ───────────────────────────────────────────────────────
#define ID_CLIP_RECT_POINT      1070
#define ID_CLIP_RECT_LINE       1071
#define ID_CLIP_RECT_POLY       1072

// ── Clipping: Square ──────────────────────────────────────────────────────────
#define ID_CLIP_SQUARE_POINT    1075
#define ID_CLIP_SQUARE_LINE     1076

// ── Clipping: Circle (Bonus) ──────────────────────────────────────────────────
#define ID_CLIP_CIRCLE_POINT    1080
#define ID_CLIP_CIRCLE_LINE     1081

// ── Legacy aliases (keep old code compiling) ──────────────────────────────────
#define ID_CLIP_RECT            ID_CLIP_RECT_LINE
#define ID_CLIP_POINT           ID_CLIP_RECT_POINT

// ── Smiley Faces (Bonus) ──────────────────────────────────────────────────────
#define ID_SMILE_HAPPY          1090
#define ID_SMILE_SAD            1091

// Add these near your other ID definitions (like ID_LINE_DDA, etc.)
#define ID_QUARTER_1     4001
#define ID_QUARTER_2     4002
#define ID_QUARTER_3     4003
#define ID_QUARTER_4     4004

#define ID_DRAW_POINT    109  // or whatever number fits your sequence
// Add this near your other drawing mode IDs (around line 20-30)
#define ID_POINT_CIRCLE         1100   // Draw point as circle
#define ID_POINT_SQUARE         1101   // Draw point as square  
#define ID_POINT_CROSSHAIR      1102   // Draw point as crosshair
#define ID_POINT_DIRECT         1103   // Draw point directly with SetPixel

// menu.h – add these lines
#define ID_POINT_SIZE_SMALL     1110
#define ID_POINT_SIZE_MEDIUM    1111
#define ID_POINT_SIZE_LARGE     1112