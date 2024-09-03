#include "../inc/Linker.hpp"


std::vector<Sekcija> Linker::sveSekcijeLinker = {};

std::ostringstream Linker::oss;


bool Linker::greske = false;

std::vector<TabelaSimbolaUlaz> Linker::tabelaSimbolaLinker = {TabelaSimbolaUlaz("",0,0)};

std::vector<char*> Linker::inputFilesNames = {};
std::vector<char*> Linker::postavljanjaString = {};
std::ifstream Linker::inputFile;

int Linker::brojacSekcija = 0;
int Linker::brojacSimbola = 0;

char* Linker::outputFileName = nullptr;



void Linker::procitajInteger(int& a, std::ifstream& stream){

  stream.read((char*)(&a), sizeof(int));
}

std::string Linker::procitajString(int& duzina, std::ifstream& stream){

  char* s = (char*)malloc(duzina + 1);
  stream.read(s, (duzina) * sizeof(uint8_t));
  s[duzina] = '\0';
  std::string string = std::string(s);
  free(s);
  return string;
}


void Linker::pokreniLinkerRelocatable(){

   for (int i = 0; i < inputFilesNames.size(); i++){
    
    std::ifstream stream;
    stream.open(inputFilesNames[i], std::ios::in | std::ios::binary);
    procitajFajl(stream);
     
  }

  int i = 0;

  for (auto& simbol1 : tabelaSimbolaLinker){

    proveriDvaPutaDefinisanSimbol(simbol1, i, oss);
    i++;
  }


  izbaciPonavljanjaEksternihSimbola(); // ovo postoji jer je moguce da mi ostane nedefinisan simbol

  tabelaSimbolaLinker.erase(std::remove_if(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), izbrisiEksterneSimboleDefinisane), tabelaSimbolaLinker.end());


  spojIstoimeneSekcije();

  //postaviAdreseSekcija();


  std::fstream outputBinaryFile;
  outputBinaryFile.open(outputFileName, std::ios::out | std::ios::binary);

  if (!greske){

  ispisiBinarniFajl(outputBinaryFile);

  ispisiRezultat();
  }

  if (greske){
    std::cout << oss.str();
  }


}









void Linker::pokreniLinkerHex(){
  

   for (int i = 0; i < inputFilesNames.size(); i++){
    
    std::ifstream stream;
    stream.open(inputFilesNames[i], std::ios::in | std::ios::binary);
    procitajFajl(stream);

    stream.close();
     
  }

  int i = 0;
    for (auto& simbol1 : tabelaSimbolaLinker){

      proveriNedefinisanSimbol(simbol1,oss);

      proveriDvaPutaDefinisanSimbol(simbol1,i,oss);
      
      i++;
    }

  tabelaSimbolaLinker.erase(std::remove_if(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), izbrisiEksterneSimboleDefinisane), tabelaSimbolaLinker.end());


  spojIstoimeneSekcije();

  postaviAdreseSekcija();

  srediVrednostSimbola();

  std::sort(sveSekcijeLinker.begin(), sveSekcijeLinker.end(), sortirajSekcijePoAdresi);

  proveriPreklapanjaSekcija(oss);

  resiRelokacije();

  if (!greske) ispisiRezultat();

  if (greske) std::cout << oss.str();
  
  


}


void Linker::proveriPreklapanjaSekcija(std::ostringstream& oss){
  for (auto it = sveSekcijeLinker.begin(); it != sveSekcijeLinker.end() - 1; ++it){
    
      if (it->startnaAdresa + it->LC > (it+1)->startnaAdresa){
        greske = true;
        oss << "Sledece sekcije se preklapaju: " << it->imeSekcije << " " << (it + 1)->imeSekcije << std::endl;
      }
  }
}



