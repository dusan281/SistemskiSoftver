#include "../inc/Emulator.hpp"

char* Emulator::inputFileName;
std::map<uint32_t,uint8_t> Emulator::mapa = {};
int Emulator::gprX[16]{0};
int Emulator::csrX[3]{0};



sem_t Emulator::sem;

void Emulator::pokreniEmulator(){


  
  incijalizujMemoriju();


  pokreniIzvrsavanje();
}






void Emulator::incijalizujMemoriju(){

  std::ifstream inputFileStream(inputFileName);

  std::string linija;

  while (getline(inputFileStream, linija)) {
       
        std::istringstream iss(linija);
        std::string word;

        
        int brojac = 0;
        std::string adresa_string;
        std::string podatak_string;
        while (iss >> word) {

          if (brojac == 0) adresa_string = word.substr(0,8);

          if (brojac > 0){
            podatak_string = word;
            uint32_t adresa = std::stoul(adresa_string,nullptr,16) + brojac - 1;
            uint8_t  podatak = std::stoul(podatak_string, nullptr, 16);

            mapa[adresa] = podatak;
          } 
          brojac++;
        }
    }

    // Close the file
    inputFileStream.close();
}


void Emulator::pokreniIzvrsavanje(){

  gprX[PC] = 0x40000000;


  procitajInstrukcije(gprX[PC]);

}



void Emulator::procitajInstrukcije(int& pc){


  

  uint8_t oc = OP_CODE::START;

  while (oc != OP_CODE::HALT){

    sem_wait(&sem);
  
    int stariPC = gprX[PC];
    gprX[PC] += 4;


      oc = (mapa.at(stariPC) >> 4) & 0xF;
      uint8_t mod = mapa.at(stariPC) &0xF;
      uint8_t regA = (mapa.at(stariPC + 1) >> 4) & 0xF;
      uint8_t regB = mapa.at(stariPC + 1) &0xF;
      uint8_t regC = (mapa.at(stariPC + 2) >> 4) & 0xF;
      int disp = ((mapa.at(stariPC + 2) &0xF) << 8) | mapa.at(stariPC + 3);

      if (disp & 0x800){
        disp = disp | 0xFFFFF000;
      }


      switch (oc)
      {
        case (OP_CODE::HALT):{
          ispisiRegistre();
          flag = true;
          sem_post(&sem);
          break;
        }

        case(OP_CODE::INT):{
          
          InstrukcijaSoftverskogPrekida();
          sem_post(&sem);         
          break;
        }

        case (OP_CODE::CALL):{

          InstrukcijaPozivaPotprograma(mod,regA,regB,regC,disp);
          sem_post(&sem);          
          break;
        }

        case (OP_CODE::JMP):{

          InstrukcijaSkoka(mod,regA,regB,regC,disp);
          sem_post(&sem);          
          break;
        }

        case (OP_CODE::EXCHANGE):{

          InstrukcijaZamene(regB,regC);
          sem_post(&sem);        
          break;
        }

        case (OP_CODE::ARITHMETIC):{

          InstrukcijaAritmetika(mod,regA,regB,regC,disp);
          sem_post(&sem);          
          break;
        }
          
        case(OP_CODE::LOGIC):{

          InstrukcijaLogika(mod,regA,regB,regC,disp);
          sem_post(&sem);          
          break;
        }

        case(OP_CODE::SHIFT):{

          InstrukcijaShift(mod,regA,regB,regC,disp);
          sem_post(&sem);          
          break;
        }

        case (OP_CODE::STORE):{

          InstrukcijaStore(mod,regA,regB,regC,disp);
          sem_post(&sem);        
          break;
        }
          
        case (OP_CODE::LOAD):{

          InstrukcijaLoad(mod,regA,regB,regC,disp);
          sem_post(&sem);
          break;
        }
        
          break;
      }

  }
  
  
  
}






















void Emulator::InstrukcijaSoftverskogPrekida(){

  gprX[SP] -= 4;
  uint32_t adresa = gprX[SP];
  int podatak = csrX[STATUS];    //smestanje statusa na stek
  smestiPodatakUMemoriju(adresa,podatak);
  

  gprX[SP] -= 4;

  adresa = gprX[SP];
  podatak = gprX[PC];   //smestanje pc na stek
  smestiPodatakUMemoriju(adresa,podatak);

  csrX[CAUSE] = 0x4;
  csrX[STATUS] |= 0x4;

  gprX[PC] = csrX[HANDLER];

}





