###############################################################################
# purpose: makefile for ch4 example recursive descent parser
#  author: J. Edward Swan II
###############################################################################
LEX		=	flex
.PRECIOUS 	=	ch4-parser.cpp exp-rules.l lexer.h 

parse.exe: lex.yy.o ch4-parser.o
	g++ -o $@ $^

lex.yy.o: lex.yy.c lexer.h
	cc -o $@ -c lex.yy.c

ch4-parser.o: ch4-parser.cpp lexer.h
	g++ -o $@ -c ch4-parser.cpp

lex.yy.c: exp-rules.l
	$(LEX) $^

clean: 
	$(RM) *.o lex.yy.c parse.exe

