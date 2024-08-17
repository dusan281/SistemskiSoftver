#include "Linker.hpp"


std::vector<Sekcija> Linker::sveSekcijeLinker = {};

std::vector<TabelaSimbolaUlaz> Linker::tabelaSimbolaLinker = {*(new TabelaSimbolaUlaz("",0,0))};

std::vector<char*> Linker::inputFilesNames = {};
std::vector<char*> Linker::postavljanjaString = {};
std::ifstream Linker::inputFile;

int Linker::brojacSekcija = 0;
int Linker::brojacSimbola = 0;

char* Linker::outputFileName = nullptr;

void Linker::pokreniLinker(){
  

   for (int i = 0; i < inputFilesNames.size(); i++){
    
    std::ifstream stream;
    stream.open(inputFilesNames[i], std::ios::in | std::ios::binary);
    procitajFajl(stream);
     
  }

  proveriSimbole();

  spojIstoimeneSekcije();

  postaviAdreseSekcija();

  std::sort(sveSekcijeLinker.begin(), sveSekcijeLinker.end(), sortirajSekcijePoAdresi);

  proveriPreklapanjaSekcija();

  resiRelokacije();
  
  ispisiRezultat();


}






void Linker::proveriPreklapanjaSekcija(){
  for (int i = 0; i < sveSekcijeLinker.size() - 1; i ++){
    
      if (sveSekcijeLinker[i].startnaAdresa + sveSekcijeLinker[i].LC > sveSekcijeLinker[i+1].startnaAdresa){
        std::ostringstream oss; 
        oss << "ledece sekcije se preklapaju: " << sveSekcijeLinker[i].imeSekcije << " " << sveSekcijeLinker[i+1].imeSekcije;
        throw std::runtime_error(oss.str());
      }
  }
}


void Linker::resiRelokacije(){


  for (int i = 0; i < sveSekcijeLinker.size(); i++){

    if (!sveSekcijeLinker[i].relokacioni_zapis.empty()){
      for (int j = 0; j < sveSekcijeLinker[i].relokacioni_zapis.size(); j++){

      auto it = std::find(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), sveSekcijeLinker[i].relokacioni_zapis[j].simbol);

      

      int podatak = it->vrednost + sveSekcijeLinker[i].relokacioni_zapis[j].dodatak;
      int adresa = sveSekcijeLinker[i].relokacioni_zapis[j].offset;


      sveSekcijeLinker[i].kod_sekcije[adresa + 3] = ((podatak >>24) & 0xFF);
      sveSekcijeLinker[i].kod_sekcije[adresa + 2] = ((podatak >> 16) & 0xFF);
      sveSekcijeLinker[i].kod_sekcije[adresa + 1] = ((podatak >> 8) & 0xFF);
      sveSekcijeLinker[i].kod_sekcije[adresa ] = (podatak & 0xFF);
      }
    }

    

  }


}


bool Linker::sortirajSekcijePoAdresi(Sekcija a, Sekcija b){
  return a.startnaAdresa < b.startnaAdresa;
}



void Linker::spojIstoimeneSekcije(){
  

  for (int i = 0 ; i < sveSekcijeLinker.size() - 1; i++){

    for (int j = i + 1; j < sveSekcijeLinker.size(); j++){

      if (sveSekcijeLinker[i].imeSekcije == sveSekcijeLinker[j].imeSekcije){

        prepisiKodSekcije(i,j, sveSekcijeLinker[i].kod_sekcije.size());

        prepisiRelokacioneZapise(i,j, sveSekcijeLinker[i].LC);

        sveSekcijeLinker[i].LC += sveSekcijeLinker[j].LC;

        sveSekcijeLinker[j].brSekcije = 0;
      }

    }
  }


  sveSekcijeLinker.erase(std::remove_if(sveSekcijeLinker.begin(), sveSekcijeLinker.end(), izbrisiSekcijuLinker), sveSekcijeLinker.end());



  for (int i = 0; i < tabelaSimbolaLinker.size() - 1; i++){

    for (int j = i + 1; j < tabelaSimbolaLinker.size(); j++){

      if (tabelaSimbolaLinker[i].tip == "SCTN" && tabelaSimbolaLinker[j].tip == "SCTN" && tabelaSimbolaLinker[i].simbol == tabelaSimbolaLinker[j].simbol){
        spojSekcijeTabelaSimbola(i,j);
        tabelaSimbolaLinker[i].velicina += tabelaSimbolaLinker[j].velicina;
        tabelaSimbolaLinker[j].brSekcije = -10;
      }
    }
  }

    tabelaSimbolaLinker.erase(std::remove_if(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), izbrisiSekcijuTabelaSimbola), tabelaSimbolaLinker.end());

  for (int i = 0; i < sveSekcijeLinker.size(); i++){

    for (int j = 0 ; j < tabelaSimbolaLinker.size(); j++){
      if (tabelaSimbolaLinker[j].simbol == sveSekcijeLinker[i].imeSekcije) sveSekcijeLinker[i].brSekcije = tabelaSimbolaLinker[j].redniBroj;
    }

  }

}