void Emulator::InstrukcijaPrekidTerminal(){

  gprX[SP] -= 4;
  uint32_t adresa = gprX[SP];
  int podatak = csrX[STATUS];
  smestiPodatakUMemoriju(adresa,podatak);
  

  gprX[SP] -= 4;

  adresa = gprX[SP];
  podatak = gprX[PC];
  smestiPodatakUMemoriju(adresa,podatak);

  csrX[CAUSE] = 0x3;
  csrX[STATUS] |= 0x4;

  gprX[PC] = csrX[HANDLER];

}





void Emulator::InstrukcijaPozivaPotprograma(int mod, int regA, int regB, int regC, int disp){

  gprX[SP] -= 4;

  uint32_t adresa = gprX[SP];
  int podatak = gprX[PC];

  smestiPodatakUMemoriju(adresa,podatak);
  

  switch (mod)
  {
      case (0):{
        
        gprX[PC] = gprX[regA] + gprX[regB] + disp;
        break;
      }

      case(1):{

        gprX[PC] = procitajIzMemorije(regA, regB, regC, disp);
        break;
      }  
      
      default:
        break;
      }
}




void Emulator::InstrukcijaSkoka(int mod, int regA, int regB, int regC, int disp){

    switch (mod)
    {
        case (0):{

          gprX[PC] = gprX[regA] + disp;
          break;
        }
          
        
        case(1):{
 
          if (gprX[regB] == gprX[regC]) gprX[PC] = gprX[regA] + disp; 
          break;
        }

        case(2):{
          if (gprX[regB] != gprX[regC]) gprX[PC] = gprX[regA] + disp; 
          break;
        }
          
        
        case(3):{
          if (gprX[regB] > gprX[regC]) gprX[PC] = gprX[regA] + disp; 
          break;
        }

        case(4):{
          gprX[PC] = procitajIzMemorije(regA, regB, regC, disp);
          break;
        }

        case(5):{
          if (gprX[regB] == gprX[regC]) gprX[PC] = procitajIzMemorije(regA, regB, regC, disp);
          break;
        }

        case(6):{
          if (gprX[regB] != gprX[regC]) gprX[PC] = procitajIzMemorije(regA, regB, regC, disp);
          break;
        }
          
        
        case(7):{
          if (gprX[regB] > gprX[regC]) gprX[PC] = procitajIzMemorije(regA, regB, regC, disp);
          break;
        }

        default:
          break;
        }
}





void Emulator::InstrukcijaZamene(int regB, int regC){

  int temp = gprX[regB];
  gprX[regB] = gprX[regC];
  gprX[regC] = temp;
}

void Emulator::InstrukcijaAritmetika(int mod, int regA, int regB, int regC, int disp){

  switch (mod)
  {
        case (0):{

          gprX[regA] = gprX[regB] + gprX[regC];
          break;
        }
          
        
        case(1):{
          gprX[regA] = gprX[regB] - gprX[regC];
          break;
        }

        case(2):{
          gprX[regA] = gprX[regB] * gprX[regC];
          break;
        }
          
        
        case(3):{
          gprX[regA] = gprX[regB] / gprX[regC];
          break;
        }
        
        default:
          break;
  }
}





void Emulator::InstrukcijaLogika(int mod, int regA, int regB, int regC, int disp){
  switch (mod)
  {
        case (0):{

          gprX[regA] = ~gprX[regB];
          break;
        }
          
        
        case(1):{
          gprX[regA] = gprX[regB] & gprX[regC];
          break;
        }

        case(2):{
          gprX[regA] = gprX[regB] | gprX[regC];
          break;
        }
          
        
        case(3):{
          gprX[regA] = gprX[regB] ^ gprX[regC];
          break;
        }
        
        default:
          break;
  }
}






void Emulator::InstrukcijaShift(int mod, int regA, int regB, int regC, int disp){

  switch (mod)
  {
        case (0):{

          gprX[regA] = gprX[regB] << gprX[regC];
          break;
        }
          
        
        case(1):{
          gprX[regA] = gprX[regB] >> gprX[regC];
          break;
        }
        
        default:
          break;
  }


}






