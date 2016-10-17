###############################################################################
# purpose: Assignment 3 example recursive descent parser
#  author: J. Edward Swan II
#  editor: Josh Hawkins
###############################################################################
LEX		=	flex
.PRECIOUS 	=	parser.cpp rules.l lexer.h 

parse.exe: lex.yy.o parser.o
	g++ -o $@ $^

lex.yy.o: lex.yy.c lexer.h
	gcc -o $@ -c lex.yy.c

parser.o: parser.cpp lexer.h
	g++ -o $@ -c parser.cpp

lex.yy.c: rules.l
	$(LEX) $^

clean: 
	$(RM) *.o lex.yy.c parse.exe

