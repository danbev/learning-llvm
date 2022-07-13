## llvm-libunwind
https://llvm.org/docs/ExceptionHandling.html


### Itanium C++ ABI: Exception Handling
The exception handling ABI has outlived the Itanium family of 64-bit Intel 
microprocessors that implemented the Intel Itanium architecture (AI-64) and is
used as a specification on lots of unix operating systems. It is used on arm,
x86, ppc, mips, ia64, aarch64, and others.


The exception handling module is divided into two parts, one is a language-
independant part, and the other is a language dependant part.

The language dependant part is responsible for dealing with an exception, search
for an exception handler, and for unwinding the stack before processing the
exception. So that is the part that is common, independant of the programming
language used. The language specific part is called a personality.


#### Process
The first things that happens is that an initial context must be created for
the current frame, well actually it would be for the previous frame as we would
be in some sort of "exception" handler when this code is run. The information
required to create this context is recorded by the compiler in static unwind
information which is stored in the text segment of the program. Each .text
segment contains a table of unwind information. A frame, think function frame
of the stack can have associated exception handling requirements. For example,
it might be that objects allocated in the frame need to be deacllocated or
other types of clean up. Or it might have one or more try regions which are
regions of code that specify an action (like the contents of a catch block) to
be taken if an exception occurs. In both of these cases the compiler will record
static unwind information for the function/frame and will store a reference to
the personality routine for this function/frame.

`___cxa_allocate_exception` will allocate an exception (TODO: what does this
struct look like?

Lets take a look at `eh_frame.cc` and what is generated for it. The following
basic block (named `5`) is generated for the throw instruction:
```llvm
5:                                                ; preds = %1
  %6 = call i8* @__cxa_allocate_exception(i64 4) #3
  %7 = bitcast i8* %6 to i32*
  store i32 2, i32* %7, align 16
  call void @__cxa_throw(i8* %6, i8* bitcast (i8** @_ZTIi to i8*), i8* null) #4
  unreachable
```

In `src/llvm-project/libcxxabi/include/cxxabi.h` we find the declaration of
`__cxa_allocate_exception`:
```c++
__cxa_allocate_exception(size_t thrown_size) throw();
```

And in `src/llvm-project/libcxxabi/src/cxa_exception.cpp` we have the impl:
```c++
void* __cxa_allocate_exception(size_t thrown_size) throw() {
    size_t actual_size = cxa_exception_size_from_exception_thrown_size(thrown_size);

    // Allocate extra space before the __cxa_exception header to ensure the
    // start of the thrown object is sufficiently aligned.
    size_t header_offset = get_cxa_exception_offset();
    char *raw_buffer = (char *)__aligned_malloc_with_fallback(header_offset + actual_size);       
    if (NULL == raw_buffer)
        std::terminate();
    __cxa_exception *exception_header =
        static_cast<__cxa_exception *>((void *)(raw_buffer + header_offset));   
    ::memset(exception_header, 0, actual_size);
    return thrown_object_from_cxa_exception(exception_header);
}

void __cxa_throw(void *thrown_object, std::type_info *tinfo, void (*dest)(void *)) {
    __cxa_eh_globals *globals = __cxa_get_globals();
    __cxa_exception* exception_header = cxa_exception_from_thrown_object(thrown_object);

    exception_header->unexpectedHandler = std::get_unexpected();
    exception_header->terminateHandler  = std::get_terminate();
    exception_header->exceptionType = tinfo;
    exception_header->exceptionDestructor = dest;
    setOurExceptionClass(&exception_header->unwindHeader);
    exception_header->referenceCount = 1;  // This is a newly allocated exception, no need for thread safety.
    globals->uncaughtExceptions += 1;   // Not atomically, since globals are thread-local

    exception_header->unwindHeader.exception_cleanup = exception_cleanup_func;

#if __has_feature(address_sanitizer)
    // Inform the ASan runtime that now might be a good time to clean stuff up.
    __asan_handle_no_return();
#endif

#ifdef __USING_SJLJ_EXCEPTIONS__
    _Unwind_SjLj_RaiseException(&exception_header->unwindHeader);
#else
    _Unwind_RaiseException(&exception_header->unwindHeader);
#endif
    //  This only happens when there is no handler, or some unexpected unwinding
    //     error happens.
    failed_throw(exception_header);
}
```
Notice that `__cxa_throw` calls `_Unwind_RaiseException` in 
`src/llvm-project/libunwind/src/Unwind-EHABI.cpp`:
```c++
_LIBUNWIND_EXPORT _Unwind_Reason_Code _Unwind_RaiseException(_Unwind_Exception *exception_object) {                   
  _LIBUNWIND_TRACE_API("_Unwind_RaiseException(ex_obj=%p)",                     
                       static_cast<void *>(exception_object));                  
  unw_context_t uc;                                                             
  unw_cursor_t cursor;                                                          
  __unw_getcontext(&uc);                                                        
                                                                                
  // This field for is for compatibility with GCC to say this isn't a forced    
  // unwind. EHABI #7.2                                                         
  exception_object->unwinder_cache.reserved1 = 0;                               
                                                                                
  // phase 1: the search phase                                                  
  _Unwind_Reason_Code phase1 = unwind_phase1(&uc, &cursor, exception_object);   
  if (phase1 != _URC_NO_REASON)                                                 
    return phase1;                                                              
                                                                                
  // phase 2: the clean up phase                                                
  return unwind_phase2(&uc, &cursor, exception_object, false);                  
}
```

This is a two stage process where the first stage is about trying to find the
correct handler.

```console
$ clang -S -emit-llvm -o eh_frame.ll eh_frame.cc
```

### Zero cost exceptions
Means that if no exceptions are thrown then there is no cost to the happy path.

### Structured Exception Handling (SEH)
Is Windows own exception handling.

__wip__

`___gxx_personality_v()` is libc++ personality routine

### Level 1. Base ABI

_Unwind_RaiseException,

_Unwind_Resume,

_Unwind_DeleteException,

_Unwind_GetGR (General Purpose Register)

_Unwind_SetGR (General Purpose Register),

_Unwind_GetIP (Instruction Pointer),

_Unwind_SetIP (Instruction Pointer),

_Unwind_GetRegionStart,

_Unwind_GetLanguageSpecificData,

_Unwind_ForcedUnwind


