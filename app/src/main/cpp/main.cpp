#include <jni.h>
#include <string>
#include "dbi/DBI.h"


void quick_sort(int arr[], int left, int right) {
    if (left >= right) return;

    int i = left, j = right;
    int pivot = arr[left];

    while (i < j) {
        while (i < j && arr[j] >= pivot) j--;
        if (i < j) arr[i++] = arr[j];

        while (i < j && arr[i] <= pivot) i++;
        if (i < j) arr[j--] = arr[i];
    }

    arr[i] = pivot;
    quick_sort(arr, left, i - 1);
    quick_sort(arr, i + 1, right);
}

void quick_sort_end(){};

typedef int (*quick_sort_sign) (int arr[], int left, int right);

bool get_register_value(aarch64_reg reg, const CPU_CONTEXT* ctx, uint64_t& out_value) {
    uint64_t value = 0;

    if (reg >= AARCH64_REG_W0 && reg <= AARCH64_REG_W30) {
        int idx = reg - AARCH64_REG_W0;
        value = ctx->x[idx] & 0xFFFFFFFF;
    } else if (reg >= AARCH64_REG_X0 && reg <= AARCH64_REG_X28) {
        int idx = reg - AARCH64_REG_X0;
        value = ctx->x[idx];
    } else {
        switch (reg) {
            case AARCH64_REG_SP: value = ctx->sp; break;
            case AARCH64_REG_FP: value = ctx->fp; break;      // AARCH64_REG_X29
            case AARCH64_REG_LR: value = ctx->lr; break;      // AARCH64_REG_X30
            default:
                return false;
        }
    }

    out_value = value;
    return true;
}


void dbi_callback (const CPU_CONTEXT* cpu_context) {
    auto cs_insn = DBI::disassemble(cpu_context->pc);
    char reg_value_info[1024] = {};
    char *reg_value_info_ptr = reg_value_info;

    for (int i = 0; i < cs_insn->detail->aarch64.op_count; i++) {
        auto op = cs_insn->detail->aarch64.operands[i];
        const char* reg_name;
        uint64_t reg_value;
        if (op.type == AARCH64_OP_REG || op.type == AARCH64_OP_MEM_REG) {
            reg_name = cs_reg_name(DBI::get_cs_handle(), op.reg);
            get_register_value(op.reg, cpu_context, reg_value);
            int len = snprintf(reg_value_info_ptr, sizeof(reg_value_info),
                               "%s=0x%llx ", reg_name, reg_value);
            reg_value_info_ptr += len;
        }
    }
    LOGE("0x%llx: %s %s; %s", cpu_context->pc, cs_insn->mnemonic, cs_insn->op_str, reg_value_info);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_lidongyooo_arm64dbidemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    Utils::show_code((void*)quick_sort, ((uint64_t)quick_sort_end - (uint64_t) quick_sort) / A64_INS_WIDTH);
    LOGE("quick_sort_end %llx", (uint64_t)quick_sort_end);
    auto quick_sort_addr = (uint64_t)quick_sort;

    int arr[] = {9, 3, 5, 8, 10, 1, 2, 6, 4};
    int n = sizeof(arr) / sizeof(arr[0]);
    auto dbi_quick_sort = (quick_sort_sign)DBI::trace(quick_sort_addr, dbi_callback);
//    quick_sort(arr, 0, n - 1);

    dbi_quick_sort(arr, 0, n - 1);

    for (int i = 0; i < n; i++) {
        LOGE("%d", arr[i]);
    }

    std::string hello = "Hello from DBI";
    return env->NewStringUTF(hello.c_str());
}