#ifndef _ASEMBLER_HPP
#define _ASEMBLER_HPP

#include <iostream>
#include <iomanip>
#include <fstream>
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
  REG_IND_POM,
  LD_IMMED,
  STORE_MEM_DIR,
  SKOK,
  POZIV_POTPROGRAM
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
  std::string simbol;

  RelokacioniZapisUlaz(int offset, std::string tip, int simbolRB, int dodatak, std::string simbol) : offset{offset}, tip{tip}, simbolRB{simbolRB}, dodatak{dodatak}, simbol{simbol} {}


  bool operator==(const int& uporedi) const { // nalazim sekciju iz tabele simbola
    return offset == uporedi;
  }

};

struct Sekcija{
  int LC;
  int brSekcije;
  std::string imeSekcije;
  uint32_t startnaAdresa;
  std::vector<std::uint8_t> kod_sekcije;
  std::vector<RelokacioniZapisUlaz> relokacioni_zapis;
  std::map<int, Argument*> promenljive_bazen; // cuvam offset od pocetka sekcije i vrednost

  Sekcija(int brSekcije, std::string imeSekcije){
    this->LC = 0;
    this->startnaAdresa = 0;
    this->brSekcije = brSekcije;
    this->imeSekcije = imeSekcije;
    this->kod_sekcije = {};
    this->relokacioni_zapis = {};
    this->promenljive_bazen = {};
  }

  Sekcija(){
    this->LC = 0;
    this->startnaAdresa = 0;
    this->brSekcije = brSekcije;
    this->imeSekcije = imeSekcije;
    this->kod_sekcije = {};
    this->relokacioni_zapis = {};
    this->promenljive_bazen = {};
  }
};

struct BackpatchUlaz{
  int sekcija;
  int offset;
  std::string instrukcija;

  BackpatchUlaz(int sekcija, int offset, std::string instrukcija){
    this->instrukcija = instrukcija;
    this->offset = offset;
    this->sekcija = sekcija;
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
  std::vector<BackpatchUlaz> backpatch;

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

  static std::fstream outputFile; 
  static std::fstream outputFileBinary;
  static std::vector<Argument*> argumenti;
  static std::map<int,Sekcija*> sveSekcije;
  static Sekcija* trSekcija;
  static Adresiranje adresiranje;


  static char* outputFileName;
  static char* outputBinaryName;



  static std::vector<TabelaSimbolaUlaz> tabelaSimbola;


  static void dodajUListuArgumenata(Argument* arg,int pozicija = -1);

  static void ispisiListuSimbola();
  static void ispisiListuSimbolaIliLiterala();
  static void ispisiKodSekcije(Sekcija* tr);
  static void zavrsiSekciju();
  static void ispisiGresku(int& line_num);


  static void literalVeliki(int gprA, int gprB, int gprC, int argument);
  static void literalMali(int gprA, int gprB, int gprC, int argument);


  static std::vector<TabelaSimbolaUlaz>::iterator dodajSimbolUTabeluSimbola(std::string);

  static int dodajUBazenLiterala(int& vrednost);
  static int dodajUBazenLiteralaSimbol(std::string simbol);
  static int simbolNedefinisan(std::string simbol, int gprA, int gprB, int gprC);
  static void simbolDefinisan(int gprA, int gprB, int gprC, std::vector<TabelaSimbolaUlaz>::iterator it);
  
  static bool izbrisiRelokacioniZapis(RelokacioniZapisUlaz zapis);
  static bool izbrisiSimbolIzTabeleSimbola(TabelaSimbolaUlaz ulaz);

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


  static void LD_REG_DIR(int gprA, int gprB);
  static void LD_MEM_DIR();
  static void resiSimbolLiteral(int gprA, int gprB, int gprC, Argument* arg);
  static void LD_REG_IND(int gprA, int gprB);
  static void LD_REG_IND_POM(int& line_num);

  static void ST_REG_DIR(int& gprA);
  static void ST_MEM_DIR(int& gprA);
  static void ST_REG_IND(int gprA, int gprC);
  static void ST_REG_IND_POM(int gprA, int gprB, int gprC, int& line_num);



  static void InstrukcijaHalt();
  static void InstrukcijaInt();
  static void InstrukcijaIret();
  static void InstrukcijaRet();
  
  static void InstrukcijaCall(int& line_num);
  static void InstrukcijaJmp(int& line_num);
  static void InstrukcijaBeq(int gpr1, int gpr2);
  static void InstrukcijaBne(int gpr1, int gpr2);
  static void InstrukcijaBgt(int gpr1, int gor2);

  static void InstrukcijaSkoka(int gpr1, int gpr2);



  static void InstrukcijaLd(int& line_num);
  static void InstrukcijaSt(int& line_num);


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


  static void napraviInstrukciju(Token_Instrukcija t, int brLinije, int gpr1 = -1, int gpr2 = -1, int gpr3 = -1, int csr = -1);

  static void relokacijaInstrukcije(int pomeraj, std::vector<TabelaSimbolaUlaz>::iterator it);

  static void relokacijaSkok(int pomeraj, std::vector<TabelaSimbolaUlaz>::iterator it, int instrukcija);

  static void ispisiIzlazneFajlove();

};

#endif 