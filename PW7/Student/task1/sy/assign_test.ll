; ModuleID = './sy/assign_test.c'
source_filename = "./sy/assign_test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.main.a = private unnamed_addr constant [2 x i32] [i32 2, i32 0], align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca float, align 4
  %3 = alloca [2 x i32], align 4
  store i32 0, ptr %1, align 4
  store float 0x3FFCCCCCC0000000, ptr %2, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 4 %3, ptr align 4 @__const.main.a, i64 8, i1 false)
  %4 = getelementptr inbounds [2 x i32], ptr %3, i64 0, i64 0
  %5 = load i32, ptr %4, align 4
  %6 = sitofp i32 %5 to float
  %7 = load float, ptr %2, align 4
  %8 = fmul float %6, %7
  %9 = fptosi float %8 to i32
  %10 = getelementptr inbounds [2 x i32], ptr %3, i64 0, i64 1
  store i32 %9, ptr %10, align 4
  %11 = load float, ptr %2, align 4
  %12 = fptosi float %11 to i32
  ret i32 %12
}

; Function Attrs: argmemonly nocallback nofree nounwind willreturn
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly nocallback nofree nounwind willreturn }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.2-1"}
