; ModuleID = 'io.c'
source_filename = "io.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca [10000 x float], align 16
  %4 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %5 = call i32 @getint()
  store i32 %5, ptr %2, align 4
  %6 = load i32, ptr %2, align 4
  call void @putint(i32 noundef %6)
  call void @llvm.memset.p0.i64(ptr align 16 %3, i8 0, i64 40000, i1 false)
  %7 = getelementptr inbounds <{ float, float, [9998 x float] }>, ptr %3, i32 0, i32 0
  store float 1.000000e+00, ptr %7, align 16
  %8 = getelementptr inbounds <{ float, float, [9998 x float] }>, ptr %3, i32 0, i32 1
  store float 2.000000e+00, ptr %8, align 4
  %9 = getelementptr inbounds [10000 x float], ptr %3, i64 0, i64 0
  %10 = call i32 @getfarray(ptr noundef %9)
  store i32 %10, ptr %4, align 4
  %11 = load i32, ptr %4, align 4
  %12 = add nsw i32 %11, 1
  %13 = getelementptr inbounds [10000 x float], ptr %3, i64 0, i64 0
  call void @putfarray(i32 noundef %12, ptr noundef %13)
  %14 = getelementptr inbounds [10000 x float], ptr %3, i64 0, i64 0
  %15 = load float, ptr %14, align 16
  %16 = fptosi float %15 to i32
  ret i32 %16
}

declare i32 @getint() #1

declare void @putint(i32 noundef) #1

; Function Attrs: argmemonly nocallback nofree nounwind willreturn writeonly
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

declare i32 @getfarray(ptr noundef) #1

declare void @putfarray(i32 noundef, ptr noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { argmemonly nocallback nofree nounwind willreturn writeonly }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.2-1"}
