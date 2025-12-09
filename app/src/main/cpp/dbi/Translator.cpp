//
// Created by lidongyooo on 2025/12/7.
//

#include "Translator.h"

csh translator_handle;
auto isok = cs_open(CS_ARCH_AARCH64, CS_MODE_ARM, &translator_handle);
auto translator_insn = (cs_insn *)malloc(sizeof(cs_insn));

void block_ending(ROUTER_TYPE type, BlockMeta* block_meta, uint8_t* dbi_code, uint64_t target_addr, uint64_t pc) {
    block_meta->block_start = (void*)block_meta->code;
    block_meta->block_size = dbi_code - (uint8_t*)block_meta->code;
    block_meta->code_start = (void*)target_addr;
    block_meta->code_size = pc - target_addr;

    // 缓存基本块
    if (type != ENDING_ROUTER_TYPE) {
        Memory::set_cache_block_meta(target_addr, block_meta->index);
    }
}

void Translator::scan(uint64_t target_addr, BlockMeta* block_meta, ROUTER_TYPE type) {
    auto dbi_code = (uint32_t *)block_meta->code;
    switch (type) {
        case ENDING_ROUTER_TYPE: {
            Router::save_ending_addr(dbi_code);
            break;
        }
        default: {
            Router::pop_register(dbi_code);
            break;
        }
    }

    if (Router::ending_addr == target_addr) {
        Assembler::ret(*dbi_code, REG_LR);
        dbi_code++;
        return block_ending(type, block_meta, (uint8_t*)dbi_code, target_addr, target_addr);
    }

    int offset = 0;

    while (true) {
        uint64_t  pc = target_addr + offset;
        cs_disasm(translator_handle, (uint8_t*)pc, 4, target_addr + offset, 1, &translator_insn);
        switch (translator_insn->id) {
            case AARCH64_INS_B: {
                b_ins_handle(dbi_code, pc);

                return block_ending(type, block_meta, (uint8_t*)dbi_code, target_addr, pc);
            }
            case AARCH64_INS_BL: {
                bl_ins_handle(dbi_code, pc);

                return block_ending(type, block_meta, (uint8_t*)dbi_code, target_addr, pc);
            }
            case AARCH64_INS_BR: {
                br_ins_handle(dbi_code, pc);

                return block_ending(type, block_meta, (uint8_t*)dbi_code, target_addr, pc);
            }
            case AARCH64_INS_RET: {
                ret_ins_handle(dbi_code, pc);

                return block_ending(type, block_meta, (uint8_t*)dbi_code, target_addr, pc);
            }
            case AARCH64_INS_ADRP: {
                adrp_ins_handle(dbi_code, pc);
                break;
            }
            default: {
                default_ins_handle(dbi_code, pc);
                break;
            }
        }

        offset += A64_INS_WIDTH;
    }
};

/*
 * push_register
 * mov x1 B_ROUTER_TYPE
 * b.cond pc + 2  // 条件命中
 * b pc + 3  // 未命中
 * mov x0 b_addr
 * mov x16 router
 * br x16
 * mov x0 b_next
 * mov x16 router
 * br x16
 * */
void Translator::b_cond_ins_handle(uint32_t *&writer, uint64_t pc) {
    callback(writer, pc);

    Router::push_register(writer);

    Assembler::write_value_to_reg(writer, REG_X1, B_ROUTER_TYPE);

    uint32_t instruction;
    Assembler::modify_b_cond_addr(instruction, *(uint32_t*)translator_insn->bytes, *writer, *writer + A64_INS_WIDTH * 2);
    *writer = instruction;
    writer++;

    Assembler::b(instruction, *writer, *writer + A64_INS_WIDTH * 8);
    *writer = instruction;
    writer++;

    uint64_t b_addr;
    Assembler::get_b_addr(b_addr, pc, *(uint32_t*)translator_insn->bytes);
    Assembler::write_value_to_reg(writer, REG_X0, b_addr);
    Assembler::br_x16_jump(writer, (uint64_t)router);

    Assembler::write_value_to_reg(writer, REG_X0, pc + A64_INS_WIDTH);
    Assembler::br_x16_jump(writer, (uint64_t)router);
}

/*
 * push_register
 * mov x1 B_ROUTER_TYPE
 * mov x0 b_addr
 * mov x16 router
 * br x16
 * */
void Translator::b_ins_handle(uint32_t *&writer, uint64_t pc) {
    if (Assembler::b_is_cond(*(uint32_t*)translator_insn->bytes)) {
        b_cond_ins_handle(writer, pc);
        return;
    }

    callback(writer, pc);

    Router::push_register(writer);

    Assembler::write_value_to_reg(writer, REG_X1, B_ROUTER_TYPE);

    uint64_t b_addr;
    Assembler::get_b_addr(b_addr, pc, *(uint32_t*)translator_insn->bytes);
    Assembler::write_value_to_reg(writer, REG_X0, b_addr);

    Assembler::br_x16_jump(writer, (uint64_t)router);
}

void Translator::bl_ins_handle(uint32_t *&writer, uint64_t pc) {
    callback(writer, pc);

    Assembler::write_value_to_reg(writer, REG_LR, pc + 4);

    Router::push_register(writer);

    Assembler::write_value_to_reg(writer, REG_X1, BL_ROUTER_TYPE);

    uint64_t bl_addr;
    Assembler::get_bl_addr(bl_addr, pc, *(uint32_t*)translator_insn->bytes);
    Assembler::write_value_to_reg(writer, REG_X0, bl_addr);

    Assembler::br_x16_jump(writer, (uint64_t)router);
}

void Translator::br_ins_handle(uint32_t *&writer, uint64_t pc) {
    callback(writer, pc);

    Router::push_register(writer);

    Assembler::write_value_to_reg(writer, REG_X1, BR_ROUTER_TYPE);

    auto reg = Assembler::get_br_reg(*(uint32_t*)translator_insn->bytes);

    uint32_t instruction;
    Assembler::mov_x_to_x(instruction, REG_X0, reg);
    *writer = instruction;
    writer++;

    Assembler::br_x16_jump(writer, (uint64_t)router);
}

void Translator::adrp_ins_handle(uint32_t *&writer, uint64_t pc) {
    callback(writer, pc);

    uint64_t adrp_addr;
    Assembler::get_adrp_addr(adrp_addr, pc, *(uint32_t*)translator_insn->bytes);
    auto reg = Assembler::get_adrp_reg(*(uint32_t*)translator_insn->bytes);
    Assembler::write_value_to_reg(writer, reg, adrp_addr);
}

void Translator::ret_ins_handle(uint32_t *&writer, uint64_t pc) {
    callback(writer, pc);

    Router::push_register(writer);

    Assembler::write_value_to_reg(writer, REG_X1, RET_ROUTER_TYPE);

    uint32_t instruction;
    Assembler::mov_x_to_x(instruction, REG_X0, REG_LR);
    *writer = instruction;
    writer++;

    Assembler::br_x16_jump(writer, (uint64_t)router);
}

void Translator::callback(uint32_t *&writer, uint64_t pc) {
    Assembler::prolog(writer);
    Assembler::write_pc_to_cpu(writer, pc);
    Assembler::mov_sp_to_x(*writer, REG_X0);
    writer++;
    Assembler::call_dbi_callback(writer);
    Assembler::epilog(writer);
}

void Translator::default_ins_handle(uint32_t *&writer, uint64_t pc) {
    callback(writer, pc);
    *writer = *(uint32_t*)translator_insn->bytes;
    writer++;
}

