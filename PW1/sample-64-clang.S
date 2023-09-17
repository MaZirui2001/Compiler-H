        .text
        .file   "sample.c"
        .globl  main                    # -- Begin function main
        .p2align        4, 0x90
        .type   main,@function
main:                                   # @main
        .cfi_startproc
# %bb.0:
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset %rbp, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register %rbp
        movl    $0, -4(%rbp)
        movl    $4, -8(%rbp)
        cmpl    $0, -8(%rbp)
        je      .LBB0_2
# %bb.1:
        movl    -8(%rbp), %eax
        addl    $4, %eax
        movl    %eax, -8(%rbp)
        jmp     .LBB0_3
.LBB0_2:
        movl    -8(%rbp), %eax
        shll    $2, %eax
        movl    %eax, -8(%rbp)
.LBB0_3:
        xorl    %eax, %eax
        popq    %rbp
        .cfi_def_cfa %rsp, 8
        retq
.Lfunc_end0:
        .size   main, .Lfunc_end0-main
        .cfi_endproc
                                        # -- End function
        .ident  "clang version 10.0.1 "
        .section        ".note.GNU-stack","",@progbits
        .addrsig