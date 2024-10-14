/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_LANGUAGE_TAB_H_INCLUDED
# define YY_YY_LANGUAGE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    t_STRING_CMD = 258,            /* t_STRING_CMD  */
    t_SET_CMD = 259,               /* t_SET_CMD  */
    t_COLLECTION_CMD = 260,        /* t_COLLECTION_CMD  */
    t_IF_CMD = 261,                /* t_IF_CMD  */
    t_ELSE_CMD = 262,              /* t_ELSE_CMD  */
    t_WHILE_CMD = 263,             /* t_WHILE_CMD  */
    t_FOR_CMD = 264,               /* t_FOR_CMD  */
    t_INTEGER_CMD = 265,           /* t_INTEGER_CMD  */
    t_OUTPUT_CMD = 266,            /* t_OUTPUT_CMD  */
    t_INPUT_CMD = 267,             /* t_INPUT_CMD  */
    GREATER = 268,                 /* GREATER  */
    LESS = 269,                    /* LESS  */
    EQUAL = 270,                   /* EQUAL  */
    GREATER_EQUAL = 271,           /* GREATER_EQUAL  */
    LESS_EQUAL = 272,              /* LESS_EQUAL  */
    NOT = 273,                     /* NOT  */
    t_STRING = 274,                /* t_STRING  */
    t_ID = 275,                    /* t_ID  */
    t_INTEGER = 276,               /* t_INTEGER  */
    PLUS = 277,                    /* PLUS  */
    MINUS = 278,                   /* MINUS  */
    MULTIPLY = 279,                /* MULTIPLY  */
    DIVIDE = 280,                  /* DIVIDE  */
    AND = 281,                     /* AND  */
    OR = 282                       /* OR  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 1399 "Language.y"

    char* str;
    int num;

#line 96 "Language.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_LANGUAGE_TAB_H_INCLUDED  */
