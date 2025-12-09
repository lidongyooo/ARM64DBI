//
// Created by lidongyooo on 2025/12/7.
//

#include "DBI.h"

DBI* DBI::instance = nullptr;

void* DBI::trace(uint64_t target_addr, DBICallback dbi_callback) {
    auto self = getInstance();

    self->dbi_callback_ptr = dbi_callback;
    auto block_meta = Memory::get_or_new_block_meta();
    Translator::scan((uint64_t)target_addr, block_meta, ENDING_ROUTER_TYPE);
//    Utils::show_block_code(block_meta);
//    LOGE("router addr: %p", router);

    return block_meta->block_start;
}

DBICallback DBI::get_dbi_callback () {
    return getInstance()->dbi_callback_ptr;
}

cs_insn* DBI::disassemble(uint64_t pc) {
    auto self = getInstance();

    cs_disasm(self->dbi_cs_handle, (uint8_t*)pc, A64_INS_WIDTH, pc, 1, &self->dbi_cs_insn);
    return self->dbi_cs_insn;
}

csh DBI::get_cs_handle() {
    auto self = getInstance();
    return self->dbi_cs_handle;
}
