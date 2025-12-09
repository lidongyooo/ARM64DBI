//
// Created by lidongyooo on 2025/12/7.
//

#include "Assembler.h"

size_t prolog_size = ((uint64_t) end_prolog - (uint64_t) start_prolog) / A64_INS_WIDTH;
auto prolog_ptr = (uint32_t *) start_prolog;
size_t epilog_size = ((uint64_t) end_epilog - (uint64_t) start_epilog) / A64_INS_WIDTH;
auto epilog_ptr = (uint32_t *) start_epilog;

void Assembler::prolog(uint32_t *&writer) {
    for (int i = 0; i < prolog_size; ++i) {
        writer[i] = prolog_ptr[i];
    }
    writer += prolog_size;
}

void Assembler::epilog(uint32_t *&writer) {
    for (int i = 0; i < epilog_size; ++i) {
        writer[i] = epilog_ptr[i];
    }
    writer += epilog_size;
}

void Assembler::movz(uint32_t &instr, ARM64_REGISTER reg, uint16_t immediate, uint8_t shift) {
    uint8_t hw = shift / 16;  // 0, 1, 2, 3

    instr = 0xD2800000 | ((immediate & 0xFFFF) << 5) | ((hw & 0x3) << 21) | (reg & 0x1F);
}

void Assembler::movk(uint32_t &instr, ARM64_REGISTER reg, uint16_t immediate, uint8_t shift) {
    uint8_t hw = shift / 16;  // 0, 1, 2, 3
    instr = 0xF2800000 | ((immediate & 0xFFFF) << 5) | ((hw & 0x3) << 21) | (reg & 0x1F);
}


void Assembler::str(uint32_t &instr, ARM64_REGISTER src_reg, ARM64_REGISTER dst_reg, int dst_offset){
    const uint scale = 8;
    auto imm12 = (uint32_t)(dst_offset / scale);

    uint32_t size = 0x3;
    instr = 0x39000000 | (size << 30) | (imm12 << 10) | (dst_reg << 5) | src_reg;
}

void Assembler::write_value_to_reg(uint32_t *&writer, ARM64_REGISTER reg, uint64_t value) {
    uint16_t parts[4] = {
            static_cast<uint16_t>(value & 0xFFFF),
            static_cast<uint16_t>((value >> 16) & 0xFFFF),
            static_cast<uint16_t>((value >> 32) & 0xFFFF),
            static_cast<uint16_t>((value >> 48) & 0xFFFF)
    };

    int first_nonzero = -1;
    for (int i = 3; i >= 0; i--) {
        if (parts[i] != 0) {
            first_nonzero = i;
            break;
        }
    }

    if (first_nonzero == -1) {
        movz(*writer, reg, 0, 0);
        writer++;
        return;
    }

    movz(*writer, reg, parts[first_nonzero], first_nonzero * 16);
    writer++;

    for (int i = first_nonzero - 1; i >= 0; i--) {
        if (parts[i] != 0) {
            movk(*writer, reg, parts[i], i * 16);
            writer++;
        }
    }
}

void Assembler::mov_sp_to_x(uint32_t &instr, ARM64_REGISTER rd) {
    instr = 0x910003E0;
    instr |= rd;
}

void Assembler::blr(uint32_t &instr, ARM64_REGISTER reg) {
    instr = 0xD63F0000 | (reg << 5);
}

void Assembler::ret(uint32_t &instr, ARM64_REGISTER reg) {
    instr = 0xD65F0000 | (reg << 5);
}

void Assembler::write_pc_to_cpu(uint32_t *&writer, uint64_t pc) {
    write_value_to_reg(writer, REG_X0, pc);
    str(*writer, REG_X0, REG_SP, POINTER_SIZE * 32);
    writer++;
}

void Assembler::call_dbi_callback(uint32_t *&writer) {
    blr_x16_jump(writer, (uint64_t) DBI::get_dbi_callback());
}

void Assembler::get_b_addr(uint64_t &addr, uint64_t pc, uint32_t machine_code) {
    if ((machine_code & 0xFC000000) == 0x14000000) {
        int32_t imm26 = machine_code & 0x03FFFFFF;
        if (imm26 & 0x02000000) imm26 |= 0xFC000000;
        addr = pc + ((int64_t)imm26 << 2);
    } else {
        int32_t imm19 = (machine_code >> 5) & 0x7FFFF;
        if (imm19 & 0x40000) imm19 |= 0xFFF80000;
        addr = pc + ((int64_t)imm19 << 2);
    }
}

void Assembler::get_bl_addr(uint64_t &addr, uint64_t pc, uint32_t machine_code) {
    int32_t imm26 = machine_code & 0x03FFFFFF;  // 低26位

    if (imm26 & 0x02000000) {
        imm26 |= 0xFC000000;  // 0xFC000000 = 0xFFFFFFFF << 26
    }
    addr = pc + ((int64_t)imm26 << 2);
}

void Assembler::get_adrp_addr(uint64_t &addr, uint64_t pc, uint32_t machine_code) {
    uint32_t instr = machine_code;

    int32_t immhi = (instr >> 5) & 0x7FFFF;
    int32_t immlo = (instr >> 29) & 0x3;
    int32_t imm = (immhi << 2) | immlo;

    if (imm & 0x00100000) {
        imm |= 0xFFE00000;
    }

    uint64_t page_base = pc & ~0xFFFULL;
    addr = page_base + ((int64_t)imm << 12);
}

ARM64_REGISTER Assembler::get_adrp_reg(uint32_t machine_code) {
    return (ARM64_REGISTER)(machine_code & 0x1F);
}

ARM64_REGISTER Assembler::get_br_reg(uint32_t machine_code) {
    return (ARM64_REGISTER)((machine_code >> 5) & 0x1F);
}

bool Assembler::b_is_cond(uint32_t machine_code) {
    return !((machine_code & 0xFC000000) == 0x14000000);
}

void Assembler::modify_b_cond_addr(uint32_t &instr, uint32_t machine_code, uint64_t current_pc, uint64_t new_address) {
    uint8_t cond = machine_code & 0xF;

    int64_t byte_offset = new_address - current_pc;
    int32_t word_offset = byte_offset >> 2;

    instr = 0x54000000;
    instr |= (word_offset & 0x7FFFF) << 5;
    instr |= cond;
}

void Assembler::b(uint32_t &instr, uint64_t pc, uint64_t target) {
    int64_t offset = target - pc;

    int64_t word_offset = offset >> 2;

    uint32_t imm26 = word_offset & 0x03FFFFFF;
    instr = 0x14000000 | imm26;
}

void Assembler::br(uint32_t &instr, ARM64_REGISTER reg) {
    instr = 0xD61F0000 | (reg << 5);
}

void Assembler::br_x16_jump(uint32_t *&writer, uint64_t target_addr) {
    write_value_to_reg(writer, REG_X16, target_addr);
    uint32_t instruction;
    br(instruction, REG_X16);
    *writer = instruction;
    writer++;
}

void Assembler::blr_x16_jump(uint32_t *&writer, uint64_t target_addr) {
    write_value_to_reg(writer, REG_X16, target_addr);
    uint32_t instruction;
    blr(instruction, REG_X16);
    *writer = instruction;
    writer++;
}

void Assembler::mov_x_to_x(uint32_t &instr, uint8_t dest_reg, uint8_t src_reg) {
    instr = 0xAA0003E0 | (src_reg << 16) | (dest_reg & 0x1F);
}
