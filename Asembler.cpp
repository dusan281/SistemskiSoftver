#include "Asembler.hpp"


std::vector<Argument*> Asembler::argumenti = {};
Sekcija* Asembler::trSekcija = nullptr;
std::map<int, Sekcija*> Asembler::sveSekcije = {};
std::vector<TabelaSimbolaUlaz> Asembler::tabelaSimbola = {*(new TabelaSimbolaUlaz("",0,0))};


char* Asembler::outputFileName = nullptr;
char* Asembler::outputBinaryName = nullptr;


Adresiranje Asembler::adresiranje = Adresiranje::MEM_DIR;

std::fstream Asembler::outputFile;
std::fstream Asembler::outputFileBinary;


void Asembler::postaviAdresiranje(enum Adresiranje a){
  Asembler::adresiranje = a;
}

void Asembler::dodajUListuArgumenata(Argument* arg, int pozicija){
  if (pozicija != -1){
    argumenti.insert(argumenti.begin(), arg);
    return;
  }
  argumenti.push_back(arg);
}

void Asembler::zavrsiSekciju(){
  if (trSekcija){
    auto it = std::find(tabelaSimbola.begin(), tabelaSimbola.end(), trSekcija->brSekcije);
    if (it != tabelaSimbola.end()){
      it->velicina = trSekcija->LC;
    }
  }
}

void Asembler::ispisiGresku(int& line_num){
  std::ostringstream oss;
  oss << "Greška linija: " << line_num;
  throw std::runtime_error(oss.str());
}

void Asembler::ispisiKodSekcije(Sekcija* tr){
  outputFile << "Nalazim se u sekciji " << tr->imeSekcije << std::endl ;

  int duzina_imena_sekcije = tr->imeSekcije.size();
  outputFileBinary.write((char*)(&duzina_imena_sekcije), sizeof(int)); //duzina imena sekcije

  outputFileBinary.write(tr->imeSekcije.c_str(), tr->imeSekcije.size()); // ime sekcije

  //int redniBroj_sekcije = tr->brSekcije;
  //outputFileBinary.write((char*)(&redniBroj_sekcije), sizeof(int)); // redni broj sekcije

  int velicina_sekcije = tr->LC;
  outputFileBinary.write((char*)(&velicina_sekcije), sizeof(int)); // velicina sekcije


  for (int i = 0 ; i < tr->kod_sekcije.size(); i++){
    if (i % 4 == 0) outputFile<<std::endl;
    outputFile << std::hex << static_cast<int>(tr->kod_sekcije[i]) << " ";

    outputFileBinary.write((char*)(&tr->kod_sekcije[i]), sizeof(uint8_t)); // ispisujem i binarni fajl svaki bajt koda sekcije
    
  }
  outputFile << std::endl << std::endl << std::endl;

  outputFile << " RELOKACIONI ZAPIS"<<std::endl;

  int nameWidth = 20;
  const char separator = ' ';

  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "OFFSET";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "TIP";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "SIMBOL";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "DODATAK";
  outputFile<<std::endl;
  

  int duzina_relokacionih_zapisa = tr->relokacioni_zapis.size();
  outputFileBinary.write((char*)(&duzina_relokacionih_zapisa), sizeof(int));  // koliko ime relokacionih zapisa


  for (int i = 0; i < tr->relokacioni_zapis.size(); i++){
    outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << tr->relokacioni_zapis[i].offset;
    outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << tr->relokacioni_zapis[i].tip;
    outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << tr->relokacioni_zapis[i].simbolRB;
    outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << tr->relokacioni_zapis[i].dodatak;

    int offset = tr->relokacioni_zapis[i].offset;
    int simbolRB = tr->relokacioni_zapis[i].simbolRB;
    int dodatak = tr->relokacioni_zapis[i].dodatak;
    std::string simbol = tr->relokacioni_zapis[i].simbol;
    int duzina_imena_simbola = simbol.size();

    outputFileBinary.write((char*)(&offset), sizeof(int));
    outputFileBinary.write((char*)(&simbolRB), sizeof(int));
    outputFileBinary.write((char*)(&dodatak), sizeof(int));
    outputFileBinary.write((char*)(&duzina_imena_simbola), sizeof(int));
    outputFileBinary.write(simbol.c_str(), simbol.size());

    outputFile<<std::endl;
  }

  outputFile << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;

  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "BAZEN LITERALA";
  outputFile << std::endl;
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "ADRESA";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "VREDNOST"; 

  outputFile << std::endl;
  for (auto it = tr->promenljive_bazen.begin(); it != tr->promenljive_bazen.end(); it++){
    outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << it->first;
    if (it->second->e == Operand::literal) outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << it->second->vrednost;
    if (it->second->e == Operand::simbol) outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << it->second->simbol;
    
    outputFile << std::endl;
  }
}

