#include "Asembler.hpp"

class Linker{

public:

  static int brojacSekcija;

  static std::vector<Sekcija> sveSekcijeLinker;
  static std::vector<TabelaSimbolaUlaz> tabelaSimbolaLinker;

  static std::vector<char*> inputFilesNames;
  static std::ifstream inputFile;


  static void pokreniLinker();
  static void procitajFajl(std::ifstream& stream);

};