void Emulator::InstrukcijaStore(int mod, int regA, int regB, int regC, int disp){

  uint32_t adresa;
  uint32_t podatak;

  switch (mod)
  {
        case (0):{

          adresa = gprX[regA] + gprX[regB] + disp;
          podatak = gprX[regC];

          smestiPodatakUMemoriju(adresa,podatak);
          break;
        }

        case(1):{

          gprX[regA] += disp;
          adresa = gprX[regA];
          podatak = gprX[regC];

          smestiPodatakUMemoriju(adresa,podatak);
          break;
        }
          
        
        case(2):{
          int regAA = procitajIzMemorije(regA,regB,0,disp);
          adresa = regAA;
          podatak = gprX[regC];
          smestiPodatakUMemoriju(adresa,podatak);
          break;
        }
        
        default:
          break;
  }

 

  if (adresa == 0xFFFFFF00){
    std::cout << std::hex << mapa[0xFFFFFF00] << std::flush;
  }
}



void Emulator::InstrukcijaLoad(int mod, int regA, int regB, int regC, int disp){

  switch (mod)
  {
        case (0):{

          gprX[regA] = csrX[regB];
          break;
        }

        case(1):{

          gprX[regA] = gprX[regB] + disp;
          break;
          
        }
          
        
        case(2):{

          gprX[regA] = int(procitajIzMemorije(0,regB,regC,disp));
          break;
        }

        case(3):{

          gprX[regA] = int (procitajIzMemorije(0,regB,0,0));
          gprX[regB] += disp;
          break;
        }

        case(4):{

          csrX[regA] = gprX[regB];
          break;
        }

        case(6):{

          csrX[regA] = procitajIzMemorije(0,regB,regC,disp);
          break;
        }


        
        default:
          break;
  }
}




uint32_t Emulator::procitajIzMemorije(int regA, int regB, int regC, int disp){

  uint32_t adresa = gprX[regA] + gprX[regB] + gprX[regC] + disp ;

  uint8_t prviBajt = mapa[gprX[regA] + gprX[regB] + gprX[regC] + disp + 3];
  uint8_t drugiBajt = mapa[gprX[regA] + gprX[regB] + gprX[regC] + disp + 2];
  uint8_t treciBajt = mapa[gprX[regA] + gprX[regB] + gprX[regC] + disp + 1];
  uint8_t cetvrtiBajt = mapa[gprX[regA] + gprX[regB] + gprX[regC] + disp + 0];

  uint32_t podatak = (prviBajt << 24) | (drugiBajt << 16) | (treciBajt << 8) | cetvrtiBajt;
  return podatak;

}



void Emulator::smestiPodatakUMemoriju(uint32_t adresa, int podatak){

  mapa[adresa] = podatak & 0xFF;
  mapa[adresa + 1] = (podatak >> 8) & 0xFF;
  mapa[adresa + 2] = (podatak >> 16) & 0xFF;
  mapa[adresa + 3] = (podatak >> 24) & 0xFF;

}



void Emulator::ispisiRegistre(){

  std::cout << std::endl << "-----------------------------------------------------------------" << std::endl;
  std::cout << "Emulated processor executed halt instruction" << std::endl;
  std::cout << "Emulated processor state:" << std::endl;
  for(int i = 0; i <= 15;i++){
    if(i>0 && i%4 == 0){
      std::cout << std::endl;
    }
    std::cout << "\tr" << std::dec << i << ":0x" << std::hex << std::setfill('0') << std::setw(8) << gprX[i];
  }

  std::cout << std::endl;
}




void Emulator::postaviNonCanonicalMode(struct termios &oldt) {

  struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);  // Dobij trenutna svojstva terminala
    newt = oldt;

    // Isključi kanonski mod i eho
    newt.c_lflag &= ~(ICANON | ECHO);

    // Primeni nova svojstva terminala
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

}

void Emulator::vratiCanonicalMode(const struct termios &oldt) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Vrati terminal u prvobitno stanje
}






void Emulator::nitTerminal(){
  
  struct termios oldt;
    postaviNonCanonicalMode(oldt);
   
    char c;
    while (!flag){

      c = getchar();  // Čitanje pritisnutog tastera  

      while(csrX[STATUS] == 4){ // cekaj dok je prekid u toku
        
      }

      sem_wait(&sem);

      
      mapa[0xFFFFFF04] = c;
      mapa[0xFFFFFF05] = mapa[0xFFFFFF06] = mapa[0xFFFFFF07] = 0;
     


      InstrukcijaPrekidTerminal();
      
      sem_post(&sem);
    }
    

    vratiCanonicalMode(oldt);  // Vrati terminal u kanonski mod
}