void Asembler::dodajPodatakUKodSekcije(int& vrednost){

      
  trSekcija->kod_sekcije.push_back( vrednost & 0xFF);
  trSekcija->kod_sekcije.push_back( (vrednost >> 8) & 0xFF);
  trSekcija->kod_sekcije.push_back( (vrednost >> 16) & 0xFF); 
  trSekcija->kod_sekcije.push_back( (vrednost >> 24) & 0xFF);

  trSekcija->LC += 4;
}





int Asembler::dodajUBazenLiterala(int& vrednost){

  bool flag = false;

  for (auto it = trSekcija->promenljive_bazen.begin(); it != trSekcija->promenljive_bazen.end(); it++){
    if (it->second->vrednost == vrednost) return it->first;
  }

  if (!flag){ //literal ne postoji u bazenu

    std::pair<int, Argument*> novi_par;

    

    trSekcija->kod_sekcije.push_back(3<<4); 
    trSekcija->kod_sekcije.push_back(0);
    trSekcija->kod_sekcije.push_back(0);
    trSekcija->kod_sekcije.push_back(4);

    trSekcija->LC += 4;

    novi_par.first = trSekcija->LC ;
    novi_par.second = new Argument(Operand::literal, "", vrednost);

    trSekcija->promenljive_bazen.insert(novi_par);

    dodajPodatakUKodSekcije(vrednost);

    
  }

  return trSekcija->LC - 4;


}

void Asembler::literalVeliki(int gprA, int gprB, int gprC, int argument){


  int prviPrepravka = trSekcija->LC + 2;

      trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
      trSekcija->kod_sekcije.push_back(gprC<<4);
      trSekcija->kod_sekcije.push_back(0);

      trSekcija->LC +=4;

      int slInstrukcija = trSekcija->LC;

      int pomeraj = dodajUBazenLiterala(argument);

      pomeraj -= slInstrukcija;

      trSekcija->kod_sekcije[prviPrepravka] |= (pomeraj >> 8) & 0xF;
      trSekcija->kod_sekcije[prviPrepravka + 1] = (pomeraj & 0xFF);


}

void Asembler::literalMali(int gprA, int gprB, int gprC, int argument){

  

      trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
      trSekcija->kod_sekcije.push_back(gprC<<4 | ((argument >> 8) & 0xF));
      trSekcija->kod_sekcije.push_back(argument & 0xFF);

      trSekcija->LC += 4;

}

int Asembler::dodajUBazenLiteralaSimbol(std::string simbol){

  bool flag = false;

  for (auto it = trSekcija->promenljive_bazen.begin(); it != trSekcija->promenljive_bazen.end(); it++){
    if (it->second->simbol == simbol) return it->first;
  }

  if (!flag){ //literal/simbol ne postoji u bazenu

    std::pair<int, Argument*> novi_par;

    

    trSekcija->kod_sekcije.push_back(3<<4); 
    trSekcija->kod_sekcije.push_back(0);
    trSekcija->kod_sekcije.push_back(0);
    trSekcija->kod_sekcije.push_back(4);

    trSekcija->LC += 4;

    novi_par.first = trSekcija->LC ;
    novi_par.second = new Argument(Operand::simbol, simbol, 0);

    trSekcija->promenljive_bazen.insert(novi_par);

    int vrednost = 0;
    dodajPodatakUKodSekcije(vrednost);


    
  }

  return trSekcija->LC - 4;


}

