%{
/* Definitions section */
#include "source.hpp"
#include "parser.tab.hpp"
#include "hw3_output.hpp"
%}

%option yylineno
%option noyywrap
whitespace      ([\t\n\r ])
relational           (<|>|<=|>=)
equality           (==|!=)
muldiv_binop    (\*|\/)
addsub_binop    (\+|\-)


%%
 /* Rules section*/
void                                                       { yylval = new Node(yylineno) ;return VOID;                                  }
int                                                        { yylval = new Node(yylineno); return INT;                                   }
byte                                                       { yylval = new Node(yylineno); return BYTE;                                  }
b                                                          { yylval = new Node(yylineno); return B;                                     }
bool                                                       { yylval = new Node(yylineno); return BOOL;                                  }
const                                                      { yylval = new Node(yylineno); return CONST;                                 }
and                                                        { yylval = new Node(yylineno); return AND;                                   }
or                                                         { yylval = new Node(yylineno); return OR;                                    }
not                                                        { yylval = new Node(yylineno); return NOT;                                   }
true                                                       { yylval = new Node(yylineno); return TRUE;                                  }
false                                                      { yylval = new Node(yylineno); return FALSE;                                 }
return                                                     { yylval = new Node(yylineno); return RETURN;                                }
if                                                         { yylval = new Node(yylineno); return IF;                                    }
else                                                       { yylval = new Node(yylineno); return ELSE;                                  }
while                                                      { yylval = new Node(yylineno); return WHILE;                                 }
break                                                      { yylval = new Node(yylineno); return BREAK;                                 }
continue                                                   { yylval = new Node(yylineno); return CONTINUE;                              }
;                                                          { yylval = new Node(yylineno); return SC;                                    }
,                                                          { yylval = new Node(yylineno); return COMMA;                                 }
\(                                                         { yylval = new Node(yylineno); return LPAREN;                                }
\)                                                         { yylval = new Node(yylineno); return RPAREN;                                }
\{                                                         { yylval = new Node(yylineno); return LBRACE;                                }
\}                                                         { yylval = new Node(yylineno); return RBRACE;                                }
=                                                          { yylval = new Node(yylineno); return ASSIGN;                                }
{relational}                                               { yylval = new Relop(yytext, yylineno); return RELATIONAL;                   }
{equality}                                                 { yylval = new Relop(yytext, yylineno); return EQUALITY;                     }
{muldiv_binop}                                             { yylval = new Binop(yytext, yylineno); return MULDIV_BINOP;                 }
{addsub_binop}                                             { yylval = new Binop(yytext, yylineno); return ADDSUB_BINOP;                 }
[a-zA-Z][a-zA-Z0-9]*                                       { yylval = new IdNode(yytext, yylineno); return ID;                              }
0|[1-9][0-9]*                                              { yylval = new NumNode(yytext, yylineno); return NUM;                            }
\"([^\n\r\"\\]|\\[rnt\"\\])+\"                             { yylval = new StringNode(yytext, yylineno); return STRING;                      }
{whitespace}|(\/\/[^\r\n]*[ \r|\n|\r\n]?)                  {                                                                            }
.                                                          { output::errorLex(yylineno); exit(0);                                       }
