#include "Asembler.hpp"

void Asembler::InstrukcijaHalt(){
  
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaInt(){
  
  trSekcija->kod_sekcije.push_back(16);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}


void Asembler::InstrukcijaIret(){

// sp = sp + 8

  trSekcija->kod_sekcije.push_back(9<<4|1);
  trSekcija->kod_sekcije.push_back(14<<4|14);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(8);  

// status = mem[sp-4]
  
  trSekcija->kod_sekcije.push_back(9<<4|6);
  trSekcija->kod_sekcije.push_back(14);
  trSekcija->kod_sekcije.push_back(15);
  trSekcija->kod_sekcije.push_back(15<<4|12);

//  pc = mem[sp-8]
  trSekcija->kod_sekcije.push_back(9<<4|2);
  trSekcija->kod_sekcije.push_back(15<<4|14);
  trSekcija->kod_sekcije.push_back(15);
  trSekcija->kod_sekcije.push_back(15<<4|8);

  trSekcija->LC += 8;
}


void Asembler::InstrukcijaRet(){
  trSekcija->kod_sekcije.push_back(9<<4|3);
  trSekcija->kod_sekcije.push_back(15<<4|14);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(4);
  zavrsiInstrukciju();
}



void Asembler::InstrukcijaAdd(int gpr1, int gpr2){
  
  trSekcija->kod_sekcije.push_back(80);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaSub(int gpr1, int gpr2){
  
  trSekcija->kod_sekcije.push_back(81);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaMul(int gpr1, int gpr2){
  
  trSekcija->kod_sekcije.push_back(82);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaDiv(int gpr1, int gpr2){
  
  trSekcija->kod_sekcije.push_back(83);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaNot(int gpr1){
  
  trSekcija->kod_sekcije.push_back(96);
  trSekcija->kod_sekcije.push_back(gpr1<<4|gpr1);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaAnd(int gpr1, int gpr2){
  
  trSekcija->kod_sekcije.push_back(97);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaOr(int gpr1, int gpr2){
  
  trSekcija->kod_sekcije.push_back(98);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaXor(int gpr1, int gpr2){
  
  trSekcija->kod_sekcije.push_back(99);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaXchg(int gpr1, int gpr2){
  
  trSekcija->kod_sekcije.push_back(64);
  trSekcija->kod_sekcije.push_back(gpr1);
  trSekcija->kod_sekcije.push_back(gpr2<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaShl(int gpr1, int gpr2){
  trSekcija->kod_sekcije.push_back(7<<4);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaShr(int gpr1, int gpr2){
  trSekcija->kod_sekcije.push_back(7<<4|1);
  trSekcija->kod_sekcije.push_back(gpr2<<4|gpr2);
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaPop(int gpr1){
  trSekcija->kod_sekcije.push_back(147);
  trSekcija->kod_sekcije.push_back(gpr1<<4|14);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(4);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaPush(int gpr1){
  trSekcija->kod_sekcije.push_back(129);
  trSekcija->kod_sekcije.push_back(14<<4);
  trSekcija->kod_sekcije.push_back(gpr1<<4|15);
  trSekcija->kod_sekcije.push_back(15<<4|12);
  zavrsiInstrukciju();
}


void Asembler::InstrukcijaCsrrd(int gpr1, int csr){ // gpr <= csr
  trSekcija->kod_sekcije.push_back(9<<4);
  trSekcija->kod_sekcije.push_back(gpr1<<4|csr);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}

void Asembler::InstrukcijaCsrwr(int gpr1, int csr){ // csr <= gpr
  trSekcija->kod_sekcije.push_back(9<<4|4);
  trSekcija->kod_sekcije.push_back(csr<<4|gpr1);
  trSekcija->kod_sekcije.push_back(0);
  trSekcija->kod_sekcije.push_back(0);
  zavrsiInstrukciju();
}




void Asembler::InstrukcijaCall(int& line_num){

  InstrukcijaPush(15);
  trSekcija->kod_sekcije.push_back(3<<4|8);
  InstrukcijaSkoka(0,0);

}


















void Asembler::InstrukcijaSkoka(int gpr1, int gpr2){

  if (argumenti[0]->e == Operand::literal){
    

    int argument = argumenti[0]->vrednost;

    if (( argument > 2047) || (argument < -2048) ) { // literal veci od 12 bita

      literalVeliki(15, gpr1, gpr2, argument);
     
    } 


    else{ // literal nije veci od 12 bita

      trSekcija->kod_sekcije[trSekcija->LC] ^= 0b1000; 
      literalMali(0, gpr1, gpr2, argument);
    }


  }

  else { // ako je operand simbol

    std::string simbol = argumenti[0]->simbol;

    auto it = dodajSimbolUTabeluSimbola(simbol); //imam iterator koji pokazuje na simbol u tabeli simbola

    


    if (it->brSekcije != trSekcija->brSekcije ){ //slucaj kada simbol nije definisan u mojo sekciji

        

        int pomeraj = simbolNedefinisan(simbol,15, gpr1, gpr2);


        
        auto iter = std::find(trSekcija->relokacioni_zapis.begin(),trSekcija->relokacioni_zapis.end(), pomeraj);
        int instrukcija = trSekcija->LC;
        
        
        if (it->brSekcije == 0){

          

          it->backpatch.push_back({trSekcija->brSekcije, instrukcija});
          if (iter == trSekcija->relokacioni_zapis.end()) trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(pomeraj, "SYMBOL_MEM", it != tabelaSimbola.end() ? it->redniBroj : tabelaSimbola.size(), 0)));
        }

        else {
          if (iter == trSekcija->relokacioni_zapis.end()) trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(pomeraj, "ABS", it != tabelaSimbola.end() ? it->redniBroj : tabelaSimbola.size(), 0)));
        }

        
    }

    else { // kada je simbol vec definisan u mojoj sekciji

        trSekcija->kod_sekcije[trSekcija->LC] ^= 0b1000; 

        simbolDefinisan(simbol,15, gpr1, gpr2, it);

    }


    





  
  }


  argumenti.clear();
  argumenti.shrink_to_fit();
}





void Asembler::InstrukcijaLd(int& line_num){

 

  switch (adresiranje)
        {
          case (Adresiranje::REG_DIR):{
            
            LD_REG_DIR();
            break;
          }


          case Adresiranje::IMMED:{
            
            LD_IMMED();
            break;
          }
            /* code */
          
          case Adresiranje::MEM_DIR:{

            LD_MEM_DIR();
            break;
          }

          case Adresiranje::REG_IND:{

            LD_REG_IND();
          }

          case Adresiranje::REG_IND_POM:{

            LD_REG_IND_POM(line_num);
          }
          

          default:
            break;
        }


}






void Asembler::zavrsiInstrukciju(){
    trSekcija->LC += 4;
}
