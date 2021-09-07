### Enabling LTO for Node.js
```console
$ configure --enable-lto
$ make -j8
```
This lead to the following error:
```console
lto1: internal compiler error: in add_symbol_to_partition_1, at lto/lto-partition.c:153
Please submit a full bug report,
with preprocessed source if appropriate.
See <http://bugzilla.redhat.com/bugzilla> for instructions.
lto-wrapper: fatal error: /usr/bin/g++ returned 1 exit status
compilation terminated.
/usr/bin/ld: error: lto-wrapper failed
collect2: error: ld returned 1 exit status
make[1]: *** [tools/v8_gypfiles/mksnapshot.target.mk:196: /home/danielbevenius/work/nodejs/node/out/Release/mksnapshot] Error 1
rm 8d70f465b7bec82f66257d3ce37f71076dbd3fbd.intermediate
make: *** [Makefile:110: node] Error 2
```

Upgrading to GCC 10 will take care of the above issue. But I still see a
compilation error when enabling LTO in V8:
```console
/usr/bin/ld: /tmp/mksnapshot.cXaTPr.ltrans124.ltrans.o: in function `v8::internal::compiler::InliningPhase::Run(v8::internal::compiler::PipelineData*, v8::internal::Zone*) [clone .constprop.0]':
<artificial>:(.text+0x18103): undefined reference to `vtable for v8::internal::compiler::JSCallReducer'
/usr/bin/ld: <artificial>:(.text+0x185c3): undefined reference to `vtable for v8::internal::compiler::JSCallReducer'
collect2: error: ld returned 1 exit status
make[1]: *** [tools/v8_gypfiles/mksnapshot.target.mk:196: /home/danielbevenius/work/nodejs/node/out/Release/mksnapshot] Error 1
rm 8d70f465b7bec82f66257d3ce37f71076dbd3fbd.intermediate
make: *** [Makefile:110: node] Error 2
```

If we look in deps/v8/src/compiler/js-call-reducer.h:
```c++
class V8_EXPORT_PRIVATE JSCallReducer final : public AdvancedReducer {             
   public:                                                                           
    // Flags that control the mode of operation.                                     
    enum Flag {                                                                      
      kNoFlags = 0u,                                                                 
      kBailoutOnUninitialized = 1u << 0,                                             
      kInlineJSToWasmCalls = 1u << 1,                                                
    };                                                                               
    using Flags = base::Flags<Flag>;                                                 
                                                                                     
    JSCallReducer(Editor* editor, JSGraph* jsgraph, JSHeapBroker* broker,            
                  Zone* temp_zone, Flags flags,                                      
                  CompilationDependencies* dependencies)                             
        : AdvancedReducer(editor),                                                   
          jsgraph_(jsgraph),                                                         
          broker_(broker),                                                           
          temp_zone_(temp_zone),                                                     
          flags_(flags),                                                             
          dependencies_(dependencies) {}                                             
```
AdvancedReducer has a virtual destructor (=default).
```c++
    ~JSCallReducer() {};          
```

https://github.com/nodejs/node/issues/38335

https://github.com/nodejs/node/pull/38346
