### Learning llvm
Notes and code samples for learning LLVM.

[Language Reference](https://llvm.org/docs/LangRef.html)

### Intermediate Representation (IR)
The contents of the files in this section represent a module in llvm which
is the top level structure.

Local values are simliar to registers in assembly language and start with `%`.

#### bitcode
The following section will work on an example that looks like this:
```c
int something(int nr) {
  return nr + 2;
}
```

To generate the `bitcode` representation (`bc` extension):
```console
$ /usr/bin/clang -emit-llvm -c -o simple.bc simple.c
```

The output `simple.bc` can be converted into assembly representation using:
```console
$ llvm-dis simple.bc -o simple.ll
```
So lets take a look at the output:
```text
; ModuleID = 'simple.c'
source_filename = "simple.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @something(i32) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = add nsw i32 %3, 2
  ret i32 %4
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.1 (Fedora 9.0.1-2.fc31)"}
```
The sections below will go through the above fields.

To generate the assemble representation:
```console
$ /usr/bin/clang -emit-llvm -c -S -o simple.ll simple.c
```
This command will generate a file name `simple.ll`.

And a file in assembly representation can be converted into bitcode using:
```console
$ llvm-as simple.ll -o simple.bc
```

### Demangling
It can sometimes be difficult to read the IR when the front end uses name
mangling. This can be demangeled using:
```console
$ $ cat main.ll | llvm-cxxfilt
```

#### target datalayout
Contains information about the endienness and type sizes of the `target triple`
This is used for some optimisation that require this information.

```
target datalayout: "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
```
`e` stands for little endian. If it was big endian it would have been `E`.
This is followed by types which have the format:
```
type:<size>:<abi>:<preferred>
```

`m:e' stands for name mangling and `e` specified that `ELF` name mangling is
used.

`i64:64` means that 64 bit integer is 64 bits.

### functions declarations
Are similar to c fuctions syntax:
```
define dso_local i32 @main(i32 %0, i8** %1) #0 {

}
```
`dso_local` means the compiler may assume that a function or variable marked as
`dso_local` will resolve to a symbol within the same linkage unit. `dso` stands
for `dynamic shared object`.

The following `i32` is the return value, followed by the name of the function.
Notice the `@main` specifies this as a global.

And main takes one `i32` which will be stored in `%0`, and a pointer-to-pointer
of `i8` in `%1`.

Then we have the following in the body of the function:
```
; Function Attrs: noinline nounwind optnone uwtable                             
define dso_local i32 @something(i32) #0 {                                       
  %2 = alloca i32, align 4                                                      
  store i32 %0, i32* %2, align 4                                                
  %3 = load i32, i32* %2, align 4                                               
  %4 = add nsw i32 %3, 2                                                        
  ret i32 %4                                                                    
}
```
Notice `#0` which specifies function attributes (listed further down in the
file):
```
attributes #0 = { noinline nounwind optnone uwtable
"correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false"
"less-precise-fpmad"="false" "min-legal-vector-width"="0"
"no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"
"no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false"
"no-signed-zeros-fp-math"="false" "no-trapping-math"="false"
"stack-protector-buffer-size"="8" "target-cpu"="x86-64"
"target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false"
"use-soft-float"="false"
}
```
`alloca` will allocate (similar to man alloca?) on the stack. The type will 
determine the size of the allocation (sizeof(type)).

```c
int something(int nr) {
  return nr + 2;
}
```
Will generate the following llvm IR:
```
; Function Attrs: noinline nounwind optnone uwtable                             
define dso_local i32 @something(i32) #0 {                                       
  %2 = alloca i32, align 4                                                      
  store i32 %0, i32* %2, align 4                                                
  %3 = load i32, i32* %2, align 4                                               
  %4 = add nsw i32 %3, 2                                                        
  ret i32 %4                                                                    
}
```
First space on the stack will be allocated (alloca) that can hold an `i32`.
Next, we store the value in `%0` which is the passed in `nr`, which is stored
in the stack (allocated in the previous instruction) `%2`.

Next, the value stored in `%2` will be loaded into `%3`, followed by calling
`add` with produces a type of `i32` and stores the result in `%4`.
`nsw` stands for `No Signed Wrap` which means that if there signed overflow the
result of this operation will be a poison value.


