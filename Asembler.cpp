#include "Asembler.hpp"


std::vector<Argument*> Asembler::argumenti = {};
Sekcija* Asembler::trSekcija = nullptr;
std::map<int, Sekcija*> Asembler::sveSekcije = {};
std::vector<TabelaSimbolaUlaz> Asembler::tabelaSimbola = {*(new TabelaSimbolaUlaz("",0,0))};

Adresiranje Asembler::adresiranje = Adresiranje::MEM_DIR;

void Asembler::postaviAdresiranje(enum Adresiranje a){
  Asembler::adresiranje = a;
}









void Asembler::dodajUListuArgumenata(Argument* arg){
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
  std::cout << "Nalazim se u sekciji velicine " << tr->LC << std::endl ;
  for (int i = 0 ; i < tr->kod_sekcije.size(); i++){
    if (i % 4 == 0) std::cout<<std::endl;
    std::cout << std::hex << static_cast<int>(tr->kod_sekcije[i]) << " ";
    
  }
  std::cout << std::endl;

  std::cout << " RELOKACIONI ZAPIS"<<std::endl;
  
  std::cout << "OFFSET    TIP   SIMBOL    DODATAK" << std::endl;

  for (int i = 0; i < tr->relokacioni_zapis.size(); i++){
    std::cout<< tr->relokacioni_zapis[i].offset << " " << tr->relokacioni_zapis[i].tip << " " << tr->relokacioni_zapis[i].simbolRB << " " << tr->relokacioni_zapis[i].dodatak << std::endl;
  }

  std::cout << std::endl;

  std::cout << " BAZEN LITERALA"<<std::endl;
  std::cout << " Vrednost" << std::endl;

  for (auto it = tr->promenljive_bazen.begin(); it != tr->promenljive_bazen.end(); it++){
    std::cout<< it->first << " " << it->second->vrednost << std::endl;
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




void Asembler::literalVelikiSkok(int gpr2, int& argument){


  int prviPrepravka = trSekcija->LC + 2;

  trSekcija->kod_sekcije.push_back(gpr2<<4);
  trSekcija->kod_sekcije.push_back(0);

  trSekcija->LC +=4;

  int slInstrukcija = trSekcija->LC;

  int pomeraj = dodajUBazenLiterala(argument);
  

  pomeraj -= slInstrukcija;

  trSekcija->kod_sekcije[prviPrepravka] |= (pomeraj >> 8) & 0xF;
  trSekcija->kod_sekcije[prviPrepravka + 1] = (pomeraj & 0xFF);


}



void Asembler::literalMaliSkok(int& gpr2, int& argument){

  trSekcija->kod_sekcije[trSekcija->LC] ^= 0b1000; 

  trSekcija->kod_sekcije.push_back(gpr2<<4 | ((argument >> 8) & 0xF));
  trSekcija->kod_sekcije.push_back(argument & 0xFF);

  trSekcija->LC += 4;

}



void Asembler::literalVelikiInstrukcija(int gpr2, int& argument){

  // std::cout << gpr2 << " " << argument << std::endl;
  // std::cout << gpr2 << " " << trSekcija->LC << std::endl;

  trSekcija->kod_sekcije[trSekcija->LC + 1] |= gpr2;
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);

  

  int prviPrepravka = trSekcija->LC + 2;

  trSekcija->LC +=4;

  int slInstrukcija = trSekcija->LC;

  int pomeraj = dodajUBazenLiterala(argument);

  pomeraj -= slInstrukcija;

  trSekcija->kod_sekcije[prviPrepravka] |= (pomeraj >> 8) & 0xF;
  trSekcija->kod_sekcije[prviPrepravka + 1] = (pomeraj & 0xFF);


}


void Asembler::literalMaliInstrukcija(int gpr2, int& argument){

  trSekcija->kod_sekcije[trSekcija->LC + 1] |= gpr2;
  trSekcija->kod_sekcije.push_back((argument >> 8) & 0xF);
  trSekcija->kod_sekcije.push_back((argument & 0xFF));

  trSekcija->LC += 4;

}









int Asembler::dodajUBazenLiteralaSimbol(std::string simbol,std::vector<TabelaSimbolaUlaz>::iterator it){

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





void Asembler::simbolNedefinisan(std::string simbol,int gprA, int gprB, int gprC, std::vector<TabelaSimbolaUlaz>::iterator it){
  int prviPrepravka = trSekcija->LC + 2;

        
  trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
  trSekcija->kod_sekcije.push_back(gprC<<4);
  trSekcija->kod_sekcije.push_back(0);

  trSekcija->LC +=4;

  int slInstrukcija = trSekcija->LC;

  int pomeraj = dodajUBazenLiteralaSimbol(simbol,it);

  pomeraj -= slInstrukcija;

  trSekcija->kod_sekcije[prviPrepravka] |= (pomeraj >> 8) & 0xF;
  trSekcija->kod_sekcije[prviPrepravka + 1] = (pomeraj & 0xFF);
}






void Asembler::simbolDefinisan(std::string simbol,int gprA, int gprB, int gprC, std::vector<TabelaSimbolaUlaz>::iterator it){


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

        

        if (back->first == trSekcija->brSekcije){
          int disp = simbol->vrednost - back->second - 4; //-4 jer pokazuje pc na sledecu instrukciju
          
          
          trSekcija->kod_sekcije[back->second] ^= 0b1000; 
          trSekcija->kod_sekcije[back->second + 2] |= (disp >> 8) & 0xF;
          trSekcija->kod_sekcije[back->second + 3] |= disp & 0xFF;

          for (int i = 0; i < trSekcija->relokacioni_zapis.size(); i++){
            //if (simbol->redniBroj == trSekcija->relokacioni_zapis[i].simbolRB && trSekcija->relokacioni_zapis[i].tip == "BACKPATCH") trSekcija->relokacioni_zapis.erase(trSekcija->relokacioni_zapis.begin() + i); 
            
          }

        }

        
        

      }

    }

    if (simbol->brSekcije == 0 && simbol->tip == "NOTYP" && simbol->vezivanje == "LOC" && simbol->redniBroj > 0){
      std::ostringstream oss; 
      oss << "Koriscen lokalni nedefnisani simbol: " << simbol->simbol;
      throw std::runtime_error(oss.str());
    }

  }
}









void Asembler::napraviInstrukciju(Token_Instrukcija t, int brLinije, int gpr1, int gpr2, int csr){

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
      InstrukcijaJmp(brLinije);
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
      InstrukcijaBeq(gpr1, gpr2);
      break;
    }


    case bne:{
      trSekcija->kod_sekcije.push_back(3<<4|10);
      InstrukcijaBne(gpr1, gpr2);
      break;
    }


    case bgt:{
      trSekcija->kod_sekcije.push_back(3<<4|11);
      InstrukcijaBgt(gpr1, gpr2);
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