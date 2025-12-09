//
// Created by lidongyooo on 2025/12/8.
//

#ifndef ARM64DBIDEMO_ROUTER_H
#define ARM64DBIDEMO_ROUTER_H
#include "../types/types.h"
#include "Assembler.h"

class Router {
public:
    static uint64_t ending_addr;
    static void push_register(uint32_t *&writer);
    static void pop_register(uint32_t *&writer);
    static void save_ending_addr(uint32_t *&writer);
};

extern "C" {
    void router_push_register();
    void router_end_push_register();
    void router_pop_register();
    void router_end_pop_register();
    void router();
    uint64_t _router(uint64_t jump_addr, ROUTER_TYPE type);
};


#endif //ARM64DBIDEMO_ROUTER_H
