; ModuleID = 'anchor'
source_filename = "anchor"

@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare i32 @printf(...)

define i32 @main(...) {
  %a = alloca i1, align 1
  store i1 false, ptr %a, align 1
  store i1 true, ptr %a, align 1
  %1 = load i1, ptr %a, align 4
  %2 = call i32 (...) @printf(ptr @0, i1 %1)
  ret i32 0
}

