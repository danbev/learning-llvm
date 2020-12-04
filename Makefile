CXXFLAGS := "-g"

lang.o: lang.cc lang.h
	${CXX} ${CXXFLAGS} -o $@ -c $<

main: main.cc lang.o
	${CXX} ${CXXFLAGS} -o $@ lang.o $<

.PHONY: clean
clean: 
	@${RM} lang.o main
