define i32 @main() {
    %1 = alloca float, align 4                                          ; %1 = &b
    store float 0x3ffcccccc0000000, float* %1, align 4                  ; b = 1.8
    %2 = alloca [2 x i32], align 4                                      ; %2 = a
    %3 = getelementptr inbounds [2 x i32], [2 x i32]* %2, i32 0, i32 0  ; %3 = a + 0
    %4 = getelementptr inbounds [2 x i32], [2 x i32]* %2, i32 0, i32 1  ; %4 = a + 1
    store i32 2, i32* %3, align 4                                       ; a[0] = 2
    %5 = load i32, i32* %3, align 4                                     ; %5 = a[0]
    %6 = load float, float* %1, align 4                                 ; %6 = b
    %7 = sitofp i32 %5 to float                                         ; %7 = float(a[0])
    %8 = fmul float %7, %6                                              ; %8 = a[0] * b
    %9 = fptosi float %8 to i32                                         ; %9 = int(a[0] * b)
    store i32 %9, i32* %4, align 4                                      ; a[1] = a[0] * b
    %10 = load i32, i32* %1, align 4                                    ; %10 = b
    ret i32 %10                                                         ; return b
}   