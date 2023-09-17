        .text
        .file   "sample.c"
        .globl  main                    # -- Begin function main
        .p2align        4, 0x90
        .type   main,@function
main:                                   # @main
# %bb.0:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $8, %esp
        movl    $0, -4(%ebp)
        movl    $4, -8(%ebp)
        cmpl    $0, -8(%ebp)
        je      .LBB0_2
# %bb.1:
        movl    -8(%ebp), %eax
        addl    $4, %eax
        movl    %eax, -8(%ebp)
        jmp     .LBB0_3
.LBB0_2:
        movl    -8(%ebp), %eax
        shll    $2, %eax
        movl    %eax, -8(%ebp)
.LBB0_3:
        xorl    %eax, %eax
        addl    $8, %esp
        popl    %ebp
        retl
.Lfunc_end0:
        .size   main, .Lfunc_end0-main
                                        # -- End function
        .ident  "clang version 10.0.1 "
        .section        ".note.GNU-stack","",@progbits
        .addrsig