void Linker::spojSekcijeTabelaSimbola(int brojPrve, int brojDruge){
  

  for (int i = 0 ; i < tabelaSimbolaLinker.size(); i++){
    if (tabelaSimbolaLinker[i].brSekcije == tabelaSimbolaLinker[brojDruge].brSekcije && tabelaSimbolaLinker[i].tip == "NOTYP"){
      tabelaSimbolaLinker[i].brSekcije = tabelaSimbolaLinker[brojPrve].brSekcije;
      tabelaSimbolaLinker[i].vrednost += tabelaSimbolaLinker[brojPrve].velicina;
    }
  }
}



void Linker::prepisiKodSekcije(int brojPrveSekcije, int brojDrugeSekcije, int velicinaPrve){

  for (int i = 0; i < sveSekcijeLinker[brojDrugeSekcije].kod_sekcije.size(); i++){
    sveSekcijeLinker[brojPrveSekcije].kod_sekcije.push_back(sveSekcijeLinker[brojDrugeSekcije].kod_sekcije[i]);
  }

}

void Linker::prepisiRelokacioneZapise(int brojPrveSekcije, int brojDrugeSekcije, int velicinaPrve){

  for (int i =0; i < sveSekcijeLinker[brojDrugeSekcije].relokacioni_zapis.size(); i++){
    RelokacioniZapisUlaz* stariUlaz = &sveSekcijeLinker[brojDrugeSekcije].relokacioni_zapis[i];
    RelokacioniZapisUlaz* ulaz = new RelokacioniZapisUlaz(stariUlaz->offset + velicinaPrve, stariUlaz->tip, stariUlaz->simbolRB, stariUlaz->dodatak, stariUlaz->simbol);

    sveSekcijeLinker[brojPrveSekcije].relokacioni_zapis.push_back(*ulaz);
  }
}




bool Linker::izbrisiSekcijuLinker(Sekcija x){
  return x.brSekcije == 0;
}


bool Linker::izbrisiSekcijuTabelaSimbola(TabelaSimbolaUlaz x){
  return x.brSekcije == -10;
}




