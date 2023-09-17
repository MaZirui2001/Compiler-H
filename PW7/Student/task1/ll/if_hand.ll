@a = dso_local global i32 0, align 4    ; global a = 0
define dso_local i32 @main() #0 {           
    %1 = alloca i32, align 4            ; %1 = &return
    store i32 0, i32* %1, align 4       ; return = 0
    store i32 10, i32* @a, align 4      ; a = 10
    %2 = load i32, i32* @a, align 4     ; %2 = a
    %3 = icmp sgt i32 %2, 0             ; compare a with 0
    br i1 %3, label %6 , label %4       ; if a > 0, goto exec, else, goto compare a with 6
4: 
    %5 = icmp slt i32 %2, 6             ; compare a with 6
    br i1 %5, label %6, label %7        ; if a < 6, goto exec, else skip exec
6:                                      ; exec
    store i32 %2, i32* %1, align 4      ; return = a
    br label %9                         ; goto return
7: 
    %8 = sub nsw i32 0, %2              ; %8 = -a
    store i32 %8, i32* %1, align 4      ; return = -a
    br label %9                         ; goto return
9:  
    %10 = load i32, i32* %1, align 4    ; return-data = reuturn
    ret i32 %10                         ; return return-data
}