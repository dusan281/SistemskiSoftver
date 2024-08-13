#include "Linker.hpp"


std::vector<Sekcija> Linker::sveSekcijeLinker = {};

std::vector<TabelaSimbolaUlaz> Linker::tabelaSimbolaLinker = {*(new TabelaSimbolaUlaz("",0,0))};

std::vector<char*> Linker::inputFilesNames = {};
std::ifstream Linker::inputFile;

int Linker::brojacSekcija = 0;




void Linker::pokreniLinker(){
  

   for (int i = 0; i < inputFilesNames.size(); i++){
    
    std::ifstream stream;
    stream.open(inputFilesNames[i], std::ios::in | std::ios::binary);
    procitajFajl(stream);
     
  }

  std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;

  for (int i = 0; i < tabelaSimbolaLinker.size(); i ++){
    std::cout << tabelaSimbolaLinker[i].simbol << " " << tabelaSimbolaLinker[i].brSekcije << " " << tabelaSimbolaLinker[i].vrednost << " " << tabelaSimbolaLinker[i].tip << " " << tabelaSimbolaLinker[i].redniBroj << std::endl;
  }



  int red = 1;
  std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
  std::cout <<sveSekcijeLinker.size() << " " << tabelaSimbolaLinker.size() << std::endl;

  
  while (red < sveSekcijeLinker.size() + 1){
    if (tabelaSimbolaLinker[red].tip == "SCTN"){
      red++;
    }
    else {
      int staraPozicija = 0;



      for (int i = red + 1 ; i < tabelaSimbolaLinker.size(); i++) {
        if (tabelaSimbolaLinker[i].tip == "SCTN"){
          staraPozicija = i;
          
          TabelaSimbolaUlaz pomoc = tabelaSimbolaLinker[i];
          tabelaSimbolaLinker[i] = tabelaSimbolaLinker[red];
          tabelaSimbolaLinker[red] = pomoc;

          tabelaSimbolaLinker[red].brSekcije = red;
          tabelaSimbolaLinker[red].redniBroj = red;
          i = tabelaSimbolaLinker.size();
        }
      }

      std::cout << staraPozicija << std::endl;


      for (int i = 0 ; i < tabelaSimbolaLinker.size(); i++){
        if (tabelaSimbolaLinker[i].tip == "NOTYP" && tabelaSimbolaLinker[i].brSekcije == staraPozicija) tabelaSimbolaLinker[i].brSekcije = red;
      }

      red++;
    }
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
          novaSekcija->LC = duzina_sekcije;

          for (int i = 0 ; i < duzina_sekcije; i ++){

            uint8_t kod;
            stream.read((char*)(&kod), sizeof(uint8_t));
            novaSekcija->kod_sekcije.push_back(kod);
          
          } 

          
          sveSekcijeLinker.push_back(*novaSekcija);

          for (int i = 0; i < sveSekcijeLinker[sveSekcijeLinker.size() - 1].kod_sekcije.size(); i ++){
            std::cout<< std::hex << static_cast<int>(sveSekcijeLinker[sveSekcijeLinker.size() - 1].kod_sekcije[i]) << std::endl;
          }
          


          delete novaSekcija;
          

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
            
            sveSekcijeLinker[sveSekcijeLinker.size() - 1].relokacioni_zapis.push_back(*noviZapis);

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
        if (tabelaSimbolaAsembler[i].brSekcije != -1){
          tabelaSimbolaAsembler[i].redniBroj = ++brojacSekcija;
        }
      }


      for (int i = 0; i < tabelaSimbolaAsembler.size(); i++){

        if (tabelaSimbolaAsembler[i].tip == "SCTN") {

          int stariBrojSekcije = tabelaSimbolaAsembler[i].brSekcije;
          
          tabelaSimbolaAsembler[i].brSekcije = tabelaSimbolaAsembler[i].redniBroj;


          for (int j = 0; j < tabelaSimbolaAsembler.size(); j++){
            if (tabelaSimbolaAsembler[j].tip == "NOTYP" && tabelaSimbolaAsembler[j].brSekcije == stariBrojSekcije) tabelaSimbolaAsembler[j].brSekcije = tabelaSimbolaAsembler[i].brSekcije;
          }
        }
      }

      
        


      for (int i = 0; i < tabelaSimbolaAsembler.size(); i++){
        if (tabelaSimbolaAsembler[i].brSekcije != -1){
          TabelaSimbolaUlaz* ulaz = new TabelaSimbolaUlaz(tabelaSimbolaAsembler[i]);
          tabelaSimbolaLinker.push_back(*ulaz);
        }
      }


      stream.close();
}