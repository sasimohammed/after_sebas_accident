#pragma once
#include "core_data.h"
#include <string>

class FileSystem {
public:
    static void save(const CoreData& core, string filename);
    static void load(CoreData& core, string filename);
};