#include "../inc/Emulator.hpp"

int main(int argc, char *argv[]) {

      sem_init(&Emulator::sem,0,1);

      Emulator::inputFileName = argv[1];

      std::thread nit1(Emulator::pokreniEmulator);
      
      std::thread nit2(Emulator::nitTerminal);
      
      
      nit1.join();
      nit2.join();
    
}