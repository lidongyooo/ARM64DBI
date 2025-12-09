.text
.align 4

#define POINTER_SIZE 8
#define CPU_CONTEXT_SIZE 34 * POINTER_SIZE

.global start_prolog, end_prolog, start_epilog, end_epilog

start_prolog:
    STP X29, X30, [SP, #-CPU_CONTEXT_SIZE]!
    ADD X29, SP, #CPU_CONTEXT_SIZE

    STP X0, X1, [SP, #(POINTER_SIZE * 2)]
    STP X2, X3, [SP, #(POINTER_SIZE * 4)]
    STP X4, X5, [SP, #(POINTER_SIZE * 6)]
    STP X6, X7, [SP, #(POINTER_SIZE * 8)]
    STP X8, X9, [SP, #(POINTER_SIZE * 10)]
    STP X10, X11, [SP, #(POINTER_SIZE * 12)]
    STP X12, X13, [SP, #(POINTER_SIZE * 14)]
    STP X14, X15, [SP, #(POINTER_SIZE * 16)]
    STP X16, X17, [SP, #(POINTER_SIZE * 18)]
    STP X18, X19, [SP, #(POINTER_SIZE * 20)]
    STP X20, X21, [SP, #(POINTER_SIZE * 22)]
    STP X22, X23, [SP, #(POINTER_SIZE * 24)]
    STP X24, X25, [SP, #(POINTER_SIZE * 26)]
    STP X26, X27, [SP, #(POINTER_SIZE * 28)]

    //将原 SP 写入到上下文
    ADD X0, SP, #CPU_CONTEXT_SIZE
    STP X28, X0, [SP, #(POINTER_SIZE * 30)]
    //保存标志位
    MRS X0, nzcv
    STR X0, [SP, #(POINTER_SIZE * 33)]
end_prolog:

start_epilog:
    //恢复标志位
    LDR X0, [SP, #(POINTER_SIZE * 33)]
    MSR nzcv, X0
    LDR X28, [SP, #(POINTER_SIZE * 30)]
    LDP X26, X27, [SP, #(POINTER_SIZE * 28)]
    LDP X24, X25, [SP, #(POINTER_SIZE * 26)]
    LDP X22, X23, [SP, #(POINTER_SIZE * 24)]
    LDP X20, X21, [SP, #(POINTER_SIZE * 22)]
    LDP X18, X19, [SP, #(POINTER_SIZE * 20)]
    LDP X16, X17, [SP, #(POINTER_SIZE * 18)]
    LDP X14, X15, [SP, #(POINTER_SIZE * 16)]
    LDP X12, X13, [SP, #(POINTER_SIZE * 14)]
    LDP X10, X11, [SP, #(POINTER_SIZE * 12)]
    LDP X8, X9, [SP, #(POINTER_SIZE * 10)]
    LDP X6, X7, [SP, #(POINTER_SIZE * 8)]
    LDP X4, X5, [SP, #(POINTER_SIZE * 6)]
    LDP X2, X3, [SP, #(POINTER_SIZE * 4)]
    LDP X0, X1, [SP, #(POINTER_SIZE * 2)]

    LDP X29, X30, [SP], #CPU_CONTEXT_SIZE
end_epilog: