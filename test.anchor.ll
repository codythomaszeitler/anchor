; ModuleID = 'anchor'
source_filename = "anchor"

@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare i32 @printf(...)

define i32 @foo(...) {
  ret i32 1
}

define i32 @main(...) {
  %1 = call i32 (...) @foo()
  %2 = call i32 (...) @printf(ptr @0, i32 %1)
  ret i32 0
}