#### store instruction
Store takes two arguments, a value to store and an address where the value
should be saved.
```
  store <type> <value>, <type>* <pointer>
```
For example:
```
  store i32 %0, i32* %2, align 4                                                
```
So this is saying store the value that exists in register/local `%0` which is of
type `i32` into `%2` as a pointer to `i32`.

#### load instruction

```
  %3 = load i32, i32* %2, align 4                                               
```
Load a `i32` from `%2` (which is a pointer) which I'm guessing is dereferenced?


#### arrays
Are declared like this:
```
%something = type [size/len x type]
```
For example:
```
%something = type [15 x i8]
```

### types/object
Is used to represent a collection of data members in memory. Like a class or
a struct in some programming languages.

For example, a type with two i32 values:
```
%something = type {i32, i32}
```

Another example with a field and a function pointer:
```
%"something_underscore" = type {i32, void (i32)* }
```

Taking a look at a more complicated example:
```
%"unwind::libunwind::_Unwind_Exception" = type { i64, void (i32, %"unwind::libunwind::_Unwind_Exception"*)*, [6 x i64] }
```
So the first part is just the name of the variable which needs quotes because of
the characters used like `::`, and we can see that this is a struct where the
first member is a i64 value, the second is a function pointer which takes a
`i32`, and then a pointer to a struct of this same time. The third argument
is an array of size/len 6 and the types of the entries are `i64`.

Now, if we take a look in the rust source code we can find
`library/unwind/src/libunwind.rs`:
```rust
#[repr(C)]
pub struct _Unwind_Exception {
    pub exception_class: _Unwind_Exception_Class,
    pub exception_cleanup: _Unwind_Exception_Cleanup_Fn,
    pub private: [_Unwind_Word; unwinder_private_data_size],
}

pub type _Unwind_Exception_Cleanup_Fn =
    extern "C" fn(unwind_code: _Unwind_Reason_Code, exception: *mut _Unwind_Exception);
```
So we can see that the `_Unwind_Exception_Cleanup_Fn` is the second parameter
to the function pointer (second member of the struct). So that is only declaring
the a type. And following that we have:
```llvm
 %"unwind::libunwind::_Unwind_Context" = type { [0 x i8] }
```
Now, this is also a variable that is declared and the type is an array of size
0 and of type i8 (1 byte). I think this matched the definition of this enum
found in `library/unwind/src/libunwind.rs`:
```rust
pub enum _Unwind_Context {}
```
And notice that this is an empty enum, so it does not have any values but it
can have associated functions.

```
@vtable.0 = private unnamed_addr constant
     <{ i8*, [16 x i8], i8*, i8*, i8* }>
     <{ i8* bitcast (void (i64**)* @"core::ptr::drop_in_place$LT$std..rt..lang_start$LT$$LP$$RP$$GT$..$u7b$$u7b$closure$u7d$$u7d$$GT$::hfd0e41c7184d2d5c" to i8*), [16 x i8] c"\08\00\00\00\00\00\00\00\08\00\00\00\00\00\00\00", i8* bitcast (i32 (i64**)* @"core::ops::function::FnOnce::call_once$u7b$$u7b$vtable.shim$u7d$$u7d$::h6376c1087f8195c9" to i8*), i8* bitcast (i32 (i64**)* @"std::rt::lang_start::_$u7b$$u7b$closure$u7d$$u7d$::h4a18ec8e9cd2a4c5" to i8*), i8* bitcast (i32 (i64**)* @"std::rt::lang_start::_$u7b$$u7b$closure$u7d$$u7d$::h4a18ec8e9cd2a4c5" to i8*) }>, align 8

```
; Function Attrs: nonlazybind uwtable                                           
declare i32 @rust_eh_personality(i32,
    i32,
    i64,
    %"unwind::libunwind::_Unwind_Exception"*,
    %"unwind::libunwind::_Unwind_Context"*) unnamed_addr #1
