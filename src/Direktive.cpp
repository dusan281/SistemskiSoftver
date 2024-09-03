#include "../inc/Asembler.hpp"


void Asembler::DirektivaSkip(int& vrednost, int& line_num){
  if (trSekcija != nullptr){
    trSekcija->LC += vrednost;
    for (int i = 0 ; i < vrednost; i ++){
      trSekcija->kod_sekcije.push_back(0);
    }
  }
  else{ // skip korisceno van sekcija
        ispisiGresku(line_num);
  }
}

void Asembler::DirektivaSection(const std::string& simbol, int& line_num){

  zavrsiSekciju();

  for (auto i = tabelaSimbola.begin(); i != tabelaSimbola.end(); ++i){
    if (i->tip == "NOTYP" && i->simbol == simbol){ //sekcija ima isto ime kao neki drugi simbol iz tabele simbola
      ispisiGresku(line_num);
    }
  }
        
  int redniBroj = tabelaSimbola.size();

  
  trSekcija = new Sekcija(redniBroj,simbol);
  sveSekcije.insert({redniBroj,trSekcija});

  
  tabelaSimbola.push_back(TabelaSimbolaUlaz(simbol,redniBroj,redniBroj,0,"SCTN")); //dodavanje u tabelu simbola
}

void Asembler::DirektivaGlobal(int& line_num){

  for (int i = 0; i < argumenti.size(); i++){

    std::string simbol = argumenti[i]->simbol;
    auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
    
    if (it == tabelaSimbola.end()){ // simbol nije u tabeli
      int redniBroj = tabelaSimbola.size();
      tabelaSimbola.push_back(TabelaSimbolaUlaz(simbol, 0, redniBroj, 0, "NOTYP", "GLOB"));
    }

    else{ // simbol je u tabeli
      if (it->brSekcije == -1 || it->vezivanje == "WEAK_GLOB"){ //ako je simbol definisan vec sa .extern
        ispisiGresku(line_num);
      }

      else{ //ako je do sada definisan u fajlu ali se nije znalo da l cu da ga izvozim ili ne
        it->vezivanje = "GLOB";
      }
    }
  }


  for (Argument* arg : argumenti) delete arg;

  argumenti.clear();
  argumenti.shrink_to_fit();  
}






void Asembler::DirektivaWeak(int& line_num){
  
    for (int i = 0; i < argumenti.size(); i++){

    std::string simbol = argumenti[i]->simbol;
    auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
    
    if (it == tabelaSimbola.end()){ // simbol nije u tabeli
      int redniBroj = tabelaSimbola.size();
      tabelaSimbola.push_back(TabelaSimbolaUlaz(simbol, 0, redniBroj, 0, "NOTYP", "WEAK_GLOB"));
    }

    else{ // simbol je u tabeli
      if (it->vezivanje == "GLOB"){ //ako je simbol definisan vec sa .extern ili .global
        ispisiGresku(line_num);
      }

      else{ //ako je do sada definisan u fajlu ali se nije znalo da l cu da ga izvozim ili ne
        it->vezivanje = "WEAK_GLOB";
      }
    }
  }


  for (Argument* arg : argumenti) delete arg;

  argumenti.clear();
  argumenti.shrink_to_fit();
}



void Asembler::DirektivaType(const std::string& simbol, int& line_num){

  std::string tip = argumenti[0]->simbol;
  auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);

  if (it == tabelaSimbola.end()){ // simbol se ne nalazi u tabeli simbola
      int redniBroj = tabelaSimbola.size();
      tabelaSimbola.push_back(TabelaSimbolaUlaz(simbol, 0, redniBroj, 0, tip, "GLOB")); // brSekcije -1 => simbol definisan sa extern
  }
  else{
    it->tip = tip;
  }
}




void Asembler::DirektivaExtern(int& line_num){

  for (int i = 0; i < argumenti.size(); i++){
    std::string simbol = argumenti[i]->simbol;
    auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);

    if (it == tabelaSimbola.end()){ // simbol se ne nalazi u tabeli simbola
      int redniBroj = tabelaSimbola.size();
      tabelaSimbola.push_back(TabelaSimbolaUlaz(simbol, -1, redniBroj, 0, "NOTYP", "GLOB")); // brSekcije -1 => simbol definisan sa extern
    }



    else{

      if (it->brSekcije > 0 || it->vezivanje == "WEAK_GLOB") ispisiGresku(line_num); // uvezen simbol za kojeg sam rekao da cu da ga izvozim
      it->brSekcije = -1; it->vezivanje = "GLOB"; it->vrednost = 0;
    }
  
  }

  for (Argument* arg : argumenti) delete arg;

  argumenti.clear();
  argumenti.shrink_to_fit();
}



