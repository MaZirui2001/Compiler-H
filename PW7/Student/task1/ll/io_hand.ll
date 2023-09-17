define dso_local i32 @main() #0 {
    %1 = alloca i32, align 4                                                        ; %1 = &a                           
    %2 = alloca [10000 x float], align 4                                            ; %2 = b
    %3 = alloca i32, align 4                                                        ; %3 = &n
    %4 = call i32 @getint()                                                         ; %4 = getint()
    store i32 %4, i32* %1, align 4                                                  ; a = getint()
    %5 = load i32, i32* %1, align 4                                                 ; %5 = a
    call void @putint(i32 %5)                                                       ; putint(a)
    %6 = getelementptr inbounds [10000 x float], [10000 x float]* %2, i32 0, i32 0  ; %6 = b + 0
    %7 = getelementptr inbounds [10000 x float], [10000 x float]* %2, i32 0, i32 1  ; %6 = b + 1
    store float 1.000000e+00, float* %6, align 4                                    ; b[0] = 1
    store float 2.000000e+00, float* %7, align 4                                    ; b[1] = 2
    %8 = getelementptr inbounds [10000 x float], [10000 x float]* %2, i64 0, i64 0  ; %8 = b + 0
    %9 = call i32 @getfarray(float* %8)                                             ; %9 = getfarray(b)
    %10 = add nsw i32 %9, 1                                                         ; %10 = n + 1
    call void @putfarray(i32 %10, float* %8)                                        ; putarray(n+1, b)
    %11 = getelementptr inbounds [10000 x float], [10000 x float]* %2, i64 0, i64 0 ; %11 = b + 0
    %12 = load float, float* %11, align 4                                           ; %12 = b[0]
    %13 = fptosi float %12 to i32                                                   ; %13 = int(b[0])
    ret i32 %13                                                                     ; return b[0]
}
declare i32 @getint() #1

declare void @putint(i32) #1

declare i32 @getfarray(float*) #1

declare void @putfarray(i32, float*) #1