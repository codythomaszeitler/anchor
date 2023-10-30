; ModuleID = 'anchor'
source_filename = "anchor"

%0 = type { ptr, i32 }

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [2 x i8] c"2\00", align 1
@2 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@3 = private unnamed_addr constant [2 x i8] c"3\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

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
  ; We have now successfully defined a. 

  %5 = alloca %0, align 8
  %6 = call ptr (...) @malloc(i32 2)
  %7 = getelementptr inbounds %0, ptr %5, i32 0, i32 0
  call void (...) @memcpy(ptr %6, ptr @1, i32 2)
  store ptr %6, ptr %7, align 8
  %8 = getelementptr inbounds %0, ptr %5, i32 0, i32 1
  store i32 2, ptr %8, align 4
  store ptr %5, ptr %a, align 8
  ; So now there is a "2" where a is.

  %9 = alloca %0, align 8
  %10 = call ptr (...) @malloc(i32 1)
  %11 = getelementptr inbounds %0, ptr %9, i32 0, i32 0
  call void (...) @memcpy(ptr %10, ptr @2, i32 1)
  store ptr %10, ptr %11, align 8
  %12 = getelementptr inbounds %0, ptr %9, i32 0, i32 1
  store i32 1, ptr %12, align 4
  %b = alloca ptr, align 8
  store ptr %9, ptr %b, align 8
  ; b has now been created

  %13 = alloca %0, align 8
  %14 = call ptr (...) @malloc(i32 2)
  %15 = getelementptr inbounds %0, ptr %13, i32 0, i32 0
  call void (...) @memcpy(ptr %14, ptr @3, i32 2)
  store ptr %14, ptr %15, align 8
  %16 = getelementptr inbounds %0, ptr %13, i32 0, i32 1
  store i32 2, ptr %16, align 4
  store ptr %13, ptr %b, align 8
  ; b should now have "3" in it

  %17 = load ptr, ptr %a, align 8
  %18 = load ptr, ptr %b, align 8
  %19 = getelementptr inbounds %0, ptr %17, i32 0, i32 1
  %20 = load i32, ptr %19, align 4
  %21 = getelementptr inbounds %0, ptr %18, i32 0, i32 1
  %22 = load i32, ptr %21, align 4
  %23 = add i32 %20, %22
  %24 = sub i32 %23, 1
  %25 = call ptr (...) @malloc(i32 %24)
  ; so at this point, you have strings a and b, 

  %26 = alloca %0, align 8
  %27 = getelementptr inbounds %0, ptr %26, i32 0, i32 0
  %28 = load ptr, ptr %27, align 8
  %29 = getelementptr inbounds %0, ptr %17, i32 0, i32 0
  %30 = load ptr, ptr %29, align 8
  %31 = getelementptr inbounds %0, ptr %18, i32 0, i32 0
  %32 = load ptr, ptr %31, align 8
  %33 = getelementptr inbounds i8, ptr %25, i32 %20
  %34 = sub i32 %20, 1
  call void (...) @memcpy(ptr %25, ptr %30, i32 %34)
  call void (...) @memcpy(ptr %33, ptr %32, i32 %22)
  store ptr %25, ptr %28, align 8
  %35 = getelementptr inbounds %0, ptr %26, i32 0, i32 0
  %36 = load ptr, ptr %35, align 8
  %37 = call i32 (...) @printf(ptr @4, ptr %36)
  call i32 (...) @printf(ptr @4, ptr %25)

  %39 = getelementptr inbounds %0, ptr %26, i32 0, i32 0
  %40 = load ptr, ptr %27, align 8
  call i32 (...) @printf(ptr @5, i32 %34)

  ret i32 0
}

