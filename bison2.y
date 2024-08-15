%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Linker.hpp"
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

%token GLOBAL EXTERN SECTION WORD SKIP END LSQB RSQB PLUS COMMA TWO_PUNKT COMMENT
%token HALT INT IRET CALL RET JMP ENDL
%token BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSR_RD CSR_WR

%token <literal> LITERAL MEM_LITERAL
%token <reg> REG_VALUE CSR
%token <symbol> SYMBOL_MEM SYMBOL 

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
    GLOBAL symbol_list { 
      Asembler::DirektivaGlobal(line_num);
      }
    | EXTERN symbol_list { 
      Asembler::DirektivaExtern(line_num);
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
    ;

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
    HALT { 
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

int main(int argc, char *argv[]) {
    const char* inputFile = nullptr;
    
    if (strcmp(argv[0], "./asembler") == 0){    // ako pokrecem asembler iz komandne linije



                  for (int i = 1; i < argc; ++i) {
                    if (strcmp(argv[i], "-o") == 0) {
                        if (i + 1 < argc) {
                            Asembler::outputBinaryName = argv[++i];
                            std::string s = std::string(Asembler::outputBinaryName);
                            Asembler::outputFileName = s.substr(0, s.size()-2) + ".txt";
                        } else {
                            std::cerr << "Option -o requires a filename." << std::endl;
                            return EXIT_FAILURE;
                        }
                    } else {
                        inputFile = argv[i];
                    }
                  }

                  if (!inputFile) {
                      std::cerr << "Both -o <output_file> and <input_file> must be specified." << std::endl;
                      return EXIT_FAILURE;
                  }

                
                  FILE* input = fopen(inputFile, "r");
                  if (!input) {
                      std::cerr << "I can't open " << inputFile << "!" << std::endl;
                      return EXIT_FAILURE;
                  }
                  yyin = input;

                  

                  
                  try {
                      yyparse();
                  } catch (const std::exception& e) {
                      std::cerr << e.what() << std::endl;
                      fclose(input);
                      return EXIT_FAILURE;
                  }

                  fclose(input);

                  return EXIT_SUCCESS;
    }



    if (strcmp(argv[0], "./linker") == 0){

                  for (int i = 1; i < argc; ++i) {
                    if (strstr(argv[i],".o")){
                      Linker::inputFilesNames.push_back(argv[i]);
                    }

                    if (strstr(argv[i], "-place=")){
                      Linker::postavljanjaString.push_back(argv[i]);
                    }

                    if (strstr(argv[i], "-o")){
                      Linker::outputFileName = argv[++i];
                    }

                  }

                  

        Linker::pokreniLinker();
    }

    
}

void yyerror(const char *s) {
    printf("EEK, parse error on line %d\n", line_num);
}