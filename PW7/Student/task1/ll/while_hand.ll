@a = dso_local global i32 0, align 4    ; global a = 0
define dso_local i32 @main() #0{            
    %1 = alloca i32, align 4            ; %1 = &return 
    %2 = alloca i32, align 4            ; %2 = &b
    store i32 0, i32* %1, align 4       ; return = 0
    store i32 0, i32* %2, align 4       ; b = 0
    store i32 3, i32* @a, align 4       ; a = 3
    br label %3
3: 
    %4 = load i32, i32* @a, align 4     ; %4 = a
    %5 = load i32, i32* %2, align 4     ; %5 = b
    %6 = icmp sgt i32 %4, 0             ; compare a with 0
    br i1 %6, label %7, label %12       ; if a > 0, go to compare, else, gskip exec 
7:
    %8 = icmp slt i32 %5, 10            ; compare b with 10
    br i1 %8, label %9, label %12       ; if b < 10, go in exec, else, skip exec
9:                                      ; exec
    %10 = add nsw i32 %4, %5            ; %10 = a + b
    store i32 %10, i32* %2, align 4     ; b = a + b
    %11 = sub nsw i32 %4, 1             ; %11 = a - 1
    store i32 %11, i32* @a, align 4     ; a = a - 1
    br label %3                         ; go back to while
12:
    %13 = load i32, i32* %1, align 4    ; %13 = 0
    ret i32 %13                         ; return 0
}   