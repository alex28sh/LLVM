; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1

define i64 @main() {
entry:
  %printfcall = call i64 (ptr, ...) @printf(ptr @.str, i64 8)
  ret i64 0
}

declare i64 @printf(ptr, ...)
