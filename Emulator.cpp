#include "Emulator.hpp"

char* Emulator::inputFileName;
std::map<uint32_t,uint8_t> Emulator::mapa = {};
int Emulator::gprX[16]{0};
int Emulator::csrX[3]{0};


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

  gprX[15] = 0x40000000;

  procitajInstrukciju(gprX[15]);

}



void Emulator::procitajInstrukciju(int& pc){

  int stariPC = pc;
  pc += 4;

  
  uint8_t oc = (mapa.at(stariPC) >> 4) & 0xF;
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
      break;
    }

    case(OP_CODE::INT):{
      
      InstrukcijaSoftverskogPrekida();
      procitajInstrukciju(gprX[15]);
      break;
    }

    case (OP_CODE::CALL):{

      InstrukcijaPozivaPotprograma(mod,regA,regB,regC,disp);
      procitajInstrukciju(gprX[15]);
      break;
    }

    case (OP_CODE::JMP):{

      InstrukcijaSkoka(mod,regA,regB,regC,disp);
      procitajInstrukciju(gprX[15]);
      break;
    }

    case (OP_CODE::EXCHANGE):{

      InstrukcijaZamene(regB,regC);
      procitajInstrukciju(pc);
      break;
    }

    case (OP_CODE::ARITHMETIC):{

      InstrukcijaAritmetika(mod,regA,regB,regC,disp);
      procitajInstrukciju(pc);
      break;
    }
      
    case(OP_CODE::LOGIC):{

      InstrukcijaLogika(mod,regA,regB,regC,disp);
      procitajInstrukciju(pc);
      break;
    }

    case(OP_CODE::SHIFT):{

      InstrukcijaShift(mod,regA,regB,regC,disp);
      procitajInstrukciju(pc);
      break;
    }

    case (OP_CODE::STORE):{

      InstrukcijaStore(mod,regA,regB,regC,disp);
      procitajInstrukciju(gprX[15]);
      break;
    }
      
    case (OP_CODE::LOAD):{

      InstrukcijaLoad(mod,regA,regB,regC,disp);
      procitajInstrukciju(pc);
      break;
    }
    
      break;
  }
}






















void Emulator::InstrukcijaSoftverskogPrekida(){

  gprX[14] -= 4;
  uint32_t adresa = gprX[14];
  int podatak = csrX[0];
  smestiPodatakUMemoriju(adresa,podatak);
  

  gprX[14] -= 4;

  adresa = gprX[14];
  podatak = gprX[15];
  smestiPodatakUMemoriju(adresa,podatak);

  csrX[2] = 4;
  csrX[0] = csrX[0] & (~0x1);

  gprX[15] = csrX[1];

}


void Emulator::InstrukcijaPozivaPotprograma(int mod, int regA, int regB, int regC, int disp){

  gprX[14] -= 4;

  uint32_t adresa = gprX[14];
  int podatak = gprX[15];

  smestiPodatakUMemoriju(adresa,podatak);
  

  switch (mod)
  {
      case (0):{
        
        gprX[15] = gprX[regA] + gprX[regB] + disp;
        break;
      }

      case(1):{

        gprX[15] = procitajIzMemorije(regA, regB, regC, disp);
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

          gprX[15] = gprX[regA] + disp;
          break;
        }
          
        
        case(1):{
 
          if (gprX[regB] == gprX[regC]) gprX[15] = gprX[regA] + disp; 
          break;
        }

        case(2):{
          if (gprX[regB] != gprX[regC]) gprX[15] = gprX[regA] + disp; 
          break;
        }
          
        
        case(3):{
          if (gprX[regB] > gprX[regC]) gprX[15] = gprX[regA] + disp; 
          break;
        }

        case(4):{
          gprX[15] = procitajIzMemorije(regA, regB, regC, disp);
          break;
        }

        case(5):{
          if (gprX[regB] == gprX[regC]) gprX[15] = procitajIzMemorije(regA, regB, regC, disp);
          break;
        }

        case(6):{
          if (gprX[regB] != gprX[regC]) gprX[15] = procitajIzMemorije(regA, regB, regC, disp);
          break;
        }
          
        
        case(7):{
          if (gprX[regB] > gprX[regC]) gprX[15] = procitajIzMemorije(regA, regB, regC, disp);
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

  switch (mod)
  {
        case (0):{

          uint32_t adresa = gprX[regA] + gprX[regB] + disp;
          int podatak = gprX[regC];

          smestiPodatakUMemoriju(adresa,podatak);
          break;
        }

        case(1):{

          gprX[regA] += disp;
          uint32_t adresa = gprX[regA];
          int podatak = gprX[regC];

          smestiPodatakUMemoriju(adresa,podatak);
          break;
        }
          
        
        case(2):{
          int regAA = procitajIzMemorije(regA,regB,0,disp);
          uint32_t adresa = regAA;
          int podatak = gprX[regC];
          smestiPodatakUMemoriju(adresa,podatak);
          break;
        }
        
        default:
          break;
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

  // std::cout << std::hex << gprX[0] << "\t" << gprX[1] << gprX[2] << "\t" << gprX[3] << std::endl;
  // std::cout << gprX[4] << "\t" << gprX[5] << gprX[6] << "\t" << gprX[7] << std::endl;
  // std::cout << gprX[8] << "\t" << gprX[9] << gprX[10] << "\t" << gprX[11] << std::endl;
  // std::cout << gprX[12] << "\t" << gprX[13] << gprX[14] << "\t" << gprX[15] << std::endl;
};
