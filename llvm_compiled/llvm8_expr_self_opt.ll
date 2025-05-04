; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @main() {
entry:
  br label %ifcont

ifcont:                                           ; preds = %entry
  %printfcall1 = call i64 (ptr, ...) @printf(ptr @.str, i64 1)
  ret i64 0
}

declare i64 @printf(ptr, ...)