void Linker::resiRelokacije(){


  for (auto& sekcijaLinker : sveSekcijeLinker){

    if (!sekcijaLinker.relokacioni_zapis.empty()){
      for (auto& relokZapis : sekcijaLinker.relokacioni_zapis){

        const auto& it = std::find(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), relokZapis.simbol);

        if (it->brSekcije != -1){

            int podatak = it->vrednost + relokZapis.dodatak;
            int adresa = relokZapis.offset;


            sekcijaLinker.kod_sekcije[adresa + 3] = ((podatak >>24) & 0xFF);
            sekcijaLinker.kod_sekcije[adresa + 2] = ((podatak >> 16) & 0xFF);
            sekcijaLinker.kod_sekcije[adresa + 1] = ((podatak >> 8) & 0xFF);
            sekcijaLinker.kod_sekcije[adresa ] = (podatak & 0xFF);
            
            relokZapis.tip = "RESEN";

        }
      }

    sekcijaLinker.relokacioni_zapis.erase(std::remove_if(sekcijaLinker.relokacioni_zapis.begin(), sekcijaLinker.relokacioni_zapis.end(), izbrisiRelokacioniZapis), sekcijaLinker.relokacioni_zapis.end());
      // obrisi sve resene relokacije iz tabele relokacija za tu sekciju
    }
  }


}




bool Linker::sortirajSekcijePoAdresi(Sekcija& a, Sekcija& b){
  return a.startnaAdresa < b.startnaAdresa;
}



void Linker::spojIstoimeneSekcije(){
  

  for (auto sekcija1 = sveSekcijeLinker.begin(); sekcija1 != sveSekcijeLinker.end() - 1; sekcija1++){

    for (auto sekcija2 = sekcija1 + 1; sekcija2 != sveSekcijeLinker.end(); sekcija2++){

      if (sekcija1->imeSekcije == sekcija2->imeSekcije){

        prepisiKodSekcije(sekcija1,sekcija2);

        prepisiRelokacioneZapise(sekcija1,sekcija2);

        sekcija1->LC += sekcija2->LC;

        sekcija2->brSekcije = 0;
      }

    }
  }


  sveSekcijeLinker.erase(std::remove_if(sveSekcijeLinker.begin(), sveSekcijeLinker.end(), izbrisiSekcijuLinker), sveSekcijeLinker.end());



  for (auto simbol1 = tabelaSimbolaLinker.begin(); simbol1 != tabelaSimbolaLinker.end() - 1; simbol1++){

    for (auto simbol2 = simbol1 + 1; simbol2 != tabelaSimbolaLinker.end(); simbol2++){

      if (simbol1->tip == "SCTN" && simbol2->tip == "SCTN" && simbol1->simbol == simbol2->simbol){ // provera da sekcije imaju ista imena
        spojSekcijeTabelaSimbola(simbol1,simbol2);
        simbol1->velicina += simbol2->velicina;
        simbol2->brSekcije = -10;
      }
    }
  }

    tabelaSimbolaLinker.erase(std::remove_if(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), izbrisiSekcijuTabelaSimbola), tabelaSimbolaLinker.end());

  for (auto& sekcija : sveSekcijeLinker){

    for (auto& simbol : tabelaSimbolaLinker){
      if (simbol.simbol == sekcija.imeSekcije) sekcija.brSekcije = simbol.redniBroj; // sada u sekcijama linkera imam redni broj koji odgovara stanju u tabeli simbola
    }

  }

}




void Linker::spojSekcijeTabelaSimbola(std::vector<TabelaSimbolaUlaz>::iterator &sekcija1, std::vector<TabelaSimbolaUlaz>::iterator &sekcija2){
  

  for (auto& simbol : tabelaSimbolaLinker){
    if (simbol.brSekcije == sekcija2->brSekcije && simbol.tip == "NOTYP"){
      simbol.brSekcije = sekcija1->brSekcije;
      simbol.vrednost += sekcija1->velicina;
    }
  }
}



void Linker::prepisiKodSekcije(std::vector<Sekcija>::iterator &sekcija1, std::vector<Sekcija>::iterator &sekcija2){

  for (int i = 0; i < sekcija2->kod_sekcije.size(); i++){
    sekcija1->kod_sekcije.push_back(sekcija2->kod_sekcije[i]);
  }

}


