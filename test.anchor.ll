; ModuleID = 'anchor'
source_filename = "anchor"

%0 = type { ptr, i32 }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [14 x i8] c"Hello, World!\00", align 1

declare i32 @printf(...)

declare ptr @malloc(...)

declare void @free(...)

declare void @memcpy(...)

define i32 @bar(...) {
  %1 = alloca %0, align 8
  %2 = call ptr (...) @malloc(i32 14)
  %3 = getelementptr inbounds %0, ptr %1, i32 0, i32 0
  call void (...) @memcpy(ptr %2, ptr @1, i32 14)
  store ptr %2, ptr %3, align 8
  %4 = getelementptr inbounds %0, ptr %1, i32 0, i32 1
  store i32 14, ptr %4, align 4
  %5 = getelementptr inbounds %0, ptr %1, i32 0, i32 0
  %6 = load ptr, ptr %5, align 8
  %7 = call i32 (...) @printf(ptr @0, ptr %6)
  ret i32 0
}

define i32 @main(...) {
  %1 = call i32 (...) @bar()
  ret i32 0
}

