; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @gcd(i64 %a_ARG, i64 %b_ARG) {
entry:
  %cmptmp = icmp ne i64 %a_ARG, 0
  %booltmp = zext i1 %cmptmp to i64
  %cmptmp1 = icmp ne i64 %b_ARG, 0
  %booltmp2 = zext i1 %cmptmp1 to i64
  %andtmp = and i64 %booltmp, %booltmp2
  %loopcond = icmp ne i64 %andtmp, 0
  br i1 %loopcond, label %body, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  %b.0 = phi i64 [ %b.2, %ifcont ], [ %b_ARG, %entry ]
  %a.0 = phi i64 [ %a.2, %ifcont ], [ %a_ARG, %entry ]
  %addtmp = add i64 %a.0, %b.0
  ret i64 %addtmp

body:                                             ; preds = %ifcont, %entry
  %b.1 = phi i64 [ %b_ARG, %entry ], [ %b.2, %ifcont ]
  %a.1 = phi i64 [ %a_ARG, %entry ], [ %a.2, %ifcont ]
  %cmptmp5 = icmp sgt i64 %a.1, %b.1
  %booltmp6 = zext i1 %cmptmp5 to i64
  %ifcond = icmp ne i64 %booltmp6, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %body
  %subtmp = sub i64 %a.1, %b.1
  br label %ifcont

else:                                             ; preds = %body
  %subtmp11 = sub i64 %b.1, %a.1
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %b.2 = phi i64 [ %b.1, %then ], [ %subtmp11, %else ]
  %a.2 = phi i64 [ %subtmp, %then ], [ %a.1, %else ]
  %cmptmp13 = icmp ne i64 %a.2, 0
  %booltmp14 = zext i1 %cmptmp13 to i64
  %cmptmp16 = icmp ne i64 %b.2, 0
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
