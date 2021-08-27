### lldb notes

    lldb test/.libs/mergesort_test

    (lldb) breakpoint set --line 41
    (lldb) breakpoint set --file mergesort.cpp --line 33
    (lldb) run

Show frame variables:

    (lldb) frame variable
    (lldb) fr v

Show current source:

    (lldb) source list
or
    (lldb) frame select
or 
    (lldb) source info

Source step:

    (lldb) thread step-in
    (lldb) s
    (lldb) thread step-over
    (lldb) n
    (lldb) thread step-out
    (lldb) finish

Show frames:

    (lldb) thread backtrace
    (lldb) bt

A useful argument is --count which limits the backtrace, for example:

    (lldb) br -c 3


This can be useful when inspecting recursive algorithms. For example, mergesort may look something like this:

    (lldb) thread backtrace
    * thread #1: tid = 0x173e0, 0x0000000100000d4b mergesort`mergeSort(arr=0x00007fff5fbff620, len=4, aux=0x00007fff5fbff5c0, low=0, high=0) + 27 at mergesort.cpp:23, queue = 'com.apple.main-thread', stop reason = step in
      * frame #0: 0x0000000100000d4b mergesort`mergeSort(arr=0x00007fff5fbff620, len=4, aux=0x00007fff5fbff5c0, low=0, high=0) + 27 at mergesort.cpp:23
        frame #1: 0x0000000100000d92 mergesort`mergeSort(arr=0x00007fff5fbff620, len=4, aux=0x00007fff5fbff5c0, low=0, high=1) + 98 at mergesort.cpp:27
        frame #2: 0x0000000100000d92 mergesort`mergeSort(arr=0x00007fff5fbff620, len=4, aux=0x00007fff5fbff5c0, low=0, high=2) + 98 at mergesort.cpp:27
        frame #3: 0x0000000100000d92 mergesort`mergeSort(arr=0x00007fff5fbff620, len=4, aux=0x00007fff5fbff5c0, low=0, high=4) + 98 at mergesort.cpp:27
        frame #4: 0x0000000100000e42 mergesort`merge_sort(arr=0x00007fff5fbff620, len=4) + 98 at mergesort.cpp:34
        frame #5: 0x0000000100000eb7 mergesort`main + 71 at mergesort.cpp:41
        frame #6: 0x00007fff922575c9 libdyld.dylib`start + 1

Printing an array:

    (lldb) p *(int(*)[5]) arr

Saving breakpoints:  
Sometime you want to exit lldb and recompile to make change in the tested code. This means you hhave to reenter you breakpoints. But you can also specify command to run when lldb start using the ```-S filename``` argument:

  	lldb -S breakpoints test/.libs/pattern_test

Adding a conditional break point:  

    (lldb) breakpoint set --file heapsort.cpp --line 41 --condition 'end==1'

Here _end_ is a local variable.

Adding a watch point:  

    (lldb) watchpoint set var end

Every time the above variable _end_ is modified it will be displayed:

    Watchpoint 1 hit:
    old value: 2
    new value: 1

You can also add a condition so that it breaks when this condition applies:  

    (lldb) watchpoint modify -c 'end==1'


Watch read and writes:

    (lldb) wa s v --watch read_write mp->nm_modname

Creating variables:
If you prefix a variable with $ you can use it in future commands, for example:

    (lldb) expr -- int* $d = new int{20}
    (lldb) expr $d
  

Calling a function:
    (lldb) expr -- printf("bajja\n")

#### Show local variable that clashes with global
Say you have a function that takes a parameter named args. If you try to print the
value of it you'll get:
```console
$ (lldb) expr args
error: reference to 'args' is ambiguous
candidate found by name lookup is 'args'
candidate found by name lookup is 'args'
candidate found by name lookup is 'args'
```
To work around this you can:
```console
(lldb) fr v args
(const char *const [4]) args = ([0] = "../src/tcp_wrap.cc", [1] = "146", [2] = "args.IsConstructCall()", [3] = "static void node::TCPWrap::New(const FunctionCallbackInfo<v8::Value> &)")
(lldb) fr v args[0]
(const char *const) args[0] = 0x0000000102620641 "../src/tcp_wrap.cc"
(lldb) fr v args[3]
(const char *const) args[3] = 0x000000010262077c "static void node::TCPWrap::New(const FunctionCallbackInfo<v8::Value> &)"
```

#### Add command on breakpoint
```console
(lldb) br s -n somebreakpoint
(lldb) br command add 1.1
> expr timer_
DONE
```
And we can also use python, for example:
```console
(lldb) br s -p. -X main -G true
(lldb) br command add -s python
Enter your Python command(s). Type 'DONE' to end.
def function (frame, bp_loc, internal_dict):
    """frame: the lldb.SBFrame for the location at which you stopped
       bp_loc: an lldb.SBBreakpointLocation for the breakpoint location information
       internal_dict: an LLDB support object not to be used"""
print("Line nr: {}".format(frame.GetLineEntry().GetLine()))
DONE
(lldb) r
Process 2989524 launched: '/home/danielbevenius/work/llvm/learning-llvm/main' (x86_64)
Line nr: 7
Line nr: 8
Usage main <source_file>
Line nr: 9
Process 2989524 exited with status = 1 (0x00000001)
```

```console
(lldb) br s -n some_function -C "thread return 1" -G true
```

Alternative to adding print statements.
```console
(lldb) br s -f node_quic_util.h -l 461 -C 'frame info' -C 'expr *this' -C continue
```
Sometimes it can be needed to have print statements if the
issue/bug is timing dependent.

Running a shell command from within lldg:
```console
(lldb) platform shell find ../v8_src/v8 -name v8.cc
```

#### Adding aliases
```console
(lldb) command alias beve br s -f %1 -l %2 -C 'expr %4' -C continue
```

#### Print/echo something in command/script
```console
(lldb) script print "something"
```

### Saving breakpoints
```console
(lldb) br s -n something_function
(lldb) br write -f breakpoints
(lldb) br s -n something_function2
(lldb) br write -f breakpoints --append
```
Then these can be read using:
```console
(lldb) br read -f breakpoints
```
`breakpoints` will be a file in the current working directory.

