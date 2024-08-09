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


void Asembler::InstrukcijaBeq(int gpr1, int gpr2){

  InstrukcijaSkoka(1, gpr1, gpr2);
}



void Asembler::InstrukcijaBne(int gpr1, int gpr2){

  InstrukcijaSkoka(2, gpr1, gpr2);
}




void Asembler::InstrukcijaBgt(int gpr1, int gpr2){

  InstrukcijaSkoka(3, gpr1, gpr2);
}








void Asembler::InstrukcijaCall(int& line_num){

  InstrukcijaPush(15);
  trSekcija->kod_sekcije.push_back(3<<4|8);
  InstrukcijaJmp(line_num);

}













void Asembler::InstrukcijaJmp(int& line_num){

  InstrukcijaSkoka(0, 0, 0);
  
}
















void Asembler::InstrukcijaSkoka(int a, int gpr1, int gpr2){

  if (argumenti[0]->e == Operand::literal){
    

    int argument = argumenti[0]->vrednost;

    if (( argument > 2047) || (argument < -2048) ) { // literal veci od 12 bita

      int prviPrepravka = trSekcija->LC + 2;

      trSekcija->kod_sekcije.push_back(15<<4|gpr1);
      trSekcija->kod_sekcije.push_back(gpr2<<4);
      trSekcija->kod_sekcije.push_back(0);

      trSekcija->LC +=4;

      int slInstrukcija = trSekcija->LC;

      int pomeraj = dodajUBazenLiterala(argument);

      pomeraj -= slInstrukcija;

      trSekcija->kod_sekcije[prviPrepravka] |= (pomeraj >> 8) & 0xF;
      trSekcija->kod_sekcije[prviPrepravka + 1] = (pomeraj & 0xFF);
     
    } 


    else{ // literal nije veci od 12 bita

      trSekcija->kod_sekcije[trSekcija->LC] ^= 0b1000; 

      trSekcija->kod_sekcije.push_back(15<<4|gpr1);
      trSekcija->kod_sekcije.push_back(gpr2<<4 | ((argument >> 8) & 0xF));
      trSekcija->kod_sekcije.push_back(argument & 0xFF);

      trSekcija->LC += 4;
    }


  }













  else { // ako je operand simbol

    std::string simbol = argumenti[0]->simbol;

    auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
    
    if (it == tabelaSimbola.end()){ // simbol nije u tabeli
      int redniBroj = tabelaSimbola.size();
      tabelaSimbola.push_back(*(new TabelaSimbolaUlaz(simbol, 0 , redniBroj, 0)));
    }

    int instrukcija = trSekcija->LC;

    it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);

    if (it == tabelaSimbola.end() || ( it != tabelaSimbola.end() && it->brSekcije != trSekcija->brSekcije)){ //slucaj kada simbol nije definisan u mojo sekciji

        simbolNedefinisan(simbol,15, gpr1, gpr2, it);

        trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(trSekcija->LC - 4, "BACKPATCH", it != tabelaSimbola.end() ? it->redniBroj : tabelaSimbola.size(), 0)));

        if (it->brSekcije != -1){
          it->backpatch.push_back({trSekcija->brSekcije, instrukcija});
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
            
            int gprB = argumenti[0]->vrednost;
            int gprA = argumenti[1]->vrednost;

            trSekcija->kod_sekcije[trSekcija->LC] |= 0b0001;

            trSekcija->kod_sekcije.push_back(gprA<<4|gprB);
            trSekcija->kod_sekcije.push_back(0);
            trSekcija->kod_sekcije.push_back(0);

            zavrsiInstrukciju();
            break;
          }


          case Adresiranje::IMMED:{
            
            int gprA = argumenti[1]->vrednost;

            if (argumenti[0]->e == Operand::literal){

              int argument = argumenti[0]->vrednost;


              if (( argument > 2047) || (argument < -2048) ) { // literal veci od 12 bita
              
                
                trSekcija->kod_sekcije[trSekcija->LC] |= 0b0010;
                trSekcija->kod_sekcije.push_back(gprA<<4);

                literalVelikiInstrukcija(15,argument);
                
              }


              else{ // literal manji od 12 bita

                trSekcija->kod_sekcije[trSekcija->LC] |= 0b0001;
                trSekcija->kod_sekcije.push_back(gprA<<4);

                literalMaliInstrukcija(0,argument);

              }


            }


            else { // ako je operand simbol

              std::string simbol = argumenti[0]->simbol;

              trSekcija->kod_sekcije[trSekcija->LC] |= 0b0010;

              auto it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);
              
              if (it == tabelaSimbola.end()){ // simbol nije u tabeli
                int redniBroj = tabelaSimbola.size();
                tabelaSimbola.push_back(*(new TabelaSimbolaUlaz(simbol, 0 , redniBroj, 0)));
              }

              int instrukcija = trSekcija->LC;

              it = std::find(tabelaSimbola.begin(),tabelaSimbola.end(), simbol);

              simbolNedefinisan(simbol, gprA , 15, 0, it);

              bool flag = false;
              for (int i = 0 ; i < trSekcija->relokacioni_zapis.size(); i++){
                if (trSekcija->relokacioni_zapis[i].simbolRB == it->redniBroj) flag = true;
              }

              if (!flag){ 

                  trSekcija->relokacioni_zapis.push_back(*(new RelokacioniZapisUlaz(trSekcija->LC - 4, "ABS", it != tabelaSimbola.end() ? it->redniBroj : tabelaSimbola.size(), 0)));

              }

          }
          }
            /* code */
          
          default:
            break;
        }


}






void Asembler::zavrsiInstrukciju(){
    trSekcija->LC += 4;
}
