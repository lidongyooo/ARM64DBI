.text
.align 4

#define POINTER_SIZE 8

.global router, router_push_register, router_end_push_register, router_pop_register, router_end_pop_register

.extern _router

router_push_register:
    STP X29, X30, [SP, #-(POINTER_SIZE * 22)]!
    ADD X29, SP, #(POINTER_SIZE * 22)

    STP X0, X1, [SP, #(POINTER_SIZE * 2)]
    STP X2, X3, [SP, #(POINTER_SIZE * 4)]
    STP X4, X5, [SP, #(POINTER_SIZE * 6)]
    STP X6, X7, [SP, #(POINTER_SIZE * 8)]
    STP X8, X9, [SP, #(POINTER_SIZE * 10)]
    STP X10, X11, [SP, #(POINTER_SIZE * 12)]
    STP X12, X13, [SP, #(POINTER_SIZE * 14)]
    STP X14, X15, [SP, #(POINTER_SIZE * 16)]
    STP X16, X17, [SP, #(POINTER_SIZE * 18)]
    //保存标志位
    MRS X0, nzcv
    STP X18, X0, [SP, #(POINTER_SIZE * 20)]
router_end_push_register:

router_pop_register:
    LDP X18, X0, [SP, #(POINTER_SIZE * 20)]
    //恢复标志位
    MSR nzcv, X0
    LDP X16, X17, [SP, #(POINTER_SIZE * 18)]
    LDP X14, X15, [SP, #(POINTER_SIZE * 16)]
    LDP X12, X13, [SP, #(POINTER_SIZE * 14)]
    LDP X10, X11, [SP, #(POINTER_SIZE * 12)]
    LDP X8, X9, [SP, #(POINTER_SIZE * 10)]
    LDP X6, X7, [SP, #(POINTER_SIZE * 8)]
    LDP X4, X5, [SP, #(POINTER_SIZE * 6)]
    LDP X2, X3, [SP, #(POINTER_SIZE * 4)]
    LDP X0, X1, [SP, #(POINTER_SIZE * 2)]

    LDP X29, X30, [SP], #(POINTER_SIZE * 22)
router_end_pop_register:

router:
    BL _router
    BR X0