void Linker::prepisiRelokacioneZapise(std::vector<Sekcija>::iterator &sekcija1, std::vector<Sekcija>::iterator &sekcija2){

  for (int i =0; i < sekcija2->relokacioni_zapis.size(); i++){
    RelokacioniZapisUlaz* stariUlaz = &sekcija2->relokacioni_zapis[i];
    RelokacioniZapisUlaz ulaz = RelokacioniZapisUlaz(stariUlaz->offset + sekcija1->LC, stariUlaz->tip, stariUlaz->simbolRB, stariUlaz->dodatak, stariUlaz->simbol);

    sekcija1->relokacioni_zapis.push_back(ulaz);
  }
}




bool Linker::izbrisiSekcijuLinker(Sekcija& x){
  return x.brSekcije == 0;
}


bool Linker::izbrisiSekcijuTabelaSimbola(TabelaSimbolaUlaz& x){
  return x.brSekcije == -10;
}


bool Linker::izbrisiRelokacioniZapis(RelokacioniZapisUlaz& x){

  return x.tip == "RESEN";
}



void Linker::procitajFajl(std::ifstream& stream){

  int brojSekcija;

  
  procitajInteger(brojSekcija,stream);

  std::map<int,int> mapaSimbola;



  for (int i = 0; i < brojSekcija; i++){

        int duzina_imena_sekcije;    
        procitajInteger(duzina_imena_sekcije, stream);

        std::string ime_sekcije = procitajString(duzina_imena_sekcije, stream);

        int redniBroj_sekcije;
        procitajInteger(redniBroj_sekcije, stream);
        
        int duzina_sekcije;
        procitajInteger(duzina_sekcije, stream);

        int iteracija = sveSekcijeLinker.empty() ? 0 : sveSekcijeLinker.size();


        brojacSekcija++;

        std::pair<int,int> par = {redniBroj_sekcije, ++brojacSimbola};
        mapaSimbola.insert(par);

        Sekcija novaSekcija = Sekcija(tabelaSimbolaLinker.size(), ime_sekcije);

        novaSekcija.LC = duzina_sekcije;
        novaSekcija.brSekcije = brojacSekcija;
        novaSekcija.imeSekcije = ime_sekcije;

        for (int j = 0 ; j < duzina_sekcije; j ++){

          uint8_t kod;
          stream.read((char*)(&kod), sizeof(uint8_t));
          novaSekcija.kod_sekcije.push_back(kod);
        
        } 

              
        sveSekcijeLinker.push_back(novaSekcija);


        int broj_relokacionih_zapisa;
        procitajInteger(broj_relokacionih_zapisa, stream);


        for (int j = 0; j < broj_relokacionih_zapisa; j++){

          int offset;
          int simbolRB;
          int dodatak;
          int duzina_imena_simbola;
          

          procitajInteger(offset, stream);
          procitajInteger(simbolRB, stream);
          procitajInteger(dodatak, stream);
          procitajInteger(duzina_imena_simbola, stream);

          std::string simbol = procitajString(duzina_imena_simbola, stream);


          RelokacioniZapisUlaz noviZapis = RelokacioniZapisUlaz(offset, "ABS", simbolRB, dodatak, simbol);
          
          sveSekcijeLinker[sveSekcijeLinker.size() - 1].relokacioni_zapis.push_back(noviZapis);             
        }          
  }
    
      

  int duzina_tabele_simbola;
  procitajInteger(duzina_tabele_simbola, stream);


  std::vector<TabelaSimbolaUlaz> tabelaSimbolaAsembler = {};
    
  for (int i = 0 ; i < duzina_tabele_simbola; i++){
    
         int duzina_imena_simbola;
         procitajInteger(duzina_imena_simbola, stream);

         std::string ime_simbola = procitajString(duzina_imena_simbola, stream);

         int broj_sekcije;
         procitajInteger(broj_sekcije, stream);

         int vrednost;
         procitajInteger(vrednost, stream);

         int duzina_tipa;
         procitajInteger(duzina_tipa, stream);


         std::string ime_tipa = procitajString(duzina_tipa, stream);

         int redniBroj;
         procitajInteger(redniBroj, stream);

         int velicina;
         procitajInteger(velicina, stream);

         if (broj_sekcije > 0){
          
           broj_sekcije = mapaSimbola.at(broj_sekcije);
         }

         if (ime_tipa == "SCTN") redniBroj = broj_sekcije;
         if (ime_tipa == "NOTYP") redniBroj = ++brojacSimbola;

         TabelaSimbolaUlaz ulaz = TabelaSimbolaUlaz(ime_simbola, broj_sekcije, redniBroj, vrednost, ime_tipa, "GLOB", velicina);

         tabelaSimbolaAsembler.push_back(ulaz);
        
   }
      
        


  for (int i = 0; i < tabelaSimbolaAsembler.size(); i++){
      TabelaSimbolaUlaz ulaz = TabelaSimbolaUlaz(tabelaSimbolaAsembler[i]);
      tabelaSimbolaLinker.push_back(ulaz);
  }


  stream.close();
}




