//
// Created by lidongyooo on 2025/12/7.
//

#ifndef ARM64DBIDEMO_TYPES_H
#define ARM64DBIDEMO_TYPES_H

#include <iostream>
#include "capstone/capstone.h"
#include "sys/mman.h"
#include <unistd.h>
#include "android/log.h"

enum ROUTER_TYPE {
    ENDING_ROUTER_TYPE,
    B_ROUTER_TYPE,
    BL_ROUTER_TYPE,
    BR_ROUTER_TYPE,
    BLR_ROUTER_TYPE,
    RET_ROUTER_TYPE
};


extern "C" {
}

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "lidongyooodbi", __VA_ARGS__)
#define PAGE_SIZE 4096
#define ALIGN_PAGE_UP(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define A64_INS_WIDTH 4

#endif //ARM64DBIDEMO_TYPES_H