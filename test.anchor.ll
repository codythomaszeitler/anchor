; ModuleID = 'anchor'
source_filename = "anchor"

%0 = type { ptr, i32 }

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [2 x i8] c"2\00", align 1
@2 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@3 = private unnamed_addr constant [2 x i8] c"3\00", align 1
@4 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@5 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@6 = private unnamed_addr constant [2 x i8] c"4\00", align 1
@7 = private unnamed_addr constant [3 x i8] c"%s\00", align 1

declare i32 @printf(...)

declare ptr @malloc(...)

declare void @free(...)

declare void @memcpy(...)

define i32 @main(...) {
  %1 = alloca %0, align 8
  %2 = call ptr (...) @malloc(i32 1)
  %3 = getelementptr inbounds %0, ptr %1, i32 0, i32 0
  call void (...) @memcpy(ptr %2, ptr @0, i32 1)
  store ptr %2, ptr %3, align 8
  %4 = getelementptr inbounds %0, ptr %1, i32 0, i32 1
  store i32 1, ptr %4, align 4
  %a = alloca ptr, align 8
  store ptr %1, ptr %a, align 8
  %5 = alloca %0, align 8
  %6 = call ptr (...) @malloc(i32 2)
  %7 = getelementptr inbounds %0, ptr %5, i32 0, i32 0
  call void (...) @memcpy(ptr %6, ptr @1, i32 2)
  store ptr %6, ptr %7, align 8
  %8 = getelementptr inbounds %0, ptr %5, i32 0, i32 1
  store i32 2, ptr %8, align 4
  store ptr %5, ptr %a, align 8
  %9 = alloca %0, align 8
  %10 = call ptr (...) @malloc(i32 1)
  %11 = getelementptr inbounds %0, ptr %9, i32 0, i32 0
  call void (...) @memcpy(ptr %10, ptr @2, i32 1)
  store ptr %10, ptr %11, align 8
  %12 = getelementptr inbounds %0, ptr %9, i32 0, i32 1
  store i32 1, ptr %12, align 4
  %b = alloca ptr, align 8
  store ptr %9, ptr %b, align 8
  %13 = alloca %0, align 8
  %14 = call ptr (...) @malloc(i32 2)
  %15 = getelementptr inbounds %0, ptr %13, i32 0, i32 0
  call void (...) @memcpy(ptr %14, ptr @3, i32 2)
  store ptr %14, ptr %15, align 8
  %16 = getelementptr inbounds %0, ptr %13, i32 0, i32 1
  store i32 2, ptr %16, align 4
  store ptr %13, ptr %b, align 8
  %17 = alloca %0, align 8
  %18 = call ptr (...) @malloc(i32 1)
  %19 = getelementptr inbounds %0, ptr %17, i32 0, i32 0
  call void (...) @memcpy(ptr %18, ptr @4, i32 1)
  store ptr %18, ptr %19, align 8
  %20 = getelementptr inbounds %0, ptr %17, i32 0, i32 1
  store i32 1, ptr %20, align 4
  %c = alloca ptr, align 8
  store ptr %17, ptr %c, align 8
  %21 = load ptr, ptr %a, align 8
  %22 = load ptr, ptr %b, align 8
  %23 = getelementptr inbounds %0, ptr %21, i32 0, i32 1
  %24 = load i32, ptr %23, align 4
  %25 = getelementptr inbounds %0, ptr %22, i32 0, i32 1
  %26 = load i32, ptr %25, align 4
  %27 = add i32 %24, %26
  %28 = sub i32 %27, 1
  %29 = call ptr (...) @malloc(i32 %28)
  %30 = alloca %0, align 8
  %31 = getelementptr inbounds %0, ptr %30, i32 0, i32 0
  %32 = load ptr, ptr %31, align 8
  %33 = getelementptr inbounds %0, ptr %21, i32 0, i32 0
  %34 = load ptr, ptr %33, align 8
  %35 = getelementptr inbounds %0, ptr %22, i32 0, i32 0
  %36 = load ptr, ptr %35, align 8
  %37 = sub i32 %24, 1
  %38 = getelementptr inbounds i8, ptr %29, i32 %37
  %39 = sub i32 %24, 1
  call void (...) @memcpy(ptr %29, ptr %34, i32 %39)
  call void (...) @memcpy(ptr %38, ptr %36, i32 %26)
  store ptr %29, ptr %31, align 8
  store ptr %30, ptr %c, align 8
  %40 = alloca %0, align 8
  %41 = call ptr (...) @malloc(i32 1)
  %42 = getelementptr inbounds %0, ptr %40, i32 0, i32 0
  call void (...) @memcpy(ptr %41, ptr @5, i32 1)
  store ptr %41, ptr %42, align 8
  %43 = getelementptr inbounds %0, ptr %40, i32 0, i32 1
  store i32 1, ptr %43, align 4
  %d = alloca ptr, align 8
  store ptr %40, ptr %d, align 8
  %44 = load ptr, ptr %c, align 8
  %45 = alloca %0, align 8
  %46 = call ptr (...) @malloc(i32 2)
  %47 = getelementptr inbounds %0, ptr %45, i32 0, i32 0
  call void (...) @memcpy(ptr %46, ptr @6, i32 2)
  store ptr %46, ptr %47, align 8
  %48 = getelementptr inbounds %0, ptr %45, i32 0, i32 1
  store i32 2, ptr %48, align 4
  %49 = getelementptr inbounds %0, ptr %44, i32 0, i32 1
  %50 = load i32, ptr %49, align 4
  %51 = getelementptr inbounds %0, ptr %45, i32 0, i32 1
  %52 = load i32, ptr %51, align 4
  %53 = add i32 %50, %52
  %54 = sub i32 %53, 1
  %55 = call ptr (...) @malloc(i32 %54)
  %56 = alloca %0, align 8
  %57 = getelementptr inbounds %0, ptr %56, i32 0, i32 0
  %58 = load ptr, ptr %57, align 8
  %59 = getelementptr inbounds %0, ptr %44, i32 0, i32 0
  %60 = load ptr, ptr %59, align 8
  %61 = getelementptr inbounds %0, ptr %45, i32 0, i32 0
  %62 = load ptr, ptr %61, align 8
  %63 = sub i32 %50, 1
  %64 = getelementptr inbounds i8, ptr %55, i32 %63
  %65 = sub i32 %50, 1
  call void (...) @memcpy(ptr %55, ptr %60, i32 %65)
  call void (...) @memcpy(ptr %64, ptr %62, i32 %52)
  store ptr %55, ptr %57, align 8
  store ptr %56, ptr %d, align 8
  %66 = load ptr, ptr %d, align 8
  %67 = getelementptr inbounds %0, ptr %66, i32 0, i32 0
  %68 = load ptr, ptr %67, align 8
  %69 = call i32 (...) @printf(ptr @7, ptr %68)
  call i32 (...) @printf(ptr @7, ptr %62)
  ret i32 0
}