//
// Created by lidongyooo on 2025/12/7.
//

#ifndef ARM64DBIDEMO_UTILS_H
#define ARM64DBIDEMO_UTILS_H
#include "../types/types.h"
#include "Memory.h"

class Utils {
public:
    static void show_code(void* code_addr, int number = 999);
    static void show_block_code(BlockMeta* block_meta);
};

#endif //ARM64DBIDEMO_UTILS_H
