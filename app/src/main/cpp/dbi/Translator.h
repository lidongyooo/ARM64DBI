//
// Created by lidongyooo on 2025/12/7.
//

#ifndef ARM64DBIDEMO_TRANSLATOR_H
#define ARM64DBIDEMO_TRANSLATOR_H
#include "../types/types.h"
#include "Memory.h"
#include "Router.h"
#include "Assembler.h"

class Translator{
public:
    // 扫描与翻译原基本块
    static void scan(uint64_t target_addr, BlockMeta* block_meta, ROUTER_TYPE type);
    // 翻译非跳转与PC无关指令的函数
    static void default_ins_handle(uint32_t *&writer, uint64_t pc);
    // 回调用户函数
    static void callback(uint32_t *&writer, uint64_t pc);
    // 翻译 B 指令
    static void b_ins_handle(uint32_t *&writer, uint64_t pc);
    // 翻译 BL 指令
    static void bl_ins_handle(uint32_t *&writer, uint64_t pc);
    // 翻译 BR 指令
    static void br_ins_handle(uint32_t *&writer, uint64_t pc);
    // 翻译 B.COND 指令
    static void b_cond_ins_handle(uint32_t *&writer, uint64_t pc);
    // 翻译 ADRP 指令
    static void adrp_ins_handle(uint32_t *&writer, uint64_t pc);
    // 翻译 RET 指令
    static void ret_ins_handle(uint32_t *&writer, uint64_t pc);
};


#endif //ARM64DBIDEMO_TRANSLATOR_H
