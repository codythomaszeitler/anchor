; ModuleID = 'anchor'
source_filename = "anchor"

@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare i32 @printf(...)

define i32 @main(...) {
  %a = alloca i32, align 4
  store i32 0, ptr %a, align 4
  store i32 0, ptr %a, align 4

whileLoopStart:                                   ; preds = %body
  %1 = load i32, ptr %a, align 4
  %2 = icmp slt i32 %1, 3
  br i1 %2, label %body, label %end

body:                                             ; preds = %whileLoopStart
  %3 = load i32, ptr %a, align 4
  %4 = call i32 (...) @printf(ptr @0, i32 %3)
  %5 = load i32, ptr %a, align 4
  %6 = add i32 %5, 1
  store i32 %6, ptr %a, align 4
  br label %whileLoopStart

end:                                              ; preds = %whileLoopStart
  ret i32 0
}

