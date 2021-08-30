### Learning llvm


### Intermediate Representation (IR)
The contents of the files in this section represent a module in llvm which
is the top level structure.

Local values are simliar to registers in assembly language and start with `%`.

https://llvm.org/docs/LangRef.html

#### bitcode
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
$ /usr/bin/clang -emit-llvm -c -S simple.c
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
define dsa_local i32 @main(i32 %0, i8** %1) #0 {

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
First space on the stack will be allocated that can old an i32.
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


### llc
I LLVM's static compiler which can take a llvm source, for example a `.ll` file
and outputs assemply source for the target platform.

For example, lets take a simple c program and emit the llvm assembly for it:
```console
$ /usr/bin/clang -emit-llvm -c -S print.c
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
If you want to see what gcc would we one can use the following command to
have gcc show the link command options:
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
