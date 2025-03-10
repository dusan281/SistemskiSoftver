#include "Asembler.hpp"


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














void Asembler::DirektivaSection(std::string simbol, int& line_num){

  zavrsiSekciju();

  for (auto i = tabelaSimbola.begin(); i != tabelaSimbola.end(); ++i){
    if (i->tip == "NOTYP" && i->simbol == simbol){ //sekcija ima isto ime kao neki drugi simbol iz tabele simbola
      ispisiGresku(line_num);
    }
  }
        
  int redniBroj = tabelaSimbola.size();

  
  trSekcija = new Sekcija(redniBroj);
  sveSekcije.insert({redniBroj,trSekcija});

  
  tabelaSimbola.push_back(*(new TabelaSimbolaUlaz(simbol,redniBroj,redniBroj,0,"SCTN"))); //dodavanje u tabelu simbola
}







void Asembler::DirektivaGlobal(int& line_num){

  for (int i = 0; i < argumenti.size(); i++){

    std::string simbol = argumenti[i]->simbol;
    auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
    
    if (it == tabelaSimbola.end()){ // simbol nije u tabeli
      int redniBroj = tabelaSimbola.size();
      tabelaSimbola.push_back(*(new TabelaSimbolaUlaz(simbol, 0, redniBroj, 0, "NOTYP", "GLOB")));
    }

    else{ // simbol je u tabeli
      if (it->brSekcije == -1){ //ako je simbol definisan vec sa .extern
        ispisiGresku(line_num);
      }

      else{ //ako je do sada definisan u fajlu ali se nije znalo da l cu da ga izvozim ili ne
        it->vezivanje = "GLOB";
      }
    }
  }

  argumenti.clear();
  argumenti.shrink_to_fit();  
}











void Asembler::DirektivaExtern(int& line_num){
  for (int i = 0; i < argumenti.size(); i++){
    std::string simbol = argumenti[i]->simbol;
    auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);

    if (it == tabelaSimbola.end()){ // simbol se ne nalazi u tabeli simbola
      int redniBroj = tabelaSimbola.size();
      tabelaSimbola.push_back(*(new TabelaSimbolaUlaz(simbol, -1, redniBroj, 0, "NOTYP", "GLOB"))); // brSekcije -1 => simbol definisan sa extern
    }



    else{
      if (it->brSekcije > 0) ispisiGresku(line_num); // uvezen simbol za kojeg sam rekao da cu da ga izvozim
      it->brSekcije = -1; it->vezivanje = "GLOB"; it->vrednost = 0;
    }
  
  }

  argumenti.clear();
  argumenti.shrink_to_fit();
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
        tabelaSimbola.push_back(*(new TabelaSimbolaUlaz(simbol, 0, redniBroj)));

        trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(0,"ABS",redniBroj,0)));
      }

      else{ // simbol se nalazi u tabeli simbola
        
        trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(trSekcija->LC,"ABS",it->redniBroj,0)));


      }
            
      trSekcija->kod_sekcije.push_back(0);
      trSekcija->kod_sekcije.push_back(0);
      trSekcija->kod_sekcije.push_back(0);
      trSekcija->kod_sekcije.push_back(0);
    }

    trSekcija->LC += 4;

  }

  argumenti.clear();
  argumenti.shrink_to_fit();
}















void Asembler::DirektivaEnd(){
  
  zavrsiSekciju();

  for (auto i = tabelaSimbola.begin(); i != tabelaSimbola.end(); i++){
    if (i->vezivanje == "GLOB" && i->brSekcije == 0) i->brSekcije = -1; // .global i .extern su alijasi
  }

  srediLokalneSimbole();

  for (const auto& sekcija : sveSekcije) {
      Asembler::ispisiKodSekcije(sekcija.second);
      std::cout << std::endl << std:: endl << std::endl << std:: endl;
  }

  std::cout << std::endl << std:: endl;
  
  std::cout<< "Simbol     Broj_sekcije     Vrednost     Tip       Vezivanje      Broj      Velicina" << std::endl;

  for (const auto& ulaz : tabelaSimbola) {
        std::cout << ulaz.simbol << " " << ulaz.brSekcije << " " << ulaz.vrednost << " " << ulaz.tip << " " << ulaz.vezivanje << " " << ulaz.redniBroj << " " << ulaz.velicina << std::endl;

        for (const auto& it : ulaz.backpatch){
          std::cout << it.first << " " << it.second << std::endl;
        }
  }

  
}
















void Asembler::obradiLabelu(std::string simbol, int& line_num){

  if (trSekcija == nullptr){ // ne moze labela van sekcije
    
    ispisiGresku(line_num); 

  } 

  auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
    
  if (it == tabelaSimbola.end()){ // simbol nije u tabeli
    int redniBroj = tabelaSimbola.size();
    tabelaSimbola.push_back(*(new TabelaSimbolaUlaz(simbol, trSekcija->brSekcije, redniBroj, trSekcija->LC, "NOTYP", "LOC")));
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