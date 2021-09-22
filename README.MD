### Learning llvm


### Intermediate Representation (IR)
The contents of the files in this section represent a module in llvm which
is the top level structure.

Local values are simliar to registers in assembly language and start with `%`.

https://llvm.org/docs/LangRef.html

#### bitcode
The following section will work on an example that looks like this:
```c
int something(int nr) {
  return nr + 2;
}
```

To generate the bitcode representation (`bc` extension):
```console
$ /usr/bin/clang -emit-llvm -c -o simple.bc simple.c
```
The output `simple.bc` can be converted into assembly representation using:
```console
$ llvm-dis simple.bc -o simple.ll
```
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

To generate the assemble representation:
```console
$ /usr/bin/clang -emit-llvm -c -S -o simple.ll simple.c
```
This command will generate a file name `simple.ll`.

And a file in assembly representation can be converted into bitcode using:
```console
$ llvm-as simple.ll -o simple.bc
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

`m:e' stands for name mangling and e specified that elf type mangling is used.
`i64:64` means that 64 bit integer is 64 bits.

### functions declarations
Are similar to c fuctions syntax:
```
define dso_local i32 @main(i32 %0, i8** %1) #0 {

}
```
dso_local means the compiler may assume that a function or variable marked as
dso_local will resolve to a symbol within the same linkage unit.

The following `i32` is the return value, followed by the name of the function.
Notice the `@main` specifies this as a global.

And main takes one i32 which will be stored in %0, and an pointer-to-pointer
of i8 in %1.

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
file:
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
First space on the stack will be allocated that can hold an i32.
Next, we store the value in `%0` which is the passed in `nr`, which is stored
in `%2`.
Next, the value stored in `%2` will be loaded into `%3` followed by calling
`add` with produces a type of `i32` and stores the result in `%4`.


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
So this is saying store the value that exists in register/local %0 which is of
type i32 into %2 as a pointer to i32.

#### load instruction

```
  %3 = load i32, i32* %2, align 4                                               
```
Load a i32 from %2 (which is a pointer) which I'm guessing is dereferenced?


#### arrays
Are declared like this:
```
[nr x type]
```
For example:
```
[15 x i8]
```

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
