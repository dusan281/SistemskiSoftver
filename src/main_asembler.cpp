#include "../inc/Asembler.hpp"
extern FILE *yyin;
extern int yyparse();
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
using namespace std;
#include <sstream>

int main(int argc, char* argv[])
{
  const char* inputFile = nullptr;

	for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "-o") == 0) {
          if (i + 1 < argc) {
              Asembler::outputBinaryName = argv[++i];
              std::string s = std::string(Asembler::outputBinaryName);
              Asembler::outputFileName = s.substr(0, s.size()-2) + ".txt";
          } else {
              std::cerr << "Option -o requires a filename." << std::endl;
              return EXIT_FAILURE;
          }
      } else {
          inputFile = argv[i];
      }
    }

    if (!inputFile) {
        std::cerr << "Both -o <output_file> and <input_file> must be specified." << std::endl;
        return EXIT_FAILURE;
    }

  
    FILE* input = fopen(inputFile, "r");
    if (!input) {
        std::cerr << "I can't open " << inputFile << "!" << std::endl;
        return EXIT_FAILURE;
    }
    yyin = input;

    

    
    try {
        yyparse();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        fclose(input);
        return EXIT_FAILURE;
    }

    fclose(input);

    return EXIT_SUCCESS;
}
