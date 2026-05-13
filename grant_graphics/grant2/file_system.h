#pragma once
#include "core_data.h"
#include <string>
#include <windows.h>

class FileSystem {
public:
    static void save(const CoreData& core);
    static void load(CoreData& core);
    static void clearScreen(HWND hwnd, CoreData& core);
};