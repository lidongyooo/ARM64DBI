//
// Created by lidongyooo on 2025/12/8.
//

#include "Router.h"

size_t push_register_size = ((uint64_t) router_end_push_register - (uint64_t) router_push_register) / A64_INS_WIDTH;
auto push_register_ptr = (uint32_t *) router_push_register;
size_t pop_register_size = ((uint64_t) router_end_pop_register - (uint64_t) router_pop_register) / A64_INS_WIDTH;
auto pop_register_ptr = (uint32_t *) router_pop_register;

void _save_ending_addr(uint64_t addr) {
    Router::ending_addr = addr;
}

void Router::push_register(uint32_t *&writer) {
    for (int i = 0; i < push_register_size; ++i) {
        writer[i] = push_register_ptr[i];
    }
    writer += push_register_size;
}

void Router::pop_register(uint32_t *&writer) {
    for (int i = 0; i < pop_register_size; ++i) {
        writer[i] = pop_register_ptr[i];
    }
    writer += pop_register_size;
}

uint64_t Router::ending_addr = 0;
void Router::save_ending_addr(uint32_t *&writer) {
    push_register(writer);

    uint32_t instruction;
    Assembler::mov_x_to_x(instruction, REG_X0, REG_LR);
    *writer = instruction;
    writer++;

    Assembler::blr_x16_jump(writer, (uint64_t) _save_ending_addr);
    pop_register(writer);
}

// 检查是否已插桩，如果已插桩直接索引到缓存的基本块返回。否则扫描后再返回
uint64_t _router(uint64_t jump_addr, ROUTER_TYPE type) {
    auto block_meta = Memory::get_cache_block_meta(jump_addr);
    if (block_meta == nullptr) {
        block_meta = Memory::get_or_new_block_meta();
        Translator::scan(jump_addr, block_meta, type);
    }

    return (uint64_t )block_meta->block_start;
}