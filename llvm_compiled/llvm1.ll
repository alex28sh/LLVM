; ModuleID = 'my cool jit'
source_filename = "my cool jit"
@.str = private unnamed_addr constant [4 x i8] c"%lf\00", align 1

define double @fact(double %n) {
entry:
  %cmptmp = fcmp ueq double %n, 0.000000e+00
  %booltmp = uitofp i1 %cmptmp to double
  %ifcond = fcmp one double %booltmp, 0.000000e+00
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  %subtmp = fsub double %n, 1.000000e+00
  %calltmp = call double @fact(double %subtmp)
  %multmp = fmul double %n, %calltmp
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %iftmp = phi double [ 1.000000e+00, %then ], [ %multmp, %else ]
  ret double %iftmp
}

define double @main() {
entry:
  %n = alloca double, align 8
  %scanfcall = call i32 (ptr, ...) @__isoc99_scanf(ptr @.str, ptr %n)
  %n_loaded = load double, ptr %n, align 8
  %calltmp = call double @fact(double %n_loaded)
  %printfcall = call i32 (ptr, ...) @printf(ptr @.str, double %calltmp)
  ret double 0.000000e+00
}

declare i32 @__isoc99_scanf(ptr, ...)

declare i32 @printf(ptr, ...)