void Linker::ispisiRezultat(){

  std::fstream outputLinkerFile;
  outputLinkerFile.open("linker.txt",std::ios::out | std::ios::app);

  for (auto sekcija : sveSekcijeLinker){

    outputLinkerFile << "-------Masinski kod sekcije " << sekcija.imeSekcije << "-------" << std::endl ;

    for (int i = 0 ; i < sekcija.kod_sekcije.size(); i++){
    
      if (i % 8 == 0) outputLinkerFile<< std:: hex << i << ": ";
      outputLinkerFile << std::hex << static_cast<int>(sekcija.kod_sekcije[i]) << " ";
      if (i % 8 == 7) outputLinkerFile << std::endl;
    
    } 


    outputLinkerFile << std::endl;
    outputLinkerFile << std::endl;

    outputLinkerFile << "-------Relokacioni zapis-------"<<std::endl;

    int nameWidth = 20;
    const char separator = ' ';

    outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "OFFSET"
          << std::left << std::setw(nameWidth) << std::setfill(separator) << "TIP"
          << std::left << std::setw(nameWidth) << std::setfill(separator) << "SIMBOL"
          << std::left << std::setw(nameWidth) << std::setfill(separator) << "DODATAK"
          <<std::endl;
    



    for (int i = 0; i < sekcija.relokacioni_zapis.size(); i++){
      outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << sekcija.relokacioni_zapis[i].offset
          << std::left << std::setw(nameWidth) << std::setfill(separator) << sekcija.relokacioni_zapis[i].tip
          << std::left << std::setw(nameWidth) << std::setfill(separator) << sekcija.relokacioni_zapis[i].simbol
          << std::left << std::setw(nameWidth) << std::setfill(separator) << sekcija.relokacioni_zapis[i].dodatak;

      

  
      outputLinkerFile<<std::endl;
    }

    outputLinkerFile << std::endl;
    outputLinkerFile << std::endl;

  }

  outputLinkerFile << std::endl << std:: endl << std::endl << std:: endl;

  outputLinkerFile << std::endl << std:: endl;

  int nameWidth = 20;
  const char separator = ' ';

  outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "SIMBOL";
  outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "BROJ SEKCIJE";
  outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "VREDNOST";
  outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "TIP";
  outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "VEZIVANJE";
  outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "BROJ";
  outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "VELICINA";
  outputLinkerFile<<std::endl;
  

  for (const auto& ulaz : tabelaSimbolaLinker) {

        outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.simbol;
        outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.brSekcije;
        outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.vrednost;
        outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.tip;
        outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.vezivanje;
        outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.redniBroj;
        outputLinkerFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.velicina;

      outputLinkerFile << std::endl;

      
      
  }  

  std::fstream outputFileHex;
  outputFileHex.open(outputFileName ,std::ios::out | std::ios::binary);

  for (auto& sekcija : sveSekcijeLinker){

    for (int j = 0; j < sekcija.kod_sekcije.size(); j = j + 8){

      uint32_t adresa = sekcija.startnaAdresa + j;
      outputFileHex << std::hex << adresa << ": \t";

      for (int k = 0; k < 8; k ++)
        if (j + k < sekcija.kod_sekcije.size() ) {

          if (sekcija.kod_sekcije[j+k] < 16) outputFileHex << "0"; 

          outputFileHex << static_cast<int>(sekcija.kod_sekcije[j+k]) << " ";

        }
        outputFileHex << std::endl;
    }
  }


  outputFileHex.close();
  outputLinkerFile.close();
}




