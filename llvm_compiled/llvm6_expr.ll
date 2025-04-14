; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @countSetBits(i64 %n_ARG) {
entry:
  %n = alloca i64, align 8
  store i64 %n_ARG, ptr %n, align 4
  %count = alloca i64, align 8
  store i64 0, ptr %count, align 4
  br label %cond

cond:                                             ; preds = %entry
  %n_loaded = load i64, ptr %n, align 4
  %cmptmp = icmp sgt i64 %n_loaded, 0
  %booltmp = zext i1 %cmptmp to i64
  %loopcond = icmp ne i64 %booltmp, 0
  br i1 %loopcond, label %body, label %afterloop

afterloop:                                        ; preds = %body, %cond
  %count_loaded7 = load i64, ptr %count, align 4
  ret i64 %count_loaded7

body:                                             ; preds = %body, %cond
  %n_loaded1 = load i64, ptr %n, align 4
  %n_loaded2 = load i64, ptr %n, align 4
  %subtmp = sub i64 %n_loaded2, 1
  %andtmp = and i64 %n_loaded1, %subtmp
  store i64 %andtmp, ptr %n, align 4
  %count_loaded = load i64, ptr %count, align 4
  %addtmp = add i64 %count_loaded, 1
  store i64 %addtmp, ptr %count, align 4
  %n_loaded3 = load i64, ptr %n, align 4
  %cmptmp4 = icmp sgt i64 %n_loaded3, 0
  %booltmp5 = zext i1 %cmptmp4 to i64
  %loopcond6 = icmp ne i64 %booltmp5, 0
  br i1 %loopcond6, label %body, label %afterloop
}

define i64 @main() {
entry:
  %a = alloca i64, align 8
  %scanfcall = call i64 (ptr, ...) @__isoc99_scanf(ptr @.str, ptr %a)
  %res = alloca i64, align 8
  %a_loaded = load i64, ptr %a, align 4
  %calltmp = call i64 @countSetBits(i64 %a_loaded)
  store i64 %calltmp, ptr %res, align 4
  %res_loaded = load i64, ptr %res, align 4
  %printfcall = call i64 (ptr, ...) @printf(ptr @.str, i64 %res_loaded)
  ret i64 0
}

declare i64 @__isoc99_scanf(ptr, ...)

declare i64 @printf(ptr, ...)
