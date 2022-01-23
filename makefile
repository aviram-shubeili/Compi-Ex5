.PHONY: all clean

all: clean
	flex scanner.lex
	bison -Wcounterexamples -d parser.ypp
	g++ -std=c++17 -o hw5 *.c *.cpp

create:
	./hw5 < hw5-tests/t9.in > hw5-tests/t9.llvm

run:
	lli hw5-tests/t9.llvm > hw5-tests/t9.res

createAndrun:
	./hw5 < hw5-tests/t2.in > hw5-tests/t2.llvm
	lli hw5-tests/t2.llvm > hw5-tests/t2.res
clean:
	rm -f lex.yy.c
	rm -f parser.tab.*pp
	rm -f hw5
