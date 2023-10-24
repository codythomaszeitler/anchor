; ModuleID = 'anchor'
source_filename = "anchor"

@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare i32 @printf(...)

define i32 @main(...) {
  %1 = call i32 (...) @printf(ptr @0, i32 5)
  ret i32 0
}

