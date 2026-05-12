#include "Globals.h"
#include <vector>
using namespace std;

HDC g_hdc = NULL;
HWND g_hwnd = NULL;

int g_menu = 0;
int g_sub = 0;
int g_quarter = 1;

int g_clickCount = 0;
int g_pts[20][2];

vector<POINT> g_poly;
bool g_shapeClosed = false;

vector<POINT> g_floodPoly;
vector<POINT> g_clipPoly;
int g_clipObjectType = 1;
bool g_clipModeEnabled = false;

// Clipping type flags
bool g_isCircleClip = false;
bool g_isRectClip = false;
bool g_isSquareClip = false;

// Circle clip data
int g_clipCenterX = 0;
int g_clipCenterY = 0;
int g_clipRadius = 0;

// Square clip data
int g_clipSize = 0;

int g_clipLeft = 100;
int g_clipTop = 100;
int g_clipRight = 300;
int g_clipBottom = 300;