; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @gcd(i64 %a_ARG, i64 %b_ARG) {
entry:
  %a = alloca i64, align 8
  %b = alloca i64, align 8
  store i64 %a_ARG, ptr %a, align 4
  store i64 %b_ARG, ptr %b, align 4
  br label %cond

cond:                                             ; preds = %entry
  %a_loaded = load i64, ptr %a, align 4
  %cmptmp = icmp ne i64 %a_loaded, 0
  %booltmp = zext i1 %cmptmp to i64
  %b_loaded = load i64, ptr %b, align 4
  %cmptmp1 = icmp ne i64 %b_loaded, 0
  %booltmp2 = zext i1 %cmptmp1 to i64
  %andtmp = and i64 %booltmp, %booltmp2
  %loopcond = icmp ne i64 %andtmp, 0
  br i1 %loopcond, label %body, label %afterloop

afterloop:                                        ; preds = %ifcont, %cond
  %a_loaded20 = load i64, ptr %a, align 4
  %b_loaded21 = load i64, ptr %b, align 4
  %addtmp = add i64 %a_loaded20, %b_loaded21
  ret i64 %addtmp

body:                                             ; preds = %ifcont, %cond
  %a_loaded3 = load i64, ptr %a, align 4
  %b_loaded4 = load i64, ptr %b, align 4
  %cmptmp5 = icmp sgt i64 %a_loaded3, %b_loaded4
  %booltmp6 = zext i1 %cmptmp5 to i64
  %ifcond = icmp ne i64 %booltmp6, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %body
  %a_loaded7 = load i64, ptr %a, align 4
  %b_loaded8 = load i64, ptr %b, align 4
  %subtmp = sub i64 %a_loaded7, %b_loaded8
  store i64 %subtmp, ptr %a, align 4
  br label %ifcont

else:                                             ; preds = %body
  %b_loaded9 = load i64, ptr %b, align 4
  %a_loaded10 = load i64, ptr %a, align 4
  %subtmp11 = sub i64 %b_loaded9, %a_loaded10
  store i64 %subtmp11, ptr %b, align 4
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %a_loaded12 = load i64, ptr %a, align 4
  %cmptmp13 = icmp ne i64 %a_loaded12, 0
  %booltmp14 = zext i1 %cmptmp13 to i64
  %b_loaded15 = load i64, ptr %b, align 4
  %cmptmp16 = icmp ne i64 %b_loaded15, 0
  %booltmp17 = zext i1 %cmptmp16 to i64
  %andtmp18 = and i64 %booltmp14, %booltmp17
  %loopcond19 = icmp ne i64 %andtmp18, 0
  br i1 %loopcond19, label %body, label %afterloop
}

define i64 @main() {
entry:
  %a = alloca i64, align 8
  %b = alloca i64, align 8
  %scanfcall = call i64 (ptr, ...) @__isoc99_scanf(ptr @.str, ptr %a)
  %scanfcall1 = call i64 (ptr, ...) @__isoc99_scanf(ptr @.str, ptr %b)
  %a_loaded = load i64, ptr %a, align 4
  %b_loaded = load i64, ptr %b, align 4
  %calltmp = call i64 @gcd(i64 %a_loaded, i64 %b_loaded)
  %printfcall = call i64 (ptr, ...) @printf(ptr @.str, i64 %calltmp)
  ret i64 0
}

declare i64 @__isoc99_scanf(ptr, ...)

declare i64 @printf(ptr, ...)
