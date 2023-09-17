; ModuleID = 'go_upstairs.c'
source_filename = "go_upstairs.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@num = dso_local global [2 x i32] [i32 4, i32 8], align 4
@tmp = dso_local global i32 1, align 4
@n = dso_local global i32 0, align 4
@x = dso_local global [1 x i32] zeroinitializer, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @climbStairs(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  %6 = load i32, ptr %3, align 4
  %7 = icmp slt i32 %6, 4
  br i1 %7, label %8, label %10

8:                                                ; preds = %1
  %9 = load i32, ptr %3, align 4
  store i32 %9, ptr %2, align 4
  br label %41

10:                                               ; preds = %1
  %11 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 0
  store i32 0, ptr %11, align 16
  %12 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 1
  store i32 1, ptr %12, align 4
  %13 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 2
  store i32 2, ptr %13, align 8
  store i32 3, ptr %5, align 4
  br label %14

14:                                               ; preds = %19, %10
  %15 = load i32, ptr %5, align 4
  %16 = load i32, ptr %3, align 4
  %17 = add nsw i32 %16, 1
  %18 = icmp slt i32 %15, %17
  br i1 %18, label %19, label %36

19:                                               ; preds = %14
  %20 = load i32, ptr %5, align 4
  %21 = sub nsw i32 %20, 1
  %22 = sext i32 %21 to i64
  %23 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %22
  %24 = load i32, ptr %23, align 4
  %25 = load i32, ptr %5, align 4
  %26 = sub nsw i32 %25, 2
  %27 = sext i32 %26 to i64
  %28 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = add nsw i32 %24, %29
  %31 = load i32, ptr %5, align 4
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %32
  store i32 %30, ptr %33, align 4
  %34 = load i32, ptr %5, align 4
  %35 = add nsw i32 %34, 1
  store i32 %35, ptr %5, align 4
  br label %14, !llvm.loop !6

36:                                               ; preds = %14
  %37 = load i32, ptr %3, align 4
  %38 = sext i32 %37 to i64
  %39 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %38
  %40 = load i32, ptr %39, align 4
  store i32 %40, ptr %2, align 4
  br label %41

41:                                               ; preds = %36, %8
  %42 = load i32, ptr %2, align 4
  ret i32 %42
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %3 = load i32, ptr @num, align 4
  store i32 %3, ptr @n, align 4
  %4 = load i32, ptr @tmp, align 4
  %5 = sext i32 %4 to i64
  %6 = getelementptr inbounds [2 x i32], ptr @num, i64 0, i64 %5
  %7 = load i32, ptr %6, align 4
  store i32 %7, ptr @x, align 4
  %8 = load i32, ptr @n, align 4
  %9 = load i32, ptr @tmp, align 4
  %10 = add nsw i32 %8, %9
  %11 = call i32 @climbStairs(i32 noundef %10)
  store i32 %11, ptr %2, align 4
  %12 = load i32, ptr %2, align 4
  %13 = load i32, ptr @x, align 4
  %14 = sub nsw i32 %12, %13
  ret i32 %14
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.2-1"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
