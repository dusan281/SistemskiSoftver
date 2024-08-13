#include "Asembler.hpp"



struct SekcijaLinker{

  Sekcija sekcijaAsembler;
  int brojTabeleSimbola;

  SekcijaLinker(Sekcija sekcijaAsembler, int brojTabeleSimbola = 0){
    this->brojTabeleSimbola = brojTabeleSimbola;
    this->sekcijaAsembler = sekcijaAsembler;
  }
};



class Linker{

public:

  static std::vector<SekcijaLinker> sveSekcijeLinker;
  static std::vector<std::vector<TabelaSimbolaUlaz>> tabeleSimbolaAsembler;
  static std::vector<TabelaSimbolaUlaz> tabelaSimbolaLinker;

  static std::vector<char*> inputFilesNames;
  static std::ifstream inputFile;


  static void pokreniLinker();
  static void procitajFajl(std::ifstream& stream);

};