void Linker::procitajFajl(std::ifstream& stream){

  int brojSekcija;

  stream.read((char*)(&brojSekcija), sizeof(int));


  std::map<int,int> mapaSimbola;



  for (int i = 0; i < brojSekcija; i++){

    int duzina_imena_sekcije;
    
    

    stream.read((char*)(&duzina_imena_sekcije), sizeof(int));

    char* ime_sekcije_char = (char*)malloc(duzina_imena_sekcije + 1);

    stream.read(ime_sekcije_char, (duzina_imena_sekcije) * sizeof(uint8_t));

    ime_sekcije_char[duzina_imena_sekcije] = '\0';

    std::string ime_sekcije = std::string(ime_sekcije_char);


    delete[] ime_sekcije_char;

    int redniBroj_sekcije;

    stream.read((char*)(&redniBroj_sekcije), sizeof(int));

    int duzina_sekcije;

    stream.read((char*)(&duzina_sekcije), sizeof(int));


    int iteracija = sveSekcijeLinker.empty() ? 0 : sveSekcijeLinker.size();

    bool flag = false;


    if (flag == false){

      brojacSekcija++;

        std::pair<int,int> par = {redniBroj_sekcije, ++brojacSimbola};

        mapaSimbola.insert(par);

        Sekcija* novaSekcija = new Sekcija(tabelaSimbolaLinker.size(), ime_sekcije);
          novaSekcija->LC = duzina_sekcije;
          novaSekcija->brSekcije = brojacSekcija;
          novaSekcija->imeSekcije = ime_sekcije;

          for (int j = 0 ; j < duzina_sekcije; j ++){

            uint8_t kod;
            stream.read((char*)(&kod), sizeof(uint8_t));
            novaSekcija->kod_sekcije.push_back(kod);
          
          } 

          
          sveSekcijeLinker.push_back(*novaSekcija);


          delete novaSekcija;







          int broj_relokacionih_zapisa;
          stream.read((char*)(&broj_relokacionih_zapisa), sizeof(int));


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


            RelokacioniZapisUlaz* noviZapis = new RelokacioniZapisUlaz(offset, "ABS", simbolRB, dodatak, simbol);
            
            sveSekcijeLinker[sveSekcijeLinker.size() - 1].relokacioni_zapis.push_back(*noviZapis);

            delete[] ime_simbola_char;

          }
    }

            
            
    }
    
          













      int duzina_tabele_simbola;
      stream.read((char*)(&duzina_tabele_simbola), sizeof(int));


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

        int velicina;
        stream.read((char*)(&velicina), sizeof(int));

        if (broj_sekcije != -1){
          broj_sekcije = mapaSimbola.at(broj_sekcije);
        }

          if (ime_tipa == "SCTN") redniBroj = broj_sekcije;
          if (ime_tipa == "NOTYP") redniBroj = ++brojacSimbola;

          TabelaSimbolaUlaz* ulaz = new TabelaSimbolaUlaz(ime_simbola, broj_sekcije, redniBroj, vrednost, ime_tipa, "GLOB", velicina);

          tabelaSimbolaAsembler.push_back(*ulaz);
        

        
      }
      
        


      for (int i = 0; i < tabelaSimbolaAsembler.size(); i++){
          TabelaSimbolaUlaz* ulaz = new TabelaSimbolaUlaz(tabelaSimbolaAsembler[i]);
          tabelaSimbolaLinker.push_back(*ulaz);
      }


      stream.close();
}




void Linker::ispisiRezultat(){
  for (int i = 0; i < tabelaSimbolaLinker.size(); i ++){
    std::cout << std::hex << tabelaSimbolaLinker[i].simbol << " " << tabelaSimbolaLinker[i].brSekcije << " " << tabelaSimbolaLinker[i].vrednost << " " << tabelaSimbolaLinker[i].tip << " " << tabelaSimbolaLinker[i].redniBroj << " " << tabelaSimbolaLinker[i].velicina << std::endl;
  }


  std::cout << "SADA SEKCIJE" << std::endl;

  for (int i = 0; i < sveSekcijeLinker.size(); i++){

    std::cout << std::endl;

    std::cout << "SEKCIJA " << sveSekcijeLinker[i].imeSekcije << " " << sveSekcijeLinker[i].brSekcije << " " << sveSekcijeLinker[i].startnaAdresa << std::endl;

    for (int j = 0; j < sveSekcijeLinker[i].relokacioni_zapis.size(); j++){
      std::cout << sveSekcijeLinker[i].relokacioni_zapis[j].simbol << " " << sveSekcijeLinker[i].relokacioni_zapis[j].offset <<  " " << sveSekcijeLinker[i].relokacioni_zapis[j].dodatak << std::endl;;
    }
  }

  std::fstream outputFileHex;
  outputFileHex.open(outputFileName ,std::ios::out | std::ios::binary);

  for (int i = 0 ; i < sveSekcijeLinker.size(); i ++){
    std::cout << std::endl << std::endl << std::endl << sveSekcijeLinker[i].imeSekcije << std::endl;
    for (int j = 0; j < sveSekcijeLinker[i].kod_sekcije.size(); j++){
      if (j % 4 == 0) std::cout << std::endl;
      std::cout << std::hex << static_cast<int>(sveSekcijeLinker[i].kod_sekcije[j]) << " ";
    }
  }


  for (int i = 0; i < sveSekcijeLinker.size(); i++){

    for (int j = 0; j < sveSekcijeLinker[i].kod_sekcije.size(); j = j + 8){
      uint32_t adresa = sveSekcijeLinker[i].startnaAdresa + j;
      outputFileHex << std::hex << adresa << ": \t";
      for (int k = 0; k < 8; k ++)
        if (j + k < sveSekcijeLinker[i].kod_sekcije.size() ) {
          if (sveSekcijeLinker[i].kod_sekcije[j+k] < 16) outputFileHex << "0"; 
          outputFileHex << static_cast<int>(sveSekcijeLinker[i].kod_sekcije[j+k]) << " ";

        }
        outputFileHex << std::endl;
    }
  }

  outputFileHex.close();
}




