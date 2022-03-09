/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOK_VOID = 258,
    TOK_INT = 259,
    TOK_INTVAL = 260,
    TOK_BOOL = 261,
    TOK_TRUE = 262,
    TOK_FALSE = 263,
    TOK_IDENT = 264,
    TOK_IF = 265,
    TOK_ELSE = 266,
    TOK_WHILE = 267,
    TOK_FOR = 268,
    TOK_PRINT = 269,
    TOK_SEMICOL = 270,
    TOK_COMMA = 271,
    TOK_LPAR = 272,
    TOK_RPAR = 273,
    TOK_LACC = 274,
    TOK_RACC = 275,
    TOK_STRING = 276,
    TOK_DO = 277,
    TOK_THEN = 278,
    TOK_AFFECT = 279,
    TOK_OR = 280,
    TOK_AND = 281,
    TOK_BOR = 282,
    TOK_BXOR = 283,
    TOK_BAND = 284,
    TOK_EQ = 285,
    TOK_NE = 286,
    TOK_GT = 287,
    TOK_LT = 288,
    TOK_GE = 289,
    TOK_LE = 290,
    TOK_SRL = 291,
    TOK_SRA = 292,
    TOK_SLL = 293,
    TOK_PLUS = 294,
    TOK_MINUS = 295,
    TOK_MUL = 296,
    TOK_DIV = 297,
    TOK_MOD = 298,
    TOK_UMINUS = 299,
    TOK_NOT = 300,
    TOK_BNOT = 301
  };
#endif
/* Tokens.  */
#define TOK_VOID 258
#define TOK_INT 259
#define TOK_INTVAL 260
#define TOK_BOOL 261
#define TOK_TRUE 262
#define TOK_FALSE 263
#define TOK_IDENT 264
#define TOK_IF 265
#define TOK_ELSE 266
#define TOK_WHILE 267
#define TOK_FOR 268
#define TOK_PRINT 269
#define TOK_SEMICOL 270
#define TOK_COMMA 271
#define TOK_LPAR 272
#define TOK_RPAR 273
#define TOK_LACC 274
#define TOK_RACC 275
#define TOK_STRING 276
#define TOK_DO 277
#define TOK_THEN 278
#define TOK_AFFECT 279
#define TOK_OR 280
#define TOK_AND 281
#define TOK_BOR 282
#define TOK_BXOR 283
#define TOK_BAND 284
#define TOK_EQ 285
#define TOK_NE 286
#define TOK_GT 287
#define TOK_LT 288
#define TOK_GE 289
#define TOK_LE 290
#define TOK_SRL 291
#define TOK_SRA 292
#define TOK_SLL 293
#define TOK_PLUS 294
#define TOK_MINUS 295
#define TOK_MUL 296
#define TOK_DIV 297
#define TOK_MOD 298
#define TOK_UMINUS 299
#define TOK_NOT 300
#define TOK_BNOT 301

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 37 "grammar.y"

    int32_t intval;
    char * strval;
    node_t ptr;

#line 155 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (node_t * program_root);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
