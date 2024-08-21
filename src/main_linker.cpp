#include "../inc/Linker.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
#include <sstream>

int main(int argc, char *argv[]) {

  for (int i = 1; i < argc; ++i) {
        if (strstr(argv[i],".o")){
          Linker::inputFilesNames.push_back(argv[i]);
        }

        if (strstr(argv[i], "-o")){
          Linker::outputFileName = argv[++i];
        }

  }
    
    if (strcmp(argv[1], "-hex") == 0){

      for (int i = 1; i < argc; ++i) {

        if (strstr(argv[i], "-place=")){
          Linker::postavljanjaString.push_back(argv[i]);
        }

      }      

      Linker::pokreniLinkerHex();

    }



    if (strcmp(argv[1], "-relocatable") == 0){

      Linker::pokreniLinkerRelocatable();
    }

    
    Linker::brojacSekcija = 0;
    Linker::brojacSimbola = 0;
    Linker::postavljanjaString = {};
    Linker::tabelaSimbolaLinker = {};
    Linker::sveSekcijeLinker = {};
    Linker::inputFilesNames = {};
  

  return 0;
}