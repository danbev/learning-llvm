CXXFLAGS := "-g"
LD = /usr/bin/ld
AS = /usr/bin/as
CLANG = /usr/bin/clang
LLC = /usr/bin/llc
OPT = /usr/bin/opt

lang.o: lang.cc lang.h
	${CXX} ${CXXFLAGS} -o $@ -c $<

main: main.cc lang.o
	${CXX} ${CXXFLAGS} -o $@ lang.o $<

print.s: print.c
	${CLANG} -emit-llvm -c -S $<
	${LLC} print.ll
	${OPT} -S -mem2reg -instnamer print.ll -o print_before_opt.ll

print.o: print.s
	${AS} $< -o $@

print: print.o
	${LD} -e main -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
	       /lib64/crt1.o /lib64/crti.o -lc -arch x86_64 $@.o  \
	       /lib64/crtn.o -o $@ 

.PHONY: clean
clean: 
	@${RM} lang.o main print print.o print.s print.ll print_before_opt.ll
