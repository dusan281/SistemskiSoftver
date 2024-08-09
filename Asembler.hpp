#ifndef _ASEMBLER_HPP
#define _ASEMBLER_HPP

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <sstream>

enum Adresiranje{
  REG_DIR,
  REG_IND,
  MEM_DIR,
  MEM_IND,
  IMMED,
  REG_IND_POM
};



enum Token_Instrukcija{
  halt = 0,
  Int,
  iret,
  call,
  ret,
  jmp,
  beq,
  bne,
  bgt,
  push,
  pop,
  xchg,
  add,
  sub,
  mul,
  Div,
  Not,
  And,
  Or,
  Xor,
  shl,
  shr,
  ld,
  st,
  csrrd,
  csrwr
};

enum Operand{
  literal = 0,
  simbol,
  gprX,
  csrX
};



struct Argument{
  Operand e;
  std::string simbol;
  int vrednost;

  Argument(Operand e,std::string simbol, int vrednost){
    this->e = e;
    this->simbol = simbol;
    this->vrednost = vrednost;
  }

  bool operator==(const Argument& uporedi) const { // nalazim sekciju iz tabele simbola
    return vrednost == uporedi.vrednost || simbol == uporedi.simbol;
    std::cout<< vrednost << " " << uporedi.vrednost << std::endl;
  }

  bool operator==(std::string uporedi){
    return simbol == uporedi;
  }
};






struct RelokacioniZapisUlaz{
  int offset;
  std::string tip;
  int simbolRB;
  int dodatak;

  RelokacioniZapisUlaz(int offset, std::string tip, int simbolRB, int dodatak) : offset{offset}, tip{tip}, simbolRB{simbolRB}, dodatak{dodatak} {}


  bool operator==(const int& uporedi) const { // nalazim sekciju iz tabele simbola
    return simbolRB == uporedi;
  }

};



struct Sekcija{
  int LC;
  int brSekcije;
  std::vector<std::uint8_t> kod_sekcije;
  std::vector<RelokacioniZapisUlaz> relokacioni_zapis;
  std::map<int, Argument*> promenljive_bazen; // cuvam offset od pocetka sekcije i vrednost

  Sekcija(int brSekcije){
    this->LC = 0;
    this->brSekcije = brSekcije;
    this->kod_sekcije = {};
    this->relokacioni_zapis = {};
    this->promenljive_bazen = {};
  }
};



struct TabelaSimbolaUlaz{
  std::string simbol;
  int brSekcije;
  int vrednost;
  std::string tip;
  std::string vezivanje;
  int redniBroj;
  int velicina;
  std::vector<std::pair<int,int>> backpatch;

  TabelaSimbolaUlaz(std::string simbol, int brSekcije, int redniBroj, int vrednost = 0, std::string tip = "NOTYP", std::string vezivanje = "LOC", int velicina = 0){
    this->simbol = simbol;
    this->brSekcije = brSekcije;
    this->vrednost = vrednost;
    this->tip = tip;
    this->vezivanje = vezivanje;
    this->redniBroj = redniBroj;
    this->velicina = velicina;
    this->backpatch = {};
  }

  bool operator==(const std::string& uporedi) const { // nalazim ulaz iz tabele simbola po imenu
    return simbol == uporedi;
  }

  bool operator==(const int& uporedi) const { // nalazim sekciju iz tabele simbola
    return brSekcije == uporedi && redniBroj == uporedi;
  }
};






class Asembler{

public:

  static std::vector<Argument*> argumenti;
  static std::map<int,Sekcija*> sveSekcije;
  static Sekcija* trSekcija;
  static Adresiranje adresiranje;


  static std::vector<TabelaSimbolaUlaz> tabelaSimbola;


  static void dodajUListuArgumenata(Argument* arg);

  static void ispisiListuSimbola();
  static void ispisiListuSimbolaIliLiterala();
  static void ispisiKodSekcije(Sekcija* tr);
  static void zavrsiSekciju();
  static void ispisiGresku(int& line_num);


  static void literalVelikiSkok(int gpr2, int& argument);
  static void literalMaliSkok(int& gpr2, int& argument);


  static void literalVelikiInstrukcija(int gpr2, int& argument);
  static void literalMaliInstrukcija(int gpr2, int& argument);


  static int dodajUBazenLiterala(int& vrednost);
  static int dodajUBazenLiteralaSimbol(std::string simbol,std::vector<TabelaSimbolaUlaz>::iterator it);
  static void simbolNedefinisan(std::string simbol, int gprA, int gprB, int gprC, std::vector<TabelaSimbolaUlaz>::iterator it);
  static void simbolDefinisan(std::string simbol, int gprA, int gprB, int gprC, std::vector<TabelaSimbolaUlaz>::iterator it);
  
  static void srediLokalneSimbole();


  static void dodajPodatakUKodSekcije(int& vrednost);
  static void zavrsiInstrukciju();  
  static void postaviAdresiranje(enum Adresiranje a);


  static void DirektivaSection(std::string simbol, int& line_num);
  static void DirektivaEnd();
  static void DirektivaGlobal(int& line_num);
  static void DirektivaSkip(int& vrednost, int& line_num);
  static void DirektivaExtern(int& line_num);
  static void DirektivaWord(int& line_num);
  static void obradiLabelu(std::string simbol, int& line_num);



  static void InstrukcijaHalt();
  static void InstrukcijaInt();
  static void InstrukcijaIret();
  static void InstrukcijaRet();
  
  static void InstrukcijaCall(int& line_num);
  static void InstrukcijaJmp(int& line_num);
  static void InstrukcijaBeq(int gpr1, int gpr2);
  static void InstrukcijaBne(int gpr1, int gpr2);
  static void InstrukcijaBgt(int gpr1, int gor2);

  static void InstrukcijaSkoka(int a, int gpr1, int gpr2);



  static void InstrukcijaLd(int& line_num);
  static void InstrukcijaSt();


  static void InstrukcijaAdd(int gpr1, int gpr2);
  static void InstrukcijaSub(int gpr1, int gpr2);
  static void InstrukcijaMul(int gpr1, int gpr2);
  static void InstrukcijaDiv(int gpr1, int gpr2);
  static void InstrukcijaNot(int gpr1);
  static void InstrukcijaAnd(int gpr1, int gpr2);
  static void InstrukcijaOr(int gpr1, int gpr2);
  static void InstrukcijaXor(int gpr1, int gpr2);
  static void InstrukcijaXchg(int gpr1, int gpr2);
  static void InstrukcijaShl(int gpr1, int gpr2);
  static void InstrukcijaShr(int gpr1, int gpr2);

  static void InstrukcijaCsrrd(int gpr1, int csr);
  static void InstrukcijaCsrwr(int gpr1, int csr);


  static void InstrukcijaPush(int gpr1);
  static void InstrukcijaPop(int gpr1);


  static void napraviInstrukciju(Token_Instrukcija t, int brLinije, int gpr1 = -1, int gpr2 = -1, int csr = -1);


};

#endif 