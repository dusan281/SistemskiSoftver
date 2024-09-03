%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/Linker.hpp"
#include "inc/Emulator.hpp"
#include <stdexcept>
using namespace std;
#include <sstream>

  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
  extern int line_num;

  void yyerror(const char *s);

%}

%union {
    int literal;
    int reg;
    char *symbol;
}

%token GLOBAL EXTERN SECTION WORD ASCII NAVODNICI SKIP END LSQB RSQB PLUS COMMA TWO_PUNKT COMMENT LVB RVB WEAK
%token HALT INT IRET CALL RET JMP ENDL TYPE 
%token BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSR_RD CSR_WR

%token <literal> LITERAL MEM_LITERAL
%token <reg> REG_VALUE CSR
%token <symbol> SYMBOL_MEM SYMBOL FUNC DATA NOTYPE

%%

program:
    program line END {Asembler::DirektivaEnd();}
    | program line
    | line
    ;

line:
    directive COMMENT ENDLS
    | labela COMMENT ENDLS
    | labela directive COMMENT ENDLS 
    | labela instruction COMMENT ENDLS 
    | labela directive ENDLS 
    | labela instruction ENDLS 
    | labela ENDLS
    | instruction COMMENT ENDLS 
    | COMMENT ENDLS
    | instruction ENDLS
    | directive ENDLS
    ;

labela:
  SYMBOL_MEM TWO_PUNKT {Asembler::obradiLabelu($1, line_num);}

directive:

    TYPE SYMBOL_MEM tip {
      Asembler::DirektivaType($2, line_num);
    }

    | GLOBAL symbol_list { 
      Asembler::DirektivaGlobal(line_num);
      }
    | EXTERN symbol_list { 
      Asembler::DirektivaExtern(line_num);
      }

    | WEAK symbol_list {
      Asembler::DirektivaWeak(line_num);
    }

    | SECTION SYMBOL_MEM {
        Asembler::DirektivaSection($2, line_num);
      }
    | WORD symbol_or_literal_list {
        Asembler::DirektivaWord(line_num);
      }
    
    | SKIP MEM_LITERAL {
      Asembler::DirektivaSkip($2, line_num);
    }

    | ASCII NAVODNICI SYMBOL_MEM NAVODNICI {
      Asembler::DirektivaAscii($3, line_num);
    }
    ;

tip:
    FUNC { Asembler::dodajUListuArgumenata(new Argument(Operand::simbol, $1, 0));}
    | DATA { Asembler::dodajUListuArgumenata(new Argument(Operand::simbol, $1, 0));}
    | NOTYPE { Asembler::dodajUListuArgumenata(new Argument(Operand::simbol, $1, 0));}


symbol_list:
    symbol_list COMMA SYMBOL_MEM { 
      Asembler::dodajUListuArgumenata(new Argument(Operand::simbol, $3, 0));
     }
    | SYMBOL_MEM { 
      Asembler::dodajUListuArgumenata(new Argument(Operand::simbol, $1, 0));
    }
    ;

symbol_or_literal_list:
    symbol_or_literal_list COMMA MEM_LITERAL {
       Argument* arg = new Argument(Operand::literal,"", $3);
       Asembler::dodajUListuArgumenata(arg);
      }
    | symbol_or_literal_list COMMA SYMBOL_MEM { 
       Argument* arg = new Argument(Operand::simbol,$3, 0);
       Asembler::dodajUListuArgumenata(arg);
      }
    | MEM_LITERAL {
       Argument* arg = new Argument(Operand::literal,"", $1);
       Asembler::dodajUListuArgumenata(arg);
        }
    | SYMBOL_MEM {
       Argument* arg = new Argument(Operand::simbol,$1, 0);
       Asembler::dodajUListuArgumenata(arg);
      }
    ;