```
Recall that a `@` sign means that this is a global identifier and it is external
`unnamed_addr` means that the address is not significant.

```
define internal void @std::sys_common::backtrace::__rust_begin_short_backtrace::hfb9be5a6e963bdda(void ()* %f)
   unnamed_addr #0 personality 
   i32 (i32, i32, i64, %"unwind::libunwind::_Unwind_Exception"*, %"unwind::libunwind::_Unwind_Context"*)* @rust_eh_personality {
```
`#0` is the attribute index.
`personality constant` allows a function to specify what function to use for
exception handling.


### Structures
Normal (non-packed/not padded)) struct:
```
%Foo = type {
  i32,           // no member name, uses index 0
  double }       // no member name, uses index 1
```
So notice that we use indexes instead of named members. But we don't use the
indexes directly as in %someFoo.0, but instead we use `getelementptr` (GEP)
(more on this instruction later).

Packed structs (padded):
```
%Foo = type <{
  i32,           // no member name, uses index 0
  double }>      // no member name, uses index 1
```


### Casts
There are 9 different types of casts in llvm, Bitwise casts which are type
casts, zero-extending casts, sign-extending casts, truncation-casts,
floating-point extending casts, address-space casts (pointer casts), Pointer-to-
integer casts, etc.

#### Bitwise casts (bitcast)
```c
  Foo *foo = (Foo *) malloc(sizeof(Foo));
```

```llvm
  %1 = call i8* @malloc(i32 4)
  %foo = bitcast i8* %1 to %Foo*
```

### Types
* `i1` is a one-bit integer (which is used for booleans for example)
* `i32` is a 32-bit integer
* `iN` is a N-bit integer

Pointer types:
* [4 x i32]* A pointer to array of four i32 values
* i32 addrspace(5)* A pointer to an i32 value that resides in address space 5.
* i32 (i32*) * A pointer to a function that takes an i32*, returning an i32.
* i8* can be used as pointer to void.

### Metadata
One of the main motivations for adding metadata was for debugging information.
Recall that ELF is the Excecutable and Linkable Format, and it contains all the
info required to load an object file (very simplified but I've written about
ELF before). It also contains debug information and the format most commonly
used in ELF is DWARF (see the connection elves and dwarfes).

In llvm IR you might find the following `!3`:
```llmv
%_4 = load void ()*, void ()** %0, align 8, !nonnull !3, !noundef !3
```
The `!3` refers to a metadata section later inte file:
```
!0 = !{i32 7, !"PIC Level", i32 2}                                              
!1 = !{i32 7, !"PIE Level", i32 2}                                              
!2 = !{i32 2, !"RtLibUseGOT", i32 1}                                            
!3 = !{}                                                                        
!4 = !{i32 3310276}
```

### landingpad
This is an llvm instruction which specifies this basic block as a landingpad
for exceptions. This corresponds to the code of catch region of a try-catch
block. For example:
```llvm
cleanup:                                          ; preds = %bb1
  %1 = landingpad { i8*, i32 }
          cleanup
  %2 = extractvalue { i8*, i32 } %1, 0
  %3 = extractvalue { i8*, i32 } %1, 1
  %4 = getelementptr inbounds { i8*, i32 }, { i8*, i32 }* %0, i32 0, i32 0
  store i8* %2, i8** %4, align 8
  %5 = getelementptr inbounds { i8*, i32 }, { i8*, i32 }* %0, i32 0, i32 1
  store i32 %3, i32* %5, align 8
  br label %bb3
```
In this case the optional `cleanup` flags indicates that this is a landing pad
for a cleanup, which I think is for running destructors/drop functions.
In this case this landing pad can catche a struct with a i8* member and a 32
member.

### Invoke instruction
Lets take the following example of an invoke instruction and try to understand
it;
```llvm
%2 = invoke i32 @"std::rt::lang_start::_$u7b$$u7b$closure$u7d$$u7d$::h4a18ec8e9cd2a4c5"(i64** align 8 %_1)
          to label %bb1 unwind label %cleanup
```

* `i32` is the return type of the function being called.
*  `@"std::rt::lang_start::_$u7b$$u7b$closure$u7d$$u7d$::h4a18ec8e9cd2a4c5"`
is the function pointer.
*  `i64** align 8 %_1` are the arguments passed to the function.
*  `to label %bb1` is the basic block to which control will be passed if the
called function returns using `ret`.
* unwind is a mandatory (non-optional) part of the invoke instruction
* `label %cleanup` is the basic block to which control will be passed if the
called functions returns using `resume`.


