; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @fib(i64 %n_ARG) {
entry:
  %n = alloca i64, align 8
  store i64 %n_ARG, ptr %n, align 4
  %n_loaded = load i64, ptr %n, align 4
  %cmptmp = icmp eq i64 %n_loaded, 0
  %booltmp = zext i1 %cmptmp to i64
  %ifcond = icmp ne i64 %booltmp, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont11

else:                                             ; preds = %entry
  %n_loaded1 = load i64, ptr %n, align 4
  %cmptmp2 = icmp eq i64 %n_loaded1, 1
  %booltmp3 = zext i1 %cmptmp2 to i64
  %ifcond4 = icmp ne i64 %booltmp3, 0
  br i1 %ifcond4, label %then5, label %else6

then5:                                            ; preds = %else
  br label %ifcont

else6:                                            ; preds = %else
  %n_loaded7 = load i64, ptr %n, align 4
  %subtmp = sub i64 %n_loaded7, 1
  %calltmp = call i64 @fib(i64 %subtmp)
  %n_loaded8 = load i64, ptr %n, align 4
  %subtmp9 = sub i64 %n_loaded8, 2
  %calltmp10 = call i64 @fib(i64 %subtmp9)
  %addtmp = add i64 %calltmp, %calltmp10
  br label %ifcont

ifcont:                                           ; preds = %else6, %then5
  %iftmp = phi i64 [ 1, %then5 ], [ %addtmp, %else6 ]
  br label %ifcont11

ifcont11:                                         ; preds = %ifcont, %then
  %iftmp12 = phi i64 [ 0, %then ], [ %iftmp, %ifcont ]
  ret i64 %iftmp12
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
