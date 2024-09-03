#include "Asembler.hpp"

class Linker{

public:

  static bool greske;
  static std::ostringstream oss;

  static int brojacSekcija;
  static int brojacSimbola;

  static std::vector<Sekcija> sveSekcijeLinker;
  static std::vector<TabelaSimbolaUlaz> tabelaSimbolaLinker;

  static std::vector<char*> inputFilesNames;
  static char* outputFileName;

  static std::vector<char*> postavljanjaString;
  static void postaviAdreseSekcija();
  static bool sortirajSekcijePoAdresi(Sekcija& a, Sekcija& b);

  static std::ifstream inputFile;


  static void pokreniLinkerHex();
  static void pokreniLinkerRelocatable();


  static void proveriNedefinisanSimbol(TabelaSimbolaUlaz& simbol1, std::ostringstream& oss);
  static void proveriDvaPutaDefinisanSimbol(TabelaSimbolaUlaz& simbol1, int i, std::ostringstream& oss);
  static void proveriWeakSimbole(std::ostringstream& oss);

  static void procitajFajl(std::ifstream& stream);
  static void procitajInteger(int& a, std::ifstream& stream);
  static std::string procitajString(int& duzina, std::ifstream& stream);


  static void spojIstoimeneSekcije();

  static void prepisiKodSekcije(std::vector<Sekcija>::iterator &sekcija1, std::vector<Sekcija>::iterator &sekcija2);
  static void prepisiRelokacioneZapise(std::vector<Sekcija>::iterator &sekcija1, std::vector<Sekcija>::iterator &sekcija2);

  static void spojSekcijeTabelaSimbola(std::vector<TabelaSimbolaUlaz>::iterator &sekcija1, std::vector<TabelaSimbolaUlaz>::iterator &sekcija2);

  static bool izbrisiSekcijuLinker(Sekcija& x);
  static bool izbrisiRelokacioniZapis(RelokacioniZapisUlaz& x);
  static bool izbrisiSekcijuTabelaSimbola(TabelaSimbolaUlaz& x);

  static void proveriPreklapanjaSekcija(std::ostringstream& oss);

  static void ispisiRezultat();

  static void resiRelokacije();

  static void srediVrednostSimbola();

  static void izbaciPonavljanjaEksternihSimbola();

  static bool izbrisiEksterneSimboleDefinisane(TabelaSimbolaUlaz& ulaz);
  static bool izbrisiWeakSimbole(TabelaSimbolaUlaz& ulaz);

  static std::vector<Sekcija>::iterator vratiSekciju(std::string& sekcija);


  static void ispisiBinarniFajl(std::fstream& stream);

  static void ispisiInteger(int& a, std::fstream& stream);
  static void ispisiString(std::string& s, std::fstream& stream);

  static void proveriTipSimbola(TabelaSimbolaUlaz& simbol1, int i, std::ostringstream& oss);

  
};