void Linker::postaviAdreseSekcija(){

  int br = 0;
  uint32_t najveca_adresa = 0;
  int offset = 0;

  if (!postavljanjaString.empty()){

    for (int i = 0; i < postavljanjaString.size(); i++){

      std::string s(postavljanjaString[i]);
      std::string sekcijaIme;
      std::uint32_t adresa;

      size_t startPos = s.find('=');
      size_t endPos = s.find('@', startPos);

    
      if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
          
          sekcijaIme = s.substr(startPos + 1, endPos - startPos - 1);
      }

      startPos = s.find('@');
      endPos = s.length();

      adresa = uint32_t(std::stoul(s.substr(startPos + 1, endPos - startPos - 1), nullptr, 16));
      

      for (auto& sekcija : sveSekcijeLinker){

        if (sekcija.imeSekcije == sekcijaIme) {

          sekcija.startnaAdresa = adresa;
          sekcija.postavljenaAdresa = true;

          auto it = std::find(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), sekcija.imeSekcije);

          it->vrednost = adresa;


          if (adresa > najveca_adresa) {

            najveca_adresa = adresa;
            offset = sekcija.LC;

          }

        }
      }
    br++;
    }
  
  }


  for (auto& sekcija : sveSekcijeLinker){

    if (!sekcija.postavljenaAdresa){

      sekcija.startnaAdresa = najveca_adresa + offset;
      sekcija.postavljenaAdresa = true;
      najveca_adresa = sekcija.startnaAdresa;
      offset = sekcija.LC;

      auto it = std::find(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), sekcija.imeSekcije);

          it->vrednost = sekcija.startnaAdresa;
    }
  }


  

}


void Linker::srediVrednostSimbola(){

  for (auto& simbol : tabelaSimbolaLinker){
    if (simbol.tip != "SCTN" && simbol.brSekcije != -1){

      auto it = std::find(tabelaSimbolaLinker.begin(), tabelaSimbolaLinker.end(), simbol.brSekcije);
      simbol.vrednost += it->vrednost;
    }
    

  
  }
}


void Linker::proveriDvaPutaDefinisanSimbol(TabelaSimbolaUlaz& simbol1, int i, std::ostringstream& oss){

  

  if (simbol1.brSekcije != -1 && simbol1.tip != "SCTN"){

        bool flag = false;
        

        for (int j = 0; j < tabelaSimbolaLinker.size(); j++){

          if (tabelaSimbolaLinker[j].simbol == simbol1.simbol && tabelaSimbolaLinker[j].brSekcije != -1 && i != j && simbol1.simbol != ""){
            flag = true;
            break; // naisao na 2 puta definisani simbol
          } 
        }

        if (flag == true){
          greske = true;
            oss << "Simbol definisan u dve razlicite sekcije: " << tabelaSimbolaLinker[i].simbol << std::endl;
        }

      }
}