### Resume instruction


#### Basic blocks
These begin with an optional label, and end with a termination instruction like
branch (br) or return (ret).

#### Branch (br)
The br instruction has two forms, one where there is a condition and one that
is an unconditional branch.
```
br i1, label_if_true, label_if_false

br label_destination
```
`i1` is a single bit integer and `i32` would be an 32-bit integer.

For example using [branch.c](./branch.c) we have the following branch in main:
```llvm
store i32 2, i32* %6, align 4
%8 = load i32, i32* %6, align 4
%9 = icmp eq i32 %8, 2
br i1 %9, label %10, label %11

10:                                               ; preds = %2
  call void @two()
  br label %12

11:                                               ; preds = %2
  call void @one()
  br label %12

12:
  ret i32 0
}
```
Note that the comment `; preds = %2` is saying that %2 is the predecessor for
this basic block.

#### call
```
%7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.2, i64 0, i64 0)) 
```
The first argument is `i32` which is the return value of printf.
After that we have the signature of printf which is `(i8*, ...)` 
@prinf is pointer to the function which is followed by the arguments. Now, in
this case the arguments are retrieved using `getelementptr`

```c
int printf(const char *format, ...);
```

#### getelementptr
Is used to perform address calculations. It takes a a base type that it is
going to operatate on. The second operand is the base pointer itself.

```
@.str.2 = private unnamed_addr constant [10 x i8] c"Bajja...\0A\00", align 1
...

getelementptr inbounds ([10 x i8], [10 x i8]* @.str.2, i64 0, i64 0)) 

base type: [10 x i8]
base ptr:  [10 x i8]* @.str.2
i64 0: base ptr offset, which is a pointer to [10 x i8]
i64 0: offset into above array  
```
These last two indices are somewhat confusing. The first is an offset of the
base type, and the second is a index into that type.
```
@.str.2 = private unnamed_addr constant [3 x i8] c"Ba\0A\00", align 1

getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i64 0, i64 0)) 
                                          |            |     |
                       +------------------+            |     |
                       |-------------------------------|-----+
                       ↓                               |
                       +-+-+--+-+-+-+-+-+-+            |
                       |B|a|\n|x|x|x|y|y|y|            |
                       +-+-+--+-+-+-+-+-+-+            |
                       [  0  ][ 1  ][  2  ]            |
                          ^                            |
                          +----------------------------+

#.str.2 will be equal to 'B'
```
Notice that the type pointed to by the first offset is an array, in this case
it is pointing to the same place as the base pointer. If we want to get a
pointer to an element in this array we use the second index:
```
getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i64 0, i64 1)) 
                                          |            |     |
                       +------------------+            |     |
                       | +-----------------------------|-----+
                       ↓ ↓                             |
                       +-+-+--+-+-+-+-+-+-+            |
                       |B|a|\n|x|x|x|y|y|y|            |
                       +-+-+--+-+-+-+-+-+-+            |
                       [  0  ][ 1  ][  2  ]            |
                          ^                            |
                          +----------------------------+
#.str.2 will be equal to 'a'
```
Notice what happens if we use a larger offset for the first index, this is
like incrementing a pointer
```

getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i64 1, i64 0)) 
                                          |            |     |
                       +------------------+            |     |
                       |      +------------------------|-----+
                       ↓      ↓                        |
                       +-+-+--+-+-+-+-+-+-+            |
                       |B|a|\n|x|x|x|y|y|y|            |
                       +-+-+--+-+-+-+-+-+-+            |
                       [  0  ][ 1  ][  2  ]            |
                                ^                      |
                                +----------------------+
```
The result is the address of the indexed element.

#### struct
```
%name = type {i8, [3 x i8], i32}
```

### llc
I LLVM's static compiler which can take a llvm source, for example a `.ll` file
and outputs assembly source for the target platform.

