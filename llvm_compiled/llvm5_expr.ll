; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @fib(i64 %n_ARG) {
entry:
  %n = alloca i64, align 8
  store i64 %n_ARG, ptr %n, align 4
  %a = alloca i64, align 8
  %b = alloca i64, align 8
  store i64 0, ptr %a, align 4
  store i64 1, ptr %b, align 4
  br label %cond

cond:                                             ; preds = %entry
  %n_loaded = load i64, ptr %n, align 4
  %cmptmp = icmp sge i64 %n_loaded, 1
  %booltmp = zext i1 %cmptmp to i64
  %loopcond = icmp ne i64 %booltmp, 0
  br i1 %loopcond, label %body, label %afterloop

afterloop:                                        ; preds = %body, %cond
  %a_loaded7 = load i64, ptr %a, align 4
  ret i64 %a_loaded7

body:                                             ; preds = %body, %cond
  %c = alloca i64, align 8
  %a_loaded = load i64, ptr %a, align 4
  %b_loaded = load i64, ptr %b, align 4
  %addtmp = add i64 %a_loaded, %b_loaded
  store i64 %addtmp, ptr %c, align 4
  %b_loaded1 = load i64, ptr %b, align 4
  store i64 %b_loaded1, ptr %a, align 4
  %c_loaded = load i64, ptr %c, align 4
  store i64 %c_loaded, ptr %b, align 4
  %n_loaded2 = load i64, ptr %n, align 4
  %subtmp = sub i64 %n_loaded2, 1
  store i64 %subtmp, ptr %n, align 4
  %n_loaded3 = load i64, ptr %n, align 4
  %cmptmp4 = icmp sge i64 %n_loaded3, 1
  %booltmp5 = zext i1 %cmptmp4 to i64
  %loopcond6 = icmp ne i64 %booltmp5, 0
  br i1 %loopcond6, label %body, label %afterloop
}

define i64 @main() {
entry:
  %n = alloca i64, align 8
  %scanfcall = call i64 (ptr, ...) @__isoc99_scanf(ptr @.str, ptr %n)
  %sum = alloca i64, align 8
  %n_loaded = load i64, ptr %n, align 4
  %calltmp = call i64 @fib(i64 %n_loaded)
  store i64 %calltmp, ptr %sum, align 4
  %n_loaded1 = load i64, ptr %n, align 4
  %cmptmp = icmp sge i64 %n_loaded1, 1
  %booltmp = zext i1 %cmptmp to i64
  %ifcond = icmp ne i64 %booltmp, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %sum_loaded = load i64, ptr %sum, align 4
  %n_loaded2 = load i64, ptr %n, align 4
  %subtmp = sub i64 %n_loaded2, 1
  %calltmp3 = call i64 @fib(i64 %subtmp)
  %addtmp = add i64 %sum_loaded, %calltmp3
  store i64 %addtmp, ptr %sum, align 4
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %n_loaded4 = load i64, ptr %n, align 4
  %cmptmp5 = icmp sge i64 %n_loaded4, 2
  %booltmp6 = zext i1 %cmptmp5 to i64
  %ifcond7 = icmp ne i64 %booltmp6, 0
  br i1 %ifcond7, label %then8, label %else14

then8:                                            ; preds = %ifcont
  %sum_loaded9 = load i64, ptr %sum, align 4
  %n_loaded10 = load i64, ptr %n, align 4
  %subtmp11 = sub i64 %n_loaded10, 2
  %calltmp12 = call i64 @fib(i64 %subtmp11)
  %addtmp13 = add i64 %sum_loaded9, %calltmp12
  store i64 %addtmp13, ptr %sum, align 4
  br label %ifcont15

else14:                                           ; preds = %ifcont
  br label %ifcont15

ifcont15:                                         ; preds = %else14, %then8
  %sum_loaded16 = load i64, ptr %sum, align 4
  %printfcall = call i64 (ptr, ...) @printf(ptr @.str, i64 %sum_loaded16)
  ret i64 0
}

declare i64 @__isoc99_scanf(ptr, ...)

declare i64 @printf(ptr, ...)