std::vector<TabelaSimbolaUlaz>::iterator Asembler::dodajSimbolUTabeluSimbola(std::string simbol){

  auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
    
    if (it == tabelaSimbola.end()){ // simbol nije u tabeli
      int redniBroj = tabelaSimbola.size();
      tabelaSimbola.push_back(*(new TabelaSimbolaUlaz(simbol, 0 , redniBroj)));
    }
  
  it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
  return it;

}

int Asembler::simbolNedefinisan(std::string simbol,int gprA, int gprB, int gprC){
  int prviPrepravka = trSekcija->LC + 2;
        
  trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
  trSekcija->kod_sekcije.push_back(gprC<<4);
  trSekcija->kod_sekcije.push_back(0);


  trSekcija->LC +=4;

  int slInstrukcija = trSekcija->LC;

  int pomeraj = dodajUBazenLiteralaSimbol(simbol);


  pomeraj -= slInstrukcija;

  trSekcija->kod_sekcije[prviPrepravka] |= ((pomeraj >> 8) & 0xF);
  trSekcija->kod_sekcije[prviPrepravka + 1] = (pomeraj & 0xFF);

  return (pomeraj +=slInstrukcija);
}

void Asembler::simbolDefinisan(int gprA, int gprB, int gprC, std::vector<TabelaSimbolaUlaz>::iterator it){


    int disp = it->vrednost - trSekcija->LC - 4;  //-4 jer pc pokazuje na sledecu instrukciju

    trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
    trSekcija->kod_sekcije.push_back(gprC<<4 | ((disp >> 8) & 0xF));
    trSekcija->kod_sekcije.push_back(disp & 0xFF);

    trSekcija->LC += 4;

}

void Asembler::srediLokalneSimbole(){
  


  for (auto simbol = tabelaSimbola.begin(); simbol != tabelaSimbola.end(); simbol++){


    if (simbol->brSekcije > 0 && simbol->tip == "NOTYP"){
      
      trSekcija = sveSekcije.at(simbol->brSekcije);

      for (auto back = simbol->backpatch.begin(); back != simbol->backpatch.end(); back++){
        

        if (back->sekcija == trSekcija->brSekcije){
          int disp = simbol->vrednost - back->offset - 4; //-4 jer pokazuje pc na sledecu instrukciju
          
          if (back->instrukcija == "JMP") trSekcija->kod_sekcije[back->offset] ^= 0b1000; 
          if (back->instrukcija == "CALL") trSekcija->kod_sekcije[back->offset] ^= 0b0001;
          trSekcija->kod_sekcije[back->offset + 2] |= ((disp >> 8) & 0xF);
          trSekcija->kod_sekcije[back->offset + 3] = disp & 0xFF;

        }
      }

    }

    if (simbol->brSekcije == 0 && simbol->tip == "NOTYP" && simbol->vezivanje == "LOC" && simbol->redniBroj > 0){
      std::ostringstream oss; 
      oss << "Koriscen lokalni nedefnisani simbol: " << simbol->simbol;
      throw std::runtime_error(oss.str());
    }

  }

  std::vector<TabelaSimbolaUlaz>::iterator itSimbol;

  for (auto itSekcije = sveSekcije.begin(); itSekcije!= sveSekcije.end(); itSekcije++){

    for (auto itRelokacija = itSekcije->second->relokacioni_zapis.begin(); itRelokacija != itSekcije->second->relokacioni_zapis.end(); itRelokacija++){

      

          for (int i = 0; i < tabelaSimbola.size() ; i++)
            if (tabelaSimbola[i].redniBroj == itRelokacija->simbolRB && tabelaSimbola[i].redniBroj != tabelaSimbola[i].brSekcije)
              itSimbol = tabelaSimbola.begin() + i;
          
          
          
          if (itRelokacija->tip == "SYMBOL_MEM" && itSimbol->brSekcije != itSekcije->second->brSekcije){
            itRelokacija->tip = "ABS";
          } 


          if (itSimbol->vezivanje == "LOC") {
            itRelokacija->simbolRB = itSimbol->brSekcije;
            itRelokacija->dodatak = itSimbol->vrednost;
          }
      
      
    }


    itSekcije->second->relokacioni_zapis.erase(std::remove_if(itSekcije->second->relokacioni_zapis.begin(),itSekcije->second->relokacioni_zapis.end(), izbrisiRelokacioniZapis), itSekcije->second->relokacioni_zapis.end());

    
  } 

}




