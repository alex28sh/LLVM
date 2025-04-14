; ModuleID = 'llvm5_expr.ll'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

; Function Attrs: nofree norecurse nosync nounwind readnone
define i64 @fib(i64 %n_ARG) local_unnamed_addr #0 {
entry:
  %cmptmp = icmp sgt i64 %n_ARG, 0
  br i1 %cmptmp, label %body, label %afterloop

afterloop:                                        ; preds = %body, %entry
  %a.0 = phi i64 [ 0, %entry ], [ %b.0, %body ]
  ret i64 %a.0

body:                                             ; preds = %entry, %body
  %n.0 = phi i64 [ %subtmp, %body ], [ %n_ARG, %entry ]
  %a.1 = phi i64 [ %b.0, %body ], [ 0, %entry ]
  %b.0 = phi i64 [ %addtmp, %body ], [ 1, %entry ]
  %addtmp = add i64 %b.0, %a.1
  %subtmp = add nsw i64 %n.0, -1
  %cmptmp4.not = icmp eq i64 %subtmp, 0
  br i1 %cmptmp4.not, label %afterloop, label %body
}

define i64 @main() local_unnamed_addr {
entry:
  %n = alloca i64, align 8
  %scanfcall = call i64 (ptr, ...) @__isoc99_scanf(ptr nonnull @.str, ptr nonnull %n)
  %n_loaded = load i64, ptr %n, align 8
  %cmptmp.i = icmp sgt i64 %n_loaded, 0
  br i1 %cmptmp.i, label %body.i, label %ifcont15

body.i:                                           ; preds = %entry, %body.i
  %n.0.i = phi i64 [ %subtmp.i, %body.i ], [ %n_loaded, %entry ]
  %a.1.i = phi i64 [ %b.0.i, %body.i ], [ 0, %entry ]
  %b.0.i = phi i64 [ %addtmp.i, %body.i ], [ 1, %entry ]
  %addtmp.i = add i64 %b.0.i, %a.1.i
  %subtmp.i = add nsw i64 %n.0.i, -1
  %cmptmp4.not.i = icmp eq i64 %subtmp.i, 0
  br i1 %cmptmp4.not.i, label %fib.exit, label %body.i

fib.exit:                                         ; preds = %body.i
  %subtmp = add nsw i64 %n_loaded, -1
  %cmptmp.i3.not = icmp eq i64 %subtmp, 0
  br i1 %cmptmp.i3.not, label %ifcont15, label %body.i11

body.i11:                                         ; preds = %fib.exit, %body.i11
  %n.0.i5 = phi i64 [ %subtmp.i9, %body.i11 ], [ %subtmp, %fib.exit ]
  %a.1.i6 = phi i64 [ %b.0.i7, %body.i11 ], [ 0, %fib.exit ]
  %b.0.i7 = phi i64 [ %addtmp.i8, %body.i11 ], [ 1, %fib.exit ]
  %addtmp.i8 = add i64 %b.0.i7, %a.1.i6
  %subtmp.i9 = add nsw i64 %n.0.i5, -1
  %cmptmp4.not.i10 = icmp eq i64 %subtmp.i9, 0
  br i1 %cmptmp4.not.i10, label %ifcont, label %body.i11

ifcont:                                           ; preds = %body.i11
  %addtmp = add i64 %b.0.i7, %b.0.i
  %cmptmp5 = icmp sgt i64 %n_loaded, 1
  br i1 %cmptmp5, label %then8, label %ifcont15

then8:                                            ; preds = %ifcont
  %subtmp11 = add nsw i64 %n_loaded, -2
  %cmptmp.i13.not = icmp eq i64 %subtmp11, 0
  br i1 %cmptmp.i13.not, label %fib.exit22, label %body.i21

body.i21:                                         ; preds = %then8, %body.i21
  %n.0.i15 = phi i64 [ %subtmp.i19, %body.i21 ], [ %subtmp11, %then8 ]
  %a.1.i16 = phi i64 [ %b.0.i17, %body.i21 ], [ 0, %then8 ]
  %b.0.i17 = phi i64 [ %addtmp.i18, %body.i21 ], [ 1, %then8 ]
  %addtmp.i18 = add i64 %b.0.i17, %a.1.i16
  %subtmp.i19 = add nsw i64 %n.0.i15, -1
  %cmptmp4.not.i20 = icmp eq i64 %subtmp.i19, 0
  br i1 %cmptmp4.not.i20, label %fib.exit22, label %body.i21

fib.exit22:                                       ; preds = %body.i21, %then8
  %a.0.i14 = phi i64 [ 0, %then8 ], [ %b.0.i17, %body.i21 ]
  %addtmp13 = add i64 %a.0.i14, %addtmp
  br label %ifcont15

ifcont15:                                         ; preds = %entry, %fib.exit, %ifcont, %fib.exit22
  %sum.1 = phi i64 [ %addtmp13, %fib.exit22 ], [ %addtmp, %ifcont ], [ %b.0.i, %fib.exit ], [ 0, %entry ]
  %printfcall = call i64 (ptr, ...) @printf(ptr nonnull @.str, i64 %sum.1)
  ret i64 0
}

declare i64 @__isoc99_scanf(ptr, ...) local_unnamed_addr

; Function Attrs: nofree nounwind
declare noundef i64 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #1

attributes #0 = { nofree norecurse nosync nounwind readnone }
attributes #1 = { nofree nounwind }
