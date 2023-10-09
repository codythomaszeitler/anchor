; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@0 = private unnamed_addr constant [14 x i8] c"Hello, world!\00", align 1
@1 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

define i32 @foobar() {
entry:
  ret i32 64
}

declare i32 @printf(...)

define i32 @main(...) {
meow:
  %0 = call i32 @foobar()
  %1 = call i32 (...) @printf(ptr @1, i32 %0)
  ret i32 32
}
