; ModuleID = 'llvm3_expr.ll'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

; Function Attrs: nofree nosync nounwind readnone
define i64 @fib(i64 %n_ARG) local_unnamed_addr #0 {
entry:
  br label %tailrecurse

tailrecurse:                                      ; preds = %else6, %entry
  %accumulator.tr = phi i64 [ 0, %entry ], [ %addtmp, %else6 ]
  %n_ARG.tr = phi i64 [ %n_ARG, %entry ], [ %subtmp9, %else6 ]
  %switch = icmp ult i64 %n_ARG.tr, 2
  br i1 %switch, label %ifcont11, label %else6

else6:                                            ; preds = %tailrecurse
  %subtmp = add i64 %n_ARG.tr, -1
  %calltmp = tail call i64 @fib(i64 %subtmp)
  %subtmp9 = add i64 %n_ARG.tr, -2
  %addtmp = add i64 %calltmp, %accumulator.tr
  br label %tailrecurse

ifcont11:                                         ; preds = %tailrecurse
  %accumulator.ret.tr = add i64 %n_ARG.tr, %accumulator.tr
  ret i64 %accumulator.ret.tr
}

define i64 @main() local_unnamed_addr {
entry:
  %n = alloca i64, align 8
  %scanfcall = call i64 (ptr, ...) @__isoc99_scanf(ptr nonnull @.str, ptr nonnull %n)
  %n_loaded = load i64, ptr %n, align 8
  %calltmp = call i64 @fib(i64 %n_loaded)
  %cmptmp = icmp sgt i64 %n_loaded, 0
  br i1 %cmptmp, label %ifcont, label %ifcont15

ifcont:                                           ; preds = %entry
  %subtmp = add nsw i64 %n_loaded, -1
  %calltmp3 = call i64 @fib(i64 %subtmp)
  %addtmp = add i64 %calltmp3, %calltmp
  %cmptmp5.not = icmp eq i64 %n_loaded, 1
  br i1 %cmptmp5.not, label %ifcont15, label %then8

then8:                                            ; preds = %ifcont
  %subtmp11 = add nsw i64 %n_loaded, -2
  %calltmp12 = call i64 @fib(i64 %subtmp11)
  %addtmp13 = add i64 %calltmp12, %addtmp
  br label %ifcont15

ifcont15:                                         ; preds = %entry, %ifcont, %then8
  %sum.1 = phi i64 [ %addtmp13, %then8 ], [ %addtmp, %ifcont ], [ %calltmp, %entry ]
  %printfcall = call i64 (ptr, ...) @printf(ptr nonnull @.str, i64 %sum.1)
  ret i64 0
}

declare i64 @__isoc99_scanf(ptr, ...) local_unnamed_addr

; Function Attrs: nofree nounwind
declare noundef i64 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #1

attributes #0 = { nofree nosync nounwind readnone }
attributes #1 = { nofree nounwind }
