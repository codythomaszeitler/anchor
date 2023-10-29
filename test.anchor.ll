; ModuleID = 'anchor'
source_filename = "anchor"

%0 = type { ptr, i32 }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [14 x i8] c"Hello, World!\00", align 1

declare i32 @printf(...)

declare ptr @malloc(...)

declare void @free(...)

declare void @memcpy(...)

define i32 @main(...) {
  %1 = alloca %0, align 8
  %2 = getelementptr inbounds %0, ptr %1, i32 0, i32 0
  ; so %3 should now be a pointer TO a ptr. 

  %output = call i8* @malloc(i32 14)
  call void (...) @memcpy(i8* %output, ptr @1, i32 14)

  store i8* %output, i8** %2

  %printme = load i8*, i8** %2, align 4;

  call i32 (...) @printf(ptr @0, i8* %printme)

  ret i32 0
}