#include "Linker.hpp"


std::vector<SekcijaLinker> Linker::sveSekcijeLinker = {};

std::vector<std::vector<TabelaSimbolaUlaz>> Linker::tabeleSimbolaAsembler = {};

std::vector<TabelaSimbolaUlaz> Linker::tabelaSimbolaLinker = {*(new TabelaSimbolaUlaz("",0,0))};

std::vector<char*> Linker::inputFilesNames = {};
std::ifstream Linker::inputFile;




void Linker::pokreniLinker(){
  

   for (int i = 0; i < inputFilesNames.size(); i++){
    
    std::ifstream stream;
    stream.open(inputFilesNames[i], std::ios::in | std::ios::binary);
    procitajFajl(stream);
     
  }


 

  for (int i = 0; i < tabelaSimbolaLinker.size(); i ++){
    std::cout << tabelaSimbolaLinker[i].simbol << " " << tabelaSimbolaLinker[i].brSekcije << " " << tabelaSimbolaLinker[i].vrednost << " " << tabelaSimbolaLinker[i].tip << " " << tabelaSimbolaLinker[i].redniBroj << std::endl;
  }

}


void Linker::procitajFajl(std::ifstream& stream){

  int brojSekcija;

  stream.read((char*)(&brojSekcija), sizeof(int));

  for (int i = 0; i < brojSekcija; i++){

    int duzina_imena_sekcije;
    
    

    stream.read((char*)(&duzina_imena_sekcije), sizeof(int));

    char* ime_sekcije_char = (char*)malloc(duzina_imena_sekcije + 1);

    stream.read(ime_sekcije_char, (duzina_imena_sekcije) * sizeof(uint8_t));

    ime_sekcije_char[duzina_imena_sekcije] = '\0';

    std::string ime_sekcije = std::string(ime_sekcije_char);
    std::cout << ime_sekcije << std::endl;

    delete[] ime_sekcije_char;


    int duzina_sekcije;

    stream.read((char*)(&duzina_sekcije), sizeof(int));

    std::cout << duzina_sekcije << std::endl;



    
          Sekcija* novaSekcija = new Sekcija(tabelaSimbolaLinker.size(), ime_sekcije);

          tabelaSimbolaLinker.push_back(*(new TabelaSimbolaUlaz(ime_sekcije,tabelaSimbolaLinker.size(),tabelaSimbolaLinker.size(),0,"SCTN","LOC",duzina_imena_sekcije)));

          novaSekcija->LC = duzina_sekcije;

          for (int i = 0 ; i < duzina_sekcije; i ++){

            uint8_t kod;
            stream.read((char*)(&kod), sizeof(uint8_t));
            novaSekcija->kod_sekcije.push_back(kod);
          
          } 

          

          SekcijaLinker* novaSekcijaLinker = new SekcijaLinker(*novaSekcija, tabeleSimbolaAsembler.size());

          sveSekcijeLinker.push_back(*novaSekcijaLinker);

          for (int i = 0; i < sveSekcijeLinker[sveSekcijeLinker.size() - 1].sekcijaAsembler.kod_sekcije.size(); i ++){
            std::cout<< std::hex << static_cast<int>(sveSekcijeLinker[sveSekcijeLinker.size() - 1].sekcijaAsembler.kod_sekcije[i]) << std::endl;
          }
          


          delete novaSekcija;
          delete novaSekcijaLinker;
          

          int broj_relokacionih_zapisa;
          stream.read((char*)(&broj_relokacionih_zapisa), sizeof(int));

          std::cout<<broj_relokacionih_zapisa<<std::endl;

          for (int j = 0; j < broj_relokacionih_zapisa; j++){

            int offset;
            int simbolRB;
            int dodatak;
            int duzina_imena_simbola;
            char* ime_simbola_char = (char*)(malloc(duzina_imena_simbola + 1));

            stream.read((char*)(&offset), sizeof(int));
            stream.read((char*)(&simbolRB), sizeof(int));
            stream.read((char*)(&dodatak), sizeof(int));
            stream.read((char*)(&duzina_imena_simbola), sizeof(int));
            stream.read(ime_simbola_char, duzina_imena_simbola * sizeof(uint8_t));

            ime_simbola_char[duzina_imena_simbola] = '\0';

            std::string simbol = std::string(ime_simbola_char);

            std::cout << offset << " " << simbolRB << " " << dodatak << " " << duzina_imena_simbola << " " << simbol << std::endl;

            RelokacioniZapisUlaz* noviZapis = new RelokacioniZapisUlaz(offset, "ABS", simbolRB, dodatak, simbol);
            
            sveSekcijeLinker[sveSekcijeLinker.size() - 1].sekcijaAsembler.relokacioni_zapis.push_back(*noviZapis);

            delete[] ime_simbola_char;

          }
    



    
    

  }



      int duzina_tabele_simbola;
      stream.read((char*)(&duzina_tabele_simbola), sizeof(int));


      std::cout << duzina_tabele_simbola << std::endl;

      std::vector<TabelaSimbolaUlaz> tabelaSimbolaAsembler = {};
    
      for (int i = 0 ; i < duzina_tabele_simbola; i++){
        int duzina_imena_simbola;
        stream.read((char*)(&duzina_imena_simbola), sizeof(int));

        char* ime_simbola_char = (char*)malloc(duzina_imena_simbola + 1);
        stream.read(ime_simbola_char, duzina_imena_simbola * sizeof(uint8_t));

        ime_simbola_char[duzina_imena_simbola] = '\0';

        std::string ime_simbola = std::string(ime_simbola_char);

        delete[] ime_simbola_char;

        int broj_sekcije;
        stream.read((char*)(&broj_sekcije), sizeof(int));

        int vrednost;
        stream.read((char*)(&vrednost), sizeof(int));

        int duzina_tipa;
        stream.read((char*)(&duzina_tipa), sizeof(int));


        char* ime_tipa_char = (char*)malloc(duzina_tipa + 1);
        stream.read(ime_tipa_char, duzina_tipa * sizeof(uint8_t));

        ime_tipa_char[duzina_tipa] = '\0';

        std::string ime_tipa = std::string(ime_tipa_char);

        delete ime_tipa_char;

        int redniBroj;
        stream.read((char*)(&redniBroj), sizeof(int));

        std::cout << ime_simbola << " " << broj_sekcije << " " << vrednost << " " << ime_tipa << " " << redniBroj << std::endl;

        TabelaSimbolaUlaz* ulaz = new TabelaSimbolaUlaz(ime_simbola, broj_sekcije, redniBroj, vrednost, ime_tipa, "GLOB");

        tabelaSimbolaAsembler.push_back(*ulaz);
      }


      for (int i = 0; i < tabelaSimbolaAsembler.size(); i++){

        if (tabelaSimbolaAsembler[i].brSekcije != -1 && tabelaSimbolaAsembler[i].tip != "SCTN"){

          TabelaSimbolaUlaz* noviUlaz = new TabelaSimbolaUlaz(tabelaSimbolaAsembler[i].simbol, sveSekcijeLinker.size(),tabelaSimbolaLinker.size(), tabelaSimbolaAsembler[i].vrednost, "NOTYP", "GLOB");
          tabelaSimbolaLinker.push_back(*noviUlaz);

        }

      }

      tabeleSimbolaAsembler.push_back(tabelaSimbolaAsembler);
      stream.close();
}