//
// Created by lidongyooo on 2025/12/7.
//

#include "Utils.h"


void Utils::show_code(void* code_addr, int number) {
    csh handle;
    cs_open(CS_ARCH_AARCH64, CS_MODE_ARM, &handle);
    int offset = 0;
    auto insn = (cs_insn *)malloc(sizeof(cs_insn));
    while (true) {
        cs_disasm(handle, (uint8_t *) code_addr + offset, 4, (uint64_t) code_addr + offset, 1, &insn);
        offset += 4;

        LOGE("%#llx: %s %s", insn->address, insn->mnemonic, insn->op_str);

        if (offset / 4 >= number) {
            break;
        }
    }

    free(insn);
    cs_close(&handle);
}

void Utils::show_block_code(BlockMeta* block_meta) {
    show_code(block_meta->block_start, block_meta->block_size / 4);
}