For example, lets take a simple c program and emit the llvm assembly for it:
```console
$ /usr/bin/clang -emit-llvm -S -o print.ll print.c
```
This will generate a file name `print.ll` which can then be passed to `llc`:
```console
$ /usr/bin/llc print.ll
```
Which will generate a `print.s` file which we can pass to an assembler:
```console
$ /usr/bin/as print.s -o print.o
```
Which will generate an object file named `print.o` which then needs to be
linked. Now, since [print.c](./print.c) used the c library we need to make sure
we link in the correct librarys which would normally be handled by the compiler
tool chain, like clang or gcc:
```console
$ /usr/bin/ld -e main -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
       /lib64/crt1.o /lib64/crti.o -lc -arch x86_64 print.o  \
       /lib64/crtn.o -o print
```
For some background about the libraries specifed here please see
[learning-linux-kernel](https://github.com/danbev/learning-linux-kernel#networking).
If you want to see what gcc would use to link one can use the following command
to have gcc show the link command options:
```console
$ gcc -### -o print_out print.c
```

Now we can run our executable:
```console
$ ./print
something....
```

One can use the `opt` command to generate some useful information in the
assembly format, like named arguments and labels:
```console
$ /usr/bin/opt -S -mem2reg -instnamer print.ll -o print_before_opt.ll
```

### Link Time Optimizations (LTO)
As then name suggests these are optimizations that the linker can perform on
all the object files that it is linking, so the linker puts all the objects
files together and combines them into a single program.

So the linker would merge all the object files (which I think can also be
llvm bit code files but not sure yet) into one big one and then do optimizations
the one big merged file. This can allow for optimizations that where not
discovered/possible in earlier stages but can also introduce scalablitlity
or memory issues while linking.

So with LTO enabled, by using `-flto
```console
$ make lto_a.o
/usr/bin/clang -flto -c -o lto_a.o lto_a.c

$ file lto_a.o
lto_a.o: LLVM IR bitcode
```
So we can indeed see that this is a LLVM IR bitcode file and we can inspect it
using:
```console
$ llvm-dis lto_a.o -o lto_a.ll
```
And we compile lto_main as a normal object file:
```console
$ make lto_main.o 
/usr/bin/clang -c -o lto_main.o lto_main.c 
```
And finally we compile the executable passing in the LLVM IR bitcode and the
object code:
```console
$ make lto_main
/usr/bin/clang -flto lto_a.o lto_main.o -o main
```

### lld (LLVM portable)
lld is LLVM's linker and supports ELF, COFF, and Mach-O.
Can be enabled by passing `-fuse-ld=lld` to clang/gcc:
```
-fuse-ld=lld
```

### bfd linker (GNU)
Is written on top of the [BFD library](https://en.wikipedia.org/wiki/Binary_File_Descriptor_library)
This is the traditional GNU binutils linker named `ld`.
```
-fuse-ld=bfd
```

### gold linker (GNU)
Was written to remove the BFD library abstraction layer.
This is the next/news GNU binutils linker named `gold`:
```
-fuse-ld=gold
```

### LLVM Exception Handling (eh)

### invoke
A call inside a try scope will will be replaces by the LLVM frontend with an
invoke instruction. This instruction has two points of continuation, one for
a normal successful call, and one if the call raises an exception.
In the case of an exception the place where the code continues execution is
called the `landing pad`.
These are alternative function entry points where an exception structure ref
and a type info index is passed as arguments.

```
invoke void @_Z9somethingi(i32 1)                                             
          to label %9 unwind label %10
      (if successful) (if throws)

(if successful)
9:                                                ; preds = %2                  
  br label %24                                                                  
                                                                                
(if throws)
10:                                               ; preds = %2                  
  %11 = landingpad { i8*, i32 }   // i8* = exception pointer part, i32=selector value part                                                
          catch i8* bitcast (i8** @_ZTIi to i8*)                                
  %12 = extractvalue { i8*, i32 } %11, 0                                        
  store i8* %12, i8** %6, align 8                                               
  %13 = extractvalue { i8*, i32 } %11, 1                                        
  store i32 %13, i32* %7, align 4                                               
  br label %14        
```


### LLVM tutorial example
This section follows the [tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl01.html)
and contains notes around it.

Building:
```console
$ make main
"clang++" "-g" -o main lang.o main.cc
```
Running the example:
```console
$ ./main lang.example
```

### libunwind
```console
$ sudo yum install libunwind-devel
```

```
$ readelf --debug-dump=frames libunwind
```

### poison value
TODO:
