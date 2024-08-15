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
  trSekcija->kod_sekcije.push_back(0 << 14);
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
  trSekcija->kod_sekcije.push_back(gpr1<<4);
  trSekcija->kod_sekcije.push_back(4);
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

  
  trSekcija->kod_sekcije.push_back(2<<4);
  adresiranje = Adresiranje::POZIV_POTPROGRAM;
  resiSimbolLiteral(0,0,0,argumenti[0]);

}

void Asembler::InstrukcijaLd(int& line_num){

 

  switch (adresiranje)
        {
          case (Adresiranje::REG_DIR):{
            
            int gprB = argumenti[0]->vrednost;
            int gprA = argumenti[1]->vrednost;
            LD_REG_DIR(gprA,gprB);
            break;
          }


          case Adresiranje::IMMED:{
            
            adresiranje = LD_IMMED;
            int gprA = argumenti[1]->vrednost;
            Argument* promenljiva = argumenti[0];

            resiSimbolLiteral(gprA,0,0,promenljiva);
            break;
          }
          
          
          case Adresiranje::MEM_DIR:{

            adresiranje = LD_IMMED;
            int gprA = argumenti[1]->vrednost;
            Argument* promenljiva = argumenti[0];
            
            resiSimbolLiteral(gprA,0,0,promenljiva);

            trSekcija->kod_sekcije.push_back(0x90); // moram jer ovako zapocinjem novu masinsku instrukciju

            LD_REG_IND(gprA,gprA);
            break;
          }

          case Adresiranje::REG_IND:{

            int gprB = argumenti[0]->vrednost;
            int gprA = argumenti[1]->vrednost;
            LD_REG_IND(gprA,gprB);
            break;
          }

          case Adresiranje::REG_IND_POM:{

            
            LD_REG_IND_POM(line_num);
            break;
          }
          

          default:
            break;
        }


}

void Asembler::InstrukcijaSt(int& line_num){

  int gprC = argumenti[0]->vrednost;

  switch (adresiranje)
        {
          case (Adresiranje::REG_DIR):{
            

            int gprA = argumenti[1]->vrednost;
            LD_REG_DIR(gprA,gprC);
            break;
          }

          case Adresiranje::REG_IND:{

            int gprA = argumenti[1]->vrednost;
            ST_REG_IND(gprA,gprC);
            break;
          }

          case Adresiranje::REG_IND_POM:{

            int gprA = argumenti[1]->vrednost;
            int disp = argumenti[2]->vrednost;

            ST_REG_IND_POM(gprA,gprC,disp,line_num);
            break;
          }
          

          case Adresiranje::MEM_DIR:{

            adresiranje = STORE_MEM_DIR;
            Argument* promenljiva = argumenti[1];
            
            resiSimbolLiteral(0,0,gprC,promenljiva);
            break;
          }

          default:
            break;
        }


}






void Asembler::zavrsiInstrukciju(){
    trSekcija->LC += 4;
}
