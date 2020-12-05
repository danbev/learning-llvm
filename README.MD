### Learning llvm


### Intermediate Representation (IR)
The contents of the files in this section represent a module in llvm which
is the top level structure.

Local values are simliar to registers in assembly language and start with `%`.

https://llvm.org/docs/LangRef.html

#### bitcode
To generate the gitcode representation:
```console
$ /usr/bin/clang -emit-llvm -c -o simple.bc simple.c
```
The output `simple.bc` can be converted into assembly representation using:
```console
$ llvm-dis simple.bc -o simple.ll
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