void Linker::proveriNedefinisanSimbol(TabelaSimbolaUlaz& simbol1, std::ostringstream& oss){

  if (simbol1.brSekcije == -1){
        bool flag = false; // ako naidjem na nedefinisani simbol

        for (auto& simbol2 : tabelaSimbolaLinker){
          if (simbol2.simbol == simbol1.simbol && simbol2.brSekcije != -1){
            flag = true;
            break;
          } 
        }

        if (flag == false){
          greske = true;
            oss << "Koriscen lokalni nedefnisani simbol: " << simbol1.simbol << std::endl;
        }

      }
}



bool Linker::izbrisiEksterneSimboleDefinisane(TabelaSimbolaUlaz& ulaz){
  
  for (auto simbol : tabelaSimbolaLinker){

    if (ulaz.redniBroj != simbol.redniBroj && ulaz.brSekcije == -1 && ulaz.simbol == simbol.simbol) return true; // da mi se ne bi ponavljali eksterni simboli u tabeli simbola
  }

  return false;
  
  
}






void Linker::ispisiInteger(int& a, std::fstream& stream){

  stream.write((char*)(&a), sizeof(int));
}

void Linker::ispisiString(std::string& s, std::fstream& stream){

  stream.write(s.c_str(), s.size());
}




void Linker::izbaciPonavljanjaEksternihSimbola(){

  for (auto it = tabelaSimbolaLinker.begin(); it != tabelaSimbolaLinker.end() - 1 ; it++){
    
    for (auto it1 = it + 1; it1 != tabelaSimbolaLinker.end(); it1++ ){

      if (it->brSekcije == -1 && it1->brSekcije == -1 && it->simbol == it1->simbol){

        tabelaSimbolaLinker.erase(it1);
        it1 = it1++;
      }
    }
  }
}



void Linker::ispisiBinarniFajl(std::fstream& stream){

  int broj_sekcija = sveSekcijeLinker.size();
  ispisiInteger(broj_sekcija, stream);   // ukupan broj sekcija

  for (auto tr : sveSekcijeLinker){

    int duzina_imena_sekcije = tr.imeSekcije.size();
    ispisiInteger(duzina_imena_sekcije, stream); //duzina imena sekcije

    ispisiString(tr.imeSekcije, stream); // ime sekcije

    ispisiInteger(tr.brSekcije, stream); // redni broj sekcije

    ispisiInteger(tr.LC, stream); // velicina sekcije

    for (int i = 0 ; i < tr.kod_sekcije.size(); i++) stream.write((char*)(&tr.kod_sekcije[i]), sizeof(uint8_t)); // ispisujem i binarni fajl svaki bajt koda sekcije

    int duzina_relokacionih_zapisa = tr.relokacioni_zapis.size();
    ispisiInteger(duzina_relokacionih_zapisa, stream);  // koliko ime relokacionih zapisa

    for (int i = 0; i < tr.relokacioni_zapis.size(); i++){

        int duzina_imena_simbola = tr.relokacioni_zapis[i].simbol.size();

        ispisiInteger(tr.relokacioni_zapis[i].offset, stream);
        ispisiInteger(tr.relokacioni_zapis[i].simbolRB, stream);
        ispisiInteger(tr.relokacioni_zapis[i].dodatak, stream);
        ispisiInteger(duzina_imena_simbola, stream);
        ispisiString(tr.relokacioni_zapis[i].simbol, stream);

    }

  }


  int duzina_tabele_simbola = tabelaSimbolaLinker.size();
  ispisiInteger(duzina_tabele_simbola, stream);

  for (auto simbol : tabelaSimbolaLinker){

    int duzina_imena_simbola = simbol.simbol.size();

    ispisiInteger(duzina_imena_simbola, stream);
    ispisiString(simbol.simbol, stream);

    ispisiInteger(simbol.brSekcije, stream);

    ispisiInteger(simbol.vrednost, stream);
    

    int duzina_tipa = simbol.tip.size();

    ispisiInteger(duzina_tipa, stream);
    ispisiString(simbol.tip, stream);

    ispisiInteger(simbol.redniBroj, stream);

    ispisiInteger(simbol.velicina, stream);

  }
}