void Asembler::DirektivaAscii(const std::string& simbol, int& line_num){

  if (!trSekcija){

      ispisiGresku(line_num);
  }

  for (int i = 0; i < simbol.size(); i++){

    uint8_t arg = simbol[i];
    
    trSekcija->kod_sekcije.push_back(arg);
    trSekcija->LC ++;
  }


}




void Asembler::DirektivaWord(int& line_num){
  
  
  if (trSekcija == nullptr){ // ne moze word van sekcije
    
    ispisiGresku(line_num); 

  }



  for (int i = 0; i < argumenti.size(); i++){
    
    if (argumenti[i]->e == Operand::literal){ //kada je operand literal
  
      if ((argumenti[0]->vrednost > 2147483647) || (argumenti[0]->vrednost < -2147483648) ) ispisiGresku(line_num); // literal veci od 32 bita

      trSekcija->kod_sekcije.push_back( (argumenti[i]->vrednost >> 24) & 0xFF);
      trSekcija->kod_sekcije.push_back( (argumenti[i]->vrednost >> 16) & 0xFF); 
      trSekcija->kod_sekcije.push_back( (argumenti[i]->vrednost >> 8) & 0xFF);
      trSekcija->kod_sekcije.push_back( argumenti[i]->vrednost & 0xFF);

    }

    else{ // kada je operand simbol

      std::string simbol = argumenti[i]->simbol;
      auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);

      if (it == tabelaSimbola.end()){ // simbol se ne nalazi u tabeli simbola
        int redniBroj = tabelaSimbola.size();
        tabelaSimbola.push_back(TabelaSimbolaUlaz(simbol, 0, redniBroj));

        trSekcija->relokacioni_zapis.push_back(RelokacioniZapisUlaz(trSekcija->LC,"ABS",redniBroj,0, simbol));
      }

      else{ // simbol se nalazi u tabeli simbola
        
        trSekcija->relokacioni_zapis.push_back(RelokacioniZapisUlaz(trSekcija->LC,"ABS",it->redniBroj,0, simbol));


      }
            
      trSekcija->kod_sekcije.push_back(0);
      trSekcija->kod_sekcije.push_back(0);
      trSekcija->kod_sekcije.push_back(0);
      trSekcija->kod_sekcije.push_back(0);
    }

    trSekcija->LC += 4;

  }

  for (Argument* arg : argumenti) delete arg;

  argumenti.clear();
  argumenti.shrink_to_fit();
}

void Asembler::DirektivaEnd(){

  zavrsiSekciju();

  for (auto i = tabelaSimbola.begin(); i != tabelaSimbola.end(); i++){
    if ((i->vezivanje == "GLOB" || i->vezivanje == "WEAK_GLOB") && i->brSekcije == 0) {
      i->brSekcije = -1; // .global i .extern su alijasi
      i->vezivanje = "GLOB";
    }
  }

  srediLokalneSimbole();

  
  ispisiIzlazneFajlove();


  for (auto sekcija : sveSekcije){

    Sekcija* i = sekcija.second;
      for (auto ulaziBazen : i->promenljive_bazen) delete ulaziBazen.second;
    
    delete i;
  }

  for (auto simbol : tabelaSimbola){

    for (BackpatchUlaz* backUlaz : simbol.backpatch) delete backUlaz;
    simbol.backpatch = std::vector<BackpatchUlaz*>();
  }

  tabelaSimbola = std::vector<TabelaSimbolaUlaz>();
  sveSekcije = std::map<int,Sekcija*>();
  
}





void Asembler::obradiLabelu(const std::string& simbol, int& line_num){

  if (trSekcija == nullptr){ // ne moze labela van sekcije
    
    ispisiGresku(line_num); 

  } 

  auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
    
  if (it == tabelaSimbola.end()){ // simbol nije u tabeli
    int redniBroj = tabelaSimbola.size();
    tabelaSimbola.push_back(TabelaSimbolaUlaz(simbol, trSekcija->brSekcije, redniBroj, trSekcija->LC, "NOTYP", "LOC"));
  }

  else{ // simbol je u tabeli
    if (it->brSekcije != 0){ // simbol je vec u tabeli vec definisan ili je definisan sa extern
      ispisiGresku(line_num);
    }

    else{ 
      it->vrednost = trSekcija->LC;
      it->brSekcije = trSekcija->brSekcije;
    }
  }

}