void Linker::postaviAdreseSekcija(){

  int br = 0;
  uint32_t najveca_adresa = 0;
  int offset = 0;

  if (!postavljanjaString.empty()){

    for (int i = 0; i < postavljanjaString.size(); i++){

      std::string s(postavljanjaString[i]);
      std::string sekcija;
      std::uint32_t adresa;

      size_t startPos = s.find('=');
      size_t endPos = s.find('@', startPos);

    
      if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
          
          sekcija = s.substr(startPos + 1, endPos - startPos - 1);
      }

      startPos = s.find('@');
      endPos = s.length();

      adresa = uint32_t(std::stoul(s.substr(startPos + 1, endPos - startPos - 1), nullptr, 16));
      

      for (int j = 0; j < sveSekcijeLinker.size(); j++){
        if (sveSekcijeLinker[j].imeSekcije == sekcija) {
          sveSekcijeLinker[j].startnaAdresa = adresa;
          sveSekcijeLinker[j].postavljenaAdresa = true;

          auto it = std::find(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), sveSekcijeLinker[j].imeSekcije);

          it->vrednost = adresa;


          if (adresa > najveca_adresa) {

            najveca_adresa = adresa;
            offset = sveSekcijeLinker[j].LC;

          }

        }
      }
    br++;
    }
  
  }


  for (int i = 0; i < sveSekcijeLinker.size(); i++){
    if (!sveSekcijeLinker[i].postavljenaAdresa){
      sveSekcijeLinker[i].startnaAdresa = najveca_adresa + offset;
      sveSekcijeLinker[i].postavljenaAdresa = true;
      najveca_adresa = sveSekcijeLinker[i].startnaAdresa;
      offset = sveSekcijeLinker[i].LC;

      auto it = std::find(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), sveSekcijeLinker[i].imeSekcije);

          it->vrednost = sveSekcijeLinker[i].startnaAdresa;
    }
  }


  for (int i = 0; i < tabelaSimbolaLinker.size(); i++){
    if (tabelaSimbolaLinker[i].tip != "SCTN" && tabelaSimbolaLinker[i].brSekcije != -1){

      auto it = std::find(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), tabelaSimbolaLinker[i].brSekcije);
      tabelaSimbolaLinker[i].vrednost += it->vrednost;
    }
    

  
  }

}



void Linker::proveriSimbole(){


    for (int i = 0; i <tabelaSimbolaLinker.size(); i++){

      if (tabelaSimbolaLinker[i].brSekcije == -1){
        bool flag = false;

        for (int j = 0; j < tabelaSimbolaLinker.size(); j++){
          if (tabelaSimbolaLinker[j].simbol == tabelaSimbolaLinker[i].simbol && tabelaSimbolaLinker[j].brSekcije != -1) flag = true;
        }

        if (flag == false){
            std::ostringstream oss; 
            oss << "Koriscen lokalni nedefnisani simbol: " << tabelaSimbolaLinker[i].simbol;
            throw std::runtime_error(oss.str());
        }

      }


      if (tabelaSimbolaLinker[i].brSekcije != -1 && tabelaSimbolaLinker[i].tip != "SCTN"){
        bool flag = false;

        for (int j = 0; j < tabelaSimbolaLinker.size(); j++){
          if (tabelaSimbolaLinker[j].simbol == tabelaSimbolaLinker[i].simbol && tabelaSimbolaLinker[j].brSekcije != -1 && i!=j) flag = true;
        }

        if (flag == true){
            std::ostringstream oss; 
            oss << "Simbol definisan u dve razlicite sekcije: " << tabelaSimbolaLinker[i].simbol;
            throw std::runtime_error(oss.str());
        }

      }
    }

    tabelaSimbolaLinker.erase(std::remove_if(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), izbrisiEksterneSimbole), tabelaSimbolaLinker.end());




}



bool Linker::izbrisiEksterneSimbole(TabelaSimbolaUlaz ulaz){
  return ulaz.brSekcije == -1;
}