bool Asembler::izbrisiRelokacioniZapis(RelokacioniZapisUlaz zapis){
  return zapis.tip == "SYMBOL_MEM";
}

bool Asembler::izbrisiSimbolIzTabeleSimbola(TabelaSimbolaUlaz ulaz){
  return ulaz.vezivanje == "LOC" && ulaz.tip == "NOTYP";
}


void Asembler::relokacijaInstrukcije(int pomeraj, std::vector<TabelaSimbolaUlaz>::iterator it){

  auto iter = std::find(trSekcija->relokacioni_zapis.begin(),trSekcija->relokacioni_zapis.end(), pomeraj);

  if (iter == trSekcija->relokacioni_zapis.end()) trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(pomeraj, "ABS", it != tabelaSimbola.end() ? it->redniBroj : tabelaSimbola.size(), 0, it->simbol)));

  iter = std::find(trSekcija->relokacioni_zapis.begin(),trSekcija->relokacioni_zapis.end(), pomeraj); // ovo proveri zasto mora da se radi
  if (iter != trSekcija->relokacioni_zapis.end() && iter->tip == "SYMBOL_MEM") iter->tip = "ABS";
}

void Asembler::relokacijaSkok(int pomeraj, std::vector<TabelaSimbolaUlaz>::iterator it, int instrukcija){

  auto iter = std::find(trSekcija->relokacioni_zapis.begin(),trSekcija->relokacioni_zapis.end(), pomeraj);
  
  
  if (it->brSekcije == 0){

    

    if (adresiranje == Adresiranje::SKOK) it->backpatch.push_back(*(new BackpatchUlaz(trSekcija->brSekcije, instrukcija, "JMP")));
    if (adresiranje == Adresiranje::POZIV_POTPROGRAM) it->backpatch.push_back(*(new BackpatchUlaz(trSekcija->brSekcije, instrukcija, "CALL")));

    if (iter == trSekcija->relokacioni_zapis.end()) trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(pomeraj, "SYMBOL_MEM", it != tabelaSimbola.end() ? it->redniBroj : tabelaSimbola.size(), 0, it->simbol)));
  }

  else {
    if (iter == trSekcija->relokacioni_zapis.end()) trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(pomeraj, "ABS", it != tabelaSimbola.end() ? it->redniBroj : tabelaSimbola.size(), 0, it->simbol)));
  }
}

