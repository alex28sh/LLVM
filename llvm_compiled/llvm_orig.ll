@.str = private unnamed_addr constant [4 x i8] c"%lf\00", align 1

define dso_local double @fact(double %0) #0 {
  %2 = alloca double, align 8
  %3 = alloca double, align 8
  store double %0, ptr %3, align 8
  %4 = load double, ptr %3, align 8
  %5 = fcmp oeq double %4, 0.000000e+00
  br i1 %5, label %6, label %7

6:
  store double 1.000000e+00, ptr %2, align 8
  br label %13

7:
  %8 = load double, ptr %3, align 8
  %9 = load double, ptr %3, align 8
  %10 = fsub double %9, 1.000000e+00
  %11 = call double @fact(double %10)
  %12 = fmul double %8, %11
  store double %12, ptr %2, align 8
  br label %13

13:
  %14 = load double, ptr %2, align 8
  ret double %14
}

define dso_local double @main() #1 {
  %1 = alloca i32, align 4
  %2 = alloca double, align 8
  store i32 0, ptr %1, align 4
  %3 = call i32 (ptr, ...) @__isoc99_scanf(ptr @.str, ptr %2)
  %4 = load double, ptr %2, align 8
  %5 = call double @fact(double %4)
  %6 = call i32 (ptr, ...) @printf(ptr @.str, double %5)
  ret double 0.000000e+00
}

declare i32 @__isoc99_scanf(ptr noundef, ...) #2

declare i32 @printf(ptr noundef, ...) #2

attributes #0 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress noinline norecurse optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }