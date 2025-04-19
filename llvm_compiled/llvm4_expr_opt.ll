; ModuleID = 'llvm4_expr.ll'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

; Function Attrs: nofree norecurse nosync nounwind readnone
define i64 @gcd(i64 %a_ARG, i64 %b_ARG) local_unnamed_addr #0 {
entry:
  %cmptmp = icmp ne i64 %a_ARG, 0
  %cmptmp1 = icmp ne i64 %b_ARG, 0
  %andtmp11 = and i1 %cmptmp, %cmptmp1
  br i1 %andtmp11, label %body, label %afterloop

afterloop:                                        ; preds = %body, %entry
  %a.0 = phi i64 [ %a_ARG, %entry ], [ %a.2, %body ]
  %b.0 = phi i64 [ %b_ARG, %entry ], [ %b.2, %body ]
  %addtmp = add i64 %b.0, %a.0
  ret i64 %addtmp

body:                                             ; preds = %entry, %body
  %a.1 = phi i64 [ %a.2, %body ], [ %a_ARG, %entry ]
  %b.1 = phi i64 [ %b.2, %body ], [ %b_ARG, %entry ]
  %cmptmp5 = icmp slt i64 %b.1, %a.1
  %subtmp = select i1 %cmptmp5, i64 %b.1, i64 0
  %a.2 = sub i64 %a.1, %subtmp
  %subtmp11 = select i1 %cmptmp5, i64 0, i64 %a.1
  %b.2 = sub i64 %b.1, %subtmp11
  %cmptmp13 = icmp ne i64 %a.2, 0
  %cmptmp16 = icmp ne i64 %b.2, 0
  %andtmp1812 = and i1 %cmptmp13, %cmptmp16
  br i1 %andtmp1812, label %body, label %afterloop
}

define i64 @main() local_unnamed_addr {
entry:
  %a = alloca i64, align 8
  %b = alloca i64, align 8
  %scanfcall = call i64 (ptr, ...) @__isoc99_scanf(ptr nonnull @.str, ptr nonnull %a)
  %scanfcall1 = call i64 (ptr, ...) @__isoc99_scanf(ptr nonnull @.str, ptr nonnull %b)
  %a_loaded = load i64, ptr %a, align 8
  %b_loaded = load i64, ptr %b, align 8
  %cmptmp.i = icmp ne i64 %a_loaded, 0
  %cmptmp1.i = icmp ne i64 %b_loaded, 0
  %andtmp11.i = and i1 %cmptmp.i, %cmptmp1.i
  br i1 %andtmp11.i, label %body.i, label %gcd.exit

body.i:                                           ; preds = %entry, %body.i
  %a.1.i = phi i64 [ %a.2.i, %body.i ], [ %a_loaded, %entry ]
  %b.1.i = phi i64 [ %b.2.i, %body.i ], [ %b_loaded, %entry ]
  %cmptmp5.i = icmp slt i64 %b.1.i, %a.1.i
  %subtmp.i = select i1 %cmptmp5.i, i64 %b.1.i, i64 0
  %a.2.i = sub i64 %a.1.i, %subtmp.i
  %subtmp11.i = select i1 %cmptmp5.i, i64 0, i64 %a.1.i
  %b.2.i = sub i64 %b.1.i, %subtmp11.i
  %cmptmp13.i = icmp ne i64 %a.2.i, 0
  %cmptmp16.i = icmp ne i64 %b.2.i, 0
  %andtmp1812.i = and i1 %cmptmp13.i, %cmptmp16.i
  br i1 %andtmp1812.i, label %body.i, label %gcd.exit

gcd.exit:                                         ; preds = %body.i, %entry
  %a.0.i = phi i64 [ %a_loaded, %entry ], [ %a.2.i, %body.i ]
  %b.0.i = phi i64 [ %b_loaded, %entry ], [ %b.2.i, %body.i ]
  %addtmp.i = add i64 %b.0.i, %a.0.i
  %printfcall = call i64 (ptr, ...) @printf(ptr nonnull @.str, i64 %addtmp.i)
  ret i64 0
}

declare i64 @__isoc99_scanf(ptr, ...) local_unnamed_addr

; Function Attrs: nofree nounwind
declare noundef i64 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #1

attributes #0 = { nofree norecurse nosync nounwind readnone }
attributes #1 = { nofree nounwind }
