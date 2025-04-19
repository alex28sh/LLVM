; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @fib(i64 %n_ARG) {
entry:
  %n = alloca i64, align 8
  store i64 %n_ARG, ptr %n, align 4
  switch i64 %n_ARG, label %else6 [
    i64 0, label %ifcont11
    i64 1, label %ifcont
  ]

else6:                                            ; preds = %entry
  %subtmp = add i64 %n_ARG, -1
  %calltmp = call i64 @fib(i64 %subtmp)
  %subtmp9 = add i64 %n_ARG, -2
  %calltmp10 = call i64 @fib(i64 %subtmp9)
  %addtmp = add i64 %calltmp10, %calltmp
  br label %ifcont11

ifcont:                                           ; preds = %entry
  br label %ifcont11

ifcont11:                                         ; preds = %else6, %entry, %ifcont
  %iftmp12 = phi i64 [ 0, %entry ], [ %addtmp, %else6 ], [ 1, %ifcont ]
  ret i64 %iftmp12
}

define i64 @main() {
entry:
  %n = alloca i64, align 8
  %scanfcall = call i64 (ptr, ...) @__isoc99_scanf(ptr nonnull @.str, ptr nonnull %n)
  %sum = alloca i64, align 8
  %n_loaded = load i64, ptr %n, align 4
  %calltmp = call i64 @fib(i64 %n_loaded)
  store i64 %calltmp, ptr %sum, align 4
  %n_loaded1 = load i64, ptr %n, align 4
  %cmptmp = icmp sgt i64 %n_loaded1, 0
  br i1 %cmptmp, label %then, label %ifcont

then:                                             ; preds = %entry
  %subtmp = add i64 %n_loaded1, -1
  %calltmp3 = call i64 @fib(i64 %subtmp)
  %addtmp = add i64 %calltmp3, %calltmp
  store i64 %addtmp, ptr %sum, align 4
  %n_loaded4.pre = load i64, ptr %n, align 4
  br label %ifcont

ifcont:                                           ; preds = %entry, %then
  %sum_loaded9 = phi i64 [ %addtmp, %then ], [ %calltmp, %entry ]
  %n_loaded10 = phi i64 [ %n_loaded4.pre, %then ], [ %n_loaded1, %entry ]
  %cmptmp5 = icmp sgt i64 %n_loaded10, 1
  br i1 %cmptmp5, label %then8, label %ifcont15

then8:                                            ; preds = %ifcont
  %subtmp11 = add i64 %n_loaded10, -2
  %calltmp12 = call i64 @fib(i64 %subtmp11)
  %addtmp13 = add i64 %calltmp12, %sum_loaded9
  store i64 %addtmp13, ptr %sum, align 4
  br label %ifcont15

ifcont15:                                         ; preds = %ifcont, %then8
  %sum_loaded16 = phi i64 [ %addtmp13, %then8 ], [ %sum_loaded9, %ifcont ]
  %printfcall = call i64 (ptr, ...) @printf(ptr nonnull @.str, i64 %sum_loaded16)
  ret i64 0
}

declare i64 @__isoc99_scanf(ptr, ...)

declare i64 @printf(ptr, ...)
