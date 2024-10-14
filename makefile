parse.exe: lex.yy.c Language.tab.c TestYacc.c
	gcc lex.yy.c Language.tab.c TestYacc.c -o parse.exe

lex.yy.c: Language.tab.c Language.l
	flex Language.l

Language.tab.c: Language.y
	bison -d Language.y
	
clean: 
	del lex.yy.c Language.tab.c Language.tab.h parse.exe test.exe

test.exe: testRes.cpp RuntimeFunctions.hpp
	g++ -std=c++17 testRes.cpp RuntimeFunctions.hpp -o test.exe

test: test.exe
	./test.exe
