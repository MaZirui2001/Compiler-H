define dso_local i32 @add(i32 %0, i32 %1) #0{
    %3 = alloca i32, align 4            ; %3 = &a
    %4 = alloca i32, align 4            ; %4 = &b
    store i32 %0, i32* %3, align 4      ; a = %0
    store i32 %1, i32* %4, align 4      ; b = %1
    %5 = load i32, i32* %3, align 4     ; %5 = a
    %6 = load i32, i32* %4, align 4     ; %6 = b
    %7 = add nsw i32 %5, %6             ; %7 = a + b
    ret i32 %7                          ; return a+b
}
define dso_local i32 @main() #0{
    %1 = alloca i32, align 4            ; %1 = &a
    %2 = alloca i32, align 4            ; %2 = &c
    store i32 3, i32* %1, align 4       ; a = 3
    store i32 5, i32* %2, align 4       ; c = 56
    %3 = load i32, i32* %1, align 4     ; %3 = a
    %4 = add nsw i32 %3, %3             ; %4 = a + a
    %5 = load i32, i32* %2, align 4     ; %5 = a
    %6 = call i32 @add(i32 %3, i32 %4)  ; %6 = add(a, a+a)
    %7 = add nsw i32 %5, %6             ; %7 = c + %6
    ret i32 %7                          ; return c + add(a, a+a)
}