void Asembler::resiSimbolLiteral(int gprA, int gprB, int gprC, Argument* promenljiva){

      if (promenljiva->e == Operand::literal){

        int argument = promenljiva->vrednost;


        if (( argument > 2047) || (argument < -2048) ) { // literal veci od 12 bita
        
          
          if (adresiranje == Adresiranje::LD_IMMED){

            trSekcija->kod_sekcije[trSekcija->LC] |= 0b0010;
            literalVeliki(gprA,15,gprC,argument);
          } 

          if (adresiranje == Adresiranje::STORE_MEM_DIR){

            trSekcija->kod_sekcije[trSekcija->LC] |= 0b0010;
            literalVeliki(15,gprB,gprC,argument);
          } 

          if (adresiranje == Adresiranje::SKOK){
            literalVeliki(15, gprB, gprC, argument);
          }

          if (adresiranje == Adresiranje::POZIV_POTPROGRAM){
            trSekcija->kod_sekcije[trSekcija->LC] |= 0b0001;
            literalVeliki(15,gprB,gprC,argument);
          }                    
                    
        }


        else{ // literal manji od 12 bita

          if (adresiranje == Adresiranje::SKOK) {

            trSekcija->kod_sekcije[trSekcija->LC] ^= 0b1000; 
            literalMali(15,gprB,gprC,argument);
            return;
          }

          if (adresiranje == Adresiranje::POZIV_POTPROGRAM){

            trSekcija->kod_sekcije[trSekcija->LC] |= 0b0000;
            literalMali(15,gprB,gprC,argument);
            return;
          }

          if (adresiranje == Adresiranje::LD_IMMED) trSekcija->kod_sekcije[trSekcija->LC] |= 0b0001; 
          
          if (adresiranje == Adresiranje::STORE_MEM_DIR || adresiranje == Adresiranje::POZIV_POTPROGRAM) trSekcija->kod_sekcije[trSekcija->LC] |= 0b0000;// ne menjaj nista

          

          literalMali(gprA,gprB,gprC,argument);

        }


      }


      else { // ako je operand simbol
        
        std::string simbol = promenljiva->simbol;

        auto it = dodajSimbolUTabeluSimbola(simbol);

        int instrukcija = trSekcija->LC;
        int pomeraj;




        if (adresiranje == Adresiranje::LD_IMMED){

          trSekcija->kod_sekcije[trSekcija->LC] |= 0b0010;
          pomeraj = simbolNedefinisan(simbol, gprA , 15, gprC);
        } 




        if (adresiranje == Adresiranje::STORE_MEM_DIR){

          trSekcija->kod_sekcije[trSekcija->LC] |= 0b0010;
          pomeraj = simbolNedefinisan(simbol,15,gprB,gprC);
        }




        if (adresiranje == Adresiranje::SKOK || adresiranje == Adresiranje::POZIV_POTPROGRAM){


            if (adresiranje == Adresiranje::POZIV_POTPROGRAM) trSekcija->kod_sekcije[trSekcija->LC] |= 0b0001;

            if (it->brSekcije != trSekcija->brSekcije ){ //slucaj kada simbol nije definisan u mojo sekciji

              pomeraj = simbolNedefinisan(simbol,15, gprB, gprC);
              
              relokacijaSkok(pomeraj,it,instrukcija);            
              }

              else { // kada je simbol vec definisan u mojoj sekciji

                  if (adresiranje == Adresiranje::SKOK) trSekcija->kod_sekcije[trSekcija->LC] ^= 0b1000; 

                  simbolDefinisan(15, gprB, gprC, it);

              }
            return;
        }

        

        relokacijaInstrukcije(pomeraj,it);

      }

}



void Asembler::ST_REG_IND(int gprA, int gprC){

  trSekcija->kod_sekcije.push_back(gprA<<4);
  trSekcija->kod_sekcije.push_back(gprC<<4);
  trSekcija->kod_sekcije.push_back(0);

  zavrsiInstrukciju();
}


void Asembler::ST_REG_IND_POM(int gprA, int gprC, int disp, int& line_num){


  if (( disp > 2047) || (disp < -2048) ) {
   
    ispisiGresku(line_num);
   }

  trSekcija->kod_sekcije.push_back(gprA<<4);
  trSekcija->kod_sekcije.push_back(gprC<<4 | ( (disp>>8) & 0xF) );
  trSekcija->kod_sekcije.push_back(disp & 0xFF) ;


  zavrsiInstrukciju();
}

void Asembler::LD_REG_DIR(int gprA, int gprB){



  trSekcija->kod_sekcije[trSekcija->LC] |= 0b0001;

  trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);

  zavrsiInstrukciju();
}

