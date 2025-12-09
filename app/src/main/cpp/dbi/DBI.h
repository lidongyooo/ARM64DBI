//
// Created by lidongyooo on 2025/12/7.
//

#ifndef ARM64DBIDEMO_DBI_H
#define ARM64DBIDEMO_DBI_H
#include "../types/types.h"
#include "Memory.h"
#include "Utils.h"
#include "Router.h"
#include "Translator.h"


typedef struct CPU_CONTEXT{
    uint64_t fp; // 栈底
    uint64_t lr;
    int64_t x[29];
    uint64_t sp; // 栈顶
    uint64_t pc;
    uint64_t nzcv;
} CPU_CONTEXT;

typedef void (*DBICallback) (const CPU_CONTEXT* cpu_context);

class DBI {
private:
    csh dbi_cs_handle;
    cs_insn* dbi_cs_insn;
    static DBI* instance;
    DBICallback dbi_callback_ptr;
    DBI() {
        cs_open(CS_ARCH_AARCH64, CS_MODE_ARM, &dbi_cs_handle);
        cs_option(dbi_cs_handle, CS_OPT_DETAIL, CS_OPT_ON);
    }

public:
    DBI(const DBI&) = delete;
    DBI& operator=(const DBI&) = delete;

    static DBI* getInstance() {
        if (!instance) {
            instance = new DBI();
        }
        return instance;
    }

    static void* trace(uint64_t target_addr, DBICallback);

    static DBICallback get_dbi_callback();

    static cs_insn* disassemble(uint64_t pc);
    static csh get_cs_handle();
};

#endif //ARM64DBIDEMO_DBI_H