instruction:

    PUSH LVB registri RVB {
      Asembler::napraviInstrukciju(Token_Instrukcija::push_mod, line_num);
    }
    
    | HALT { 
      Asembler::napraviInstrukciju(Token_Instrukcija::halt,line_num);
      }

    | INT { 
      Asembler::napraviInstrukciju(Token_Instrukcija::Int, line_num);
     }

    | IRET { 
      Asembler::napraviInstrukciju(Token_Instrukcija::iret, line_num);
      }

    | CALL operand_skok { 
      Asembler::napraviInstrukciju(Token_Instrukcija::call, line_num);
      }

    | RET { 
      Asembler::napraviInstrukciju(Token_Instrukcija::ret, line_num);
    }
    | JMP operand_skok { 
      Asembler::napraviInstrukciju(Token_Instrukcija::jmp, line_num);
    }

    | BEQ REG_VALUE COMMA REG_VALUE COMMA operand_skok { 
      Asembler::napraviInstrukciju(Token_Instrukcija::beq, line_num, $2, $4);
      }

    | BNE REG_VALUE COMMA REG_VALUE COMMA operand_skok { 
      Asembler::napraviInstrukciju(Token_Instrukcija::bne, line_num, $2, $4);
      }

    | BGT REG_VALUE COMMA REG_VALUE COMMA operand_skok { 
      Asembler::napraviInstrukciju(Token_Instrukcija::bgt, line_num, $2, $4);
      }

    | PUSH REG_VALUE { 
        Asembler::napraviInstrukciju(Token_Instrukcija::push, line_num, $2);
      }
    | POP REG_VALUE { 
        Asembler::napraviInstrukciju(Token_Instrukcija::pop, line_num, $2);
      }

    | XCHG REG_VALUE COMMA REG_VALUE { 
        Asembler::napraviInstrukciju(Token_Instrukcija::xchg, line_num, $2, $4);
      }
    | ADD REG_VALUE COMMA REG_VALUE { 
        Asembler::napraviInstrukciju(Token_Instrukcija::add, line_num, $2, $4);
      }
    
    | ADD REG_VALUE COMMA REG_VALUE COMMA SHL MEM_LITERAL {
        Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $7));
        Asembler::napraviInstrukciju(Token_Instrukcija::add_shl, line_num, $2, $4);
    }

    | ADD REG_VALUE COMMA REG_VALUE COMMA SHR MEM_LITERAL {
        Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $7));
        Asembler::napraviInstrukciju(Token_Instrukcija::add_shr, line_num, $2, $4);
    }

    | SUB REG_VALUE COMMA REG_VALUE COMMA SHL MEM_LITERAL {
        Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $7));
        Asembler::napraviInstrukciju(Token_Instrukcija::sub_shl, line_num, $2, $4);
    }

    | SUB REG_VALUE COMMA REG_VALUE COMMA SHR MEM_LITERAL {
        Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $7));
        Asembler::napraviInstrukciju(Token_Instrukcija::sub_shr, line_num, $2, $4);
    }

    | SUB REG_VALUE COMMA REG_VALUE { 
        Asembler::napraviInstrukciju(Token_Instrukcija::sub, line_num, $2, $4);
        }
    | MUL REG_VALUE COMMA REG_VALUE { 

        Asembler::napraviInstrukciju(Token_Instrukcija::mul, line_num, $2, $4);
      }
    | DIV REG_VALUE COMMA REG_VALUE { 

        Asembler::napraviInstrukciju(Token_Instrukcija::Div, line_num, $2, $4);
      }
    | NOT REG_VALUE { 
  
        Asembler::napraviInstrukciju(Token_Instrukcija::Not, line_num, $2);
      }
    | AND REG_VALUE COMMA REG_VALUE { 
     
        Asembler::napraviInstrukciju(Token_Instrukcija::And, line_num, $2, $4);
      }
    | OR REG_VALUE COMMA REG_VALUE { 
   
        Asembler::napraviInstrukciju(Token_Instrukcija::Or, line_num, $2, $4);
      }
    | XOR REG_VALUE COMMA REG_VALUE { 
  
        Asembler::napraviInstrukciju(Token_Instrukcija::Xor, line_num, $2, $4);
      }
    | SHL REG_VALUE COMMA REG_VALUE { 
        Asembler::napraviInstrukciju(Token_Instrukcija::shl, line_num, $2, $4);

      }
    | SHR REG_VALUE COMMA REG_VALUE { 
        Asembler::napraviInstrukciju(Token_Instrukcija::shr, line_num, $2, $4);
  
      }
    | LD operand_instrukcija COMMA REG_VALUE { Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $4)); Asembler::napraviInstrukciju(Token_Instrukcija::ld, line_num); } 
    
    | ST REG_VALUE COMMA operand_instrukcija { Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $2),0); Asembler::napraviInstrukciju(Token_Instrukcija::st, line_num); }

    | CSR_RD CSR COMMA REG_VALUE { 
      Asembler::napraviInstrukciju(Token_Instrukcija::csrrd, line_num, $4, -1,-1, $2);
      }

    | CSR_WR REG_VALUE COMMA CSR { 
      Asembler::napraviInstrukciju(Token_Instrukcija::csrwr, line_num, $2, -1, -1, $4);
      }
    ;

registri:
  REG_VALUE { Asembler::dodajUListuArgumenata(new Argument(Operand::literal,"",$1));}
  | registri COMMA REG_VALUE { Asembler::dodajUListuArgumenata(new Argument(Operand::literal,"",$3));}

operand_skok:
    MEM_LITERAL { Asembler::Asembler::postaviAdresiranje(Adresiranje::SKOK); Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $1));}
    | SYMBOL_MEM { Asembler::Asembler::postaviAdresiranje(Adresiranje::SKOK); Asembler::dodajUListuArgumenata(new Argument(Operand::simbol, $1, 0));}


operand_instrukcija:
    LITERAL { Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $1)); Asembler::Asembler::postaviAdresiranje(Adresiranje::IMMED); }
    | SYMBOL  { Asembler::dodajUListuArgumenata(new Argument(Operand::simbol, $1, 0)); Asembler::postaviAdresiranje(Adresiranje::IMMED); }
    | MEM_LITERAL { Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $1)); Asembler::postaviAdresiranje(Adresiranje::MEM_DIR); }
    | SYMBOL_MEM { Asembler::dodajUListuArgumenata(new Argument(Operand::simbol, $1, 0)); Asembler::postaviAdresiranje(Adresiranje::MEM_DIR);;}
    | REG_VALUE { Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $1)); Asembler::postaviAdresiranje(Adresiranje::REG_DIR);; }
    | LSQB REG_VALUE RSQB { Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $2)); Asembler::postaviAdresiranje(Adresiranje::REG_IND);; }
    | LSQB REG_VALUE PLUS MEM_LITERAL RSQB { Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $2)); Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $4));Asembler::postaviAdresiranje(Adresiranje::REG_IND_POM); }
    | LSQB REG_VALUE PLUS REG_VALUE RSQB { Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $2)); Asembler::dodajUListuArgumenata(new Argument(Operand::literal, "", $4)); Asembler::postaviAdresiranje(Adresiranje::REG_REG_IND); }


ENDLS:
    ENDLS ENDL
    | ENDL


;
%%

#include <iostream>
#include <fstream>

extern "C" {
    FILE* outputFile;  // Izlazni tok
}



void yyerror(const char *s) {
    printf("EEK, parse error on line %d\n", line_num);
}