void Asembler::LD_REG_IND(int gprA, int gprB){

  

  trSekcija->kod_sekcije[trSekcija->LC] |= 0b0010;


  trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);

  zavrsiInstrukciju();
}

void Asembler::LD_REG_IND_POM(int& line_num){



  trSekcija->kod_sekcije[trSekcija->LC] |= 0b0010;
  int gprB = argumenti[0]->vrednost;
  int gprA = argumenti[1]->vrednost;
  int disp = argumenti[2]->vrednost;

   if (( disp > 2047) || (disp < -2048) ) {
   
    ispisiGresku(line_num);
   }

  trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
  trSekcija->kod_sekcije.push_back( (disp>>8) & 0xF);
  trSekcija->kod_sekcije.push_back(disp & 0xFF);

  zavrsiInstrukciju();
}

void Asembler::LD_MEM_DIR(){

  adresiranje = LD_IMMED;
  int gprA = argumenti[1]->vrednost;
  Argument* promenljiva = argumenti[0];
  
  resiSimbolLiteral(gprA,0,0,promenljiva);


  trSekcija->kod_sekcije.push_back(0x92);
  trSekcija->kod_sekcije.push_back(gprA<<4|gprA);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);

  zavrsiInstrukciju();
}

void Asembler::napraviInstrukciju(Token_Instrukcija t, int brLinije, int gpr1, int gpr2, int gpr3, int csr){

  if (trSekcija == nullptr){  // instrukcija ne moze da stoji van sekcije
    ispisiGresku(brLinije);
  }
  
  switch (t)
  {
    case halt:{
      InstrukcijaHalt();
      break;
    }
      
    
    case Int:{
      InstrukcijaInt();
      break;
    } 

    case iret:{
      InstrukcijaIret();
      zavrsiInstrukciju();
      break;
    }

    case call:{
      InstrukcijaCall(brLinije);
      break;
    }

    case ret:{
      InstrukcijaRet();
      break;
    }

    case jmp:{
      trSekcija->kod_sekcije.push_back(3<<4|8);
      resiSimbolLiteral(0,0,0,argumenti[0]);
      break;
    }
    
    case add:{
      InstrukcijaAdd(gpr1, gpr2);
      break;
    }

    case sub:{
      InstrukcijaSub(gpr1, gpr2);
      break;
    }

    case mul:{
      InstrukcijaMul(gpr1, gpr2);
      break;
    }

    case Div:{
      InstrukcijaDiv(gpr1, gpr2);
      break;
    }

    case Not:{
      InstrukcijaNot(gpr1);
      break;
    }

    case Or:{
      InstrukcijaOr(gpr1, gpr2);
      break;
    }

    case Xor:{
      InstrukcijaXor(gpr1, gpr2);
      break;
    }

    case And:{
      InstrukcijaAnd(gpr1, gpr2);
      break;
    }

    case shl:{
      InstrukcijaShl(gpr1, gpr2);
      break;
    }

    case shr:{
      InstrukcijaShr(gpr1, gpr2);
      break;
    }

    case csrrd:{
      InstrukcijaCsrrd(gpr1, csr);
      break;
    }

    case csrwr:{
      InstrukcijaCsrwr(gpr1, csr);
      break;
    }

    case xchg:{
      InstrukcijaXchg(gpr1, gpr2);
    }

    case beq:{
      trSekcija->kod_sekcije.push_back(3<<4|9);
      std::cout << gpr1 << " " << gpr2 << std::endl;
      resiSimbolLiteral(0,gpr1,gpr2,argumenti[0]);
      break;
    }


    case bne:{
      trSekcija->kod_sekcije.push_back(3<<4|10);
      resiSimbolLiteral(0,gpr1,gpr2,argumenti[0]);
      std::cout << gpr1 << " " << gpr2 << std::endl;
      break;
    }


    case bgt:{
      trSekcija->kod_sekcije.push_back(3<<4|11);
      resiSimbolLiteral(0,gpr1,gpr2,argumenti[0]);;
      break;
    }


    case pop:{
      InstrukcijaPop(gpr1);
      break;
    }

    case push:{
      InstrukcijaPush(gpr1);
      break;
    }

    case st:{
      trSekcija->kod_sekcije.push_back(8<<4);
      InstrukcijaSt(brLinije);
      break;
    }

    case ld:{
      trSekcija->kod_sekcije.push_back(9<<4);
      InstrukcijaLd(brLinije);
      break;
    }

    
    
    default:
      break;
  }

  argumenti.clear();
  argumenti.shrink_to_fit();
}




void Asembler::ispisiIzlazneFajlove(){
  if (!outputFileName || !outputBinaryName) {
    std::cerr << "Both -o <output_file> and <input_file> must be specified." << std::endl;      
  }

  outputFile.open(outputFileName, std::ios::out | std::ios::app);
    
  outputFileBinary.open(outputBinaryName, std::ios::out | std::ios::binary);
    
  if (!outputFileName) {
      std::cerr << "I can't create " << outputFileName << "!" << std::endl;
      outputFile.close();
      outputFileBinary.close();
  }



  int broj_sekcija = sveSekcije.size();
  outputFileBinary.write((char*)(&broj_sekcija), sizeof(int) );   // ukupan broj sekcija


  for (const auto& sekcija : sveSekcije) {
      Asembler::ispisiKodSekcije(sekcija.second);
      outputFile << std::endl << std:: endl << std::endl << std:: endl;
  }

  outputFile << std::endl << std:: endl;

  int nameWidth = 20;
  const char separator = ' ';

  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "SIMBOL";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "BROJ SEKCIJE";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "VREDNOST";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "TIP";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "VEZIVANJE";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "BROJ";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "VELICINA";
  outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << "BACKPATCH";
  outputFile<<std::endl;
  

  for (const auto& ulaz : tabelaSimbola) {

        outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.simbol;
        outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.brSekcije;
        outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.vrednost;
        outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.tip;
        outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.vezivanje;
        outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.redniBroj;
        outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << ulaz.velicina;

        for (const auto& it : ulaz.backpatch){

          outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << it.sekcija;
          outputFile << std::left << std::setw(nameWidth) << std::setfill(separator) << it.offset;
          outputFile << std::endl;
        

        }
      outputFile << std::endl;

      
      
  }

  
  int brSekcija = sveSekcije.size();


  tabelaSimbola.erase(std::remove_if(tabelaSimbola.begin(), tabelaSimbola.end(), izbrisiSimbolIzTabeleSimbola), tabelaSimbola.end());



  int duzina_tabele_simbola = tabelaSimbola.size();
  outputFileBinary.write((char*)(&duzina_tabele_simbola), sizeof(int));

  for (int i = 0 ; i < tabelaSimbola.size(); i++){
    std::string ime_simbola = tabelaSimbola[i].simbol;
    int duzina_imena_simbola = ime_simbola.size();

    outputFileBinary.write((char*)(&duzina_imena_simbola), sizeof(int));
    outputFileBinary.write(ime_simbola.c_str(), duzina_imena_simbola);

    int broj_sekcije = tabelaSimbola[i].brSekcije;
    outputFileBinary.write((char*)(&broj_sekcije), sizeof(int));

    int vrednost = tabelaSimbola[i].vrednost;
    outputFileBinary.write((char*)(&vrednost), sizeof(int));

    int duzina_tipa = tabelaSimbola[i].tip.size();
    outputFileBinary.write((char*)(&duzina_tipa), sizeof(int));
    outputFileBinary.write(tabelaSimbola[i].tip.c_str(), duzina_tipa);

    int redniBroj = tabelaSimbola[i].redniBroj;
    outputFileBinary.write((char*)(&redniBroj), sizeof(int));

  }
}