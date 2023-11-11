; ModuleID = 'anchor'
source_filename = "anchor"

%0 = type { ptr, i32 }

@0 = private unnamed_addr constant [2 x i8] c"2\00", align 1
@1 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1

declare i32 @printf(...)

declare ptr @malloc(...)

declare void @free(...)

declare void @memcpy(...)

define ptr @foo() {
  %1 = alloca %0, align 8
  %2 = call ptr (...) @malloc(i32 2)
  %3 = getelementptr inbounds %0, ptr %1, i32 0, i32 0
  call void (...) @memcpy(ptr %2, ptr @0, i32 2)
  store ptr %2, ptr %3, align 8
  %4 = getelementptr inbounds %0, ptr %1, i32 0, i32 1
  store i32 2, ptr %4, align 4
  ret ptr %1
}

define i32 @main(...) {
  %1 = alloca %0, align 8
  %2 = call ptr (...) @malloc(i32 1)
  %3 = getelementptr inbounds %0, ptr %1, i32 0, i32 0
  call void (...) @memcpy(ptr %2, ptr @1, i32 1)
  store ptr %2, ptr %3, align 8
  %4 = getelementptr inbounds %0, ptr %1, i32 0, i32 1
  store i32 1, ptr %4, align 4
  %a = alloca ptr, align 8
  store ptr %1, ptr %a, align 8
  %5 = call ptr @foo()
  store ptr %5, ptr %a, align 8
  %6 = load ptr, ptr %a, align 8
  %7 = getelementptr inbounds %0, ptr %6, i32 0, i32 0
  %8 = load ptr, ptr %7, align 8
  %9 = call i32 (...) @printf(ptr @2, ptr %8)
  ret i32 0
}

