; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @fact(i64 %n_ARG) {
entry:
  %cmptmp = icmp eq i64 %n_ARG, 0
  %booltmp = zext i1 %cmptmp to i64
  %ifcond = icmp ne i64 %booltmp, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  %subtmp = sub i64 %n_ARG, 1
  %calltmp = call i64 @fact(i64 %subtmp)
  %multmp = mul i64 %n_ARG, %calltmp
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %iftmp = phi i64 [ 1, %then ], [ %multmp, %else ]
  ret i64 %iftmp
}

define i64 @main() {
entry:
  %n = alloca i64, align 8
  %scanfcall = call i64 (ptr, ...) @__isoc99_scanf(ptr @.str, ptr %n)
  %n_loaded = load i64, ptr %n, align 4
  %calltmp = call i64 @fact(i64 %n_loaded)
  %printfcall = call i64 (ptr, ...) @printf(ptr @.str, i64 %calltmp)
  ret i64 0
}

declare i64 @__isoc99_scanf(ptr, ...)

declare i64 @printf(ptr, ...)
