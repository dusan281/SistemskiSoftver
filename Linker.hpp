#include "Asembler.hpp"

class Linker{

public:

  static int brojacSekcija;
  static int brojacSimbola;

  static std::vector<Sekcija> sveSekcijeLinker;
  static std::vector<TabelaSimbolaUlaz> tabelaSimbolaLinker;

  static std::vector<char*> inputFilesNames;
  static char* outputFileName;

  static std::vector<char*> postavljanjaString;
  static void postaviAdreseSekcija();
  static bool sortirajSekcijePoAdresi(Sekcija a, Sekcija b);

  static std::ifstream inputFile;


  static void pokreniLinker();
  static void procitajFajl(std::ifstream& stream);
  static void spojIstoimeneSekcije();

  static void prepisiKodSekcije(int brojPrveSekcije, int brojDrugeSekcije, int velicinaPrve);
  static void prepisiRelokacioneZapise(int brojPrveSekcije, int brojDrugeSekcije, int velicinaPrve);

  static void spojSekcijeTabelaSimbola(int brojPrve, int brojDruge);

  static bool izbrisiSekcijuLinker(Sekcija x);
  static bool izbrisiSekcijuTabelaSimbola(TabelaSimbolaUlaz x);

  static void proveriPreklapanjaSekcija();

  static void proveriSimbole();
  static void ispisiRezultat();

  static void resiRelokacije();

  static bool izbrisiEksterneSimbole(TabelaSimbolaUlaz ulaz);

  static std::vector<Sekcija>::iterator vratiSekciju(std::string sekcija);


};