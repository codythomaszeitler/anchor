; ModuleID = 'anchor'
source_filename = "anchor"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [2 x i8] c"2\00", align 1

declare i32 @printf(...)

define i32 @main(...) {
  br i1 true, label %ifStmt, label %ifStmtToo

ifStmt:                                           ; preds = %0
  %1 = call i32 (...) @printf(ptr @0, ptr @1)
  br label %end

ifStmtToo:                                             ; preds = %0
  br label %end
end:
  ret i32 0
}

