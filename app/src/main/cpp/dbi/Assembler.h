//
// Created by lidongyooo on 2025/12/7.
//

#ifndef ARM64DBIDEMO_ASSEMBLER_H
#define ARM64DBIDEMO_ASSEMBLER_H
#include "../types/types.h"
#include "DBI.h"

typedef enum ARM64_REGISTER {
    REG_X0,
    REG_X1,
    REG_X2,
    REG_X3,
    REG_X4,
    REG_X5,
    REG_X6,
    REG_X7,
    REG_X8,
    REG_X9,
    REG_X10,
    REG_X11,
    REG_X12,
    REG_X13,
    REG_X14,
    REG_X15,
    REG_X16,
    REG_X17,
    REG_X18,
    REG_X19,
    REG_X20,
    REG_X21,
    REG_X22,
    REG_X23,
    REG_X24,
    REG_X25,
    REG_X26,
    REG_X27,
    REG_X28,
    REG_LP,
    REG_LR,
    REG_SP
} ARM64_REGISTER;

class Assembler {
public:
    // 回调用户函数之前保存寄存器
    static void prolog(uint32_t *&writer);

    // 回调用户函数之后恢复寄存器
    static void epilog(uint32_t *&writer);

    // 以下都为生成 ARM64 指令机器码的函数

    static void movz(uint32_t &instr, ARM64_REGISTER reg, uint16_t immediate, uint8_t shift);

    static void movk(uint32_t &instr, ARM64_REGISTER reg, uint16_t immediate, uint8_t shift);

    static void str(uint32_t &instr, ARM64_REGISTER src_reg, ARM64_REGISTER dst_reg, int dst_offset);

    static void blr(uint32_t &instr, ARM64_REGISTER reg);

    static void ret(uint32_t &instr, ARM64_REGISTER  reg);

    static void b(uint32_t &instr, uint64_t pc, uint64_t target);

    static void mov_sp_to_x(uint32_t &instr, ARM64_REGISTER rd);

    static void mov_x_to_x(uint32_t &instr, uint8_t dest_reg, uint8_t src_reg);

    static void write_value_to_reg(uint32_t *&writer, ARM64_REGISTER reg, uint64_t value);

    static void write_pc_to_cpu(uint32_t *&writer, uint64_t pc);

    static void call_dbi_callback(uint32_t *&writer);

    static void get_b_addr(uint64_t &addr, uint64_t pc, uint32_t machine_code);

    static void get_bl_addr(uint64_t &addr, uint64_t pc, uint32_t machine_code);

    static void get_adrp_addr(uint64_t &addr, uint64_t pc, uint32_t machine_code);

    static ARM64_REGISTER get_adrp_reg(uint32_t machine_code);

    static ARM64_REGISTER get_br_reg(uint32_t machine_code);

    static bool b_is_cond(uint32_t machine_code);

    static void modify_b_cond_addr(uint32_t &instr, uint32_t machine_code, uint64_t current_pc, uint64_t new_address);

    static void br(uint32_t &instr, ARM64_REGISTER reg);

    static void br_x16_jump(uint32_t *&writer, uint64_t target_addr);

    static void blr_x16_jump(uint32_t *&writer, uint64_t target_addr);
};

#define POINTER_SIZE 8
#define CPU_CONTEXT_SIZE 34 * POINTER_SIZE

extern "C" {
    void start_prolog();
    void end_prolog();
    void start_epilog();
    void end_epilog();
};

#endif //ARM64DBIDEMO_ASSEMBLER_H
