#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
#include <sstream>
#include <string>
#include <iomanip> 


class Emulator{

public :
  static std::map<uint32_t, uint8_t> mapa;

  static void pokreniEmulator();
  static void incijalizujMemoriju();
  static void pokreniIzvrsavanje();
  static void procitajInstrukciju(int& pc);



  static void InstrukcijaSoftverskogPrekida();
  static void InstrukcijaPozivaPotprograma(int mod, int regA, int regB, int regC, int disp);
  static void InstrukcijaSkoka(int mod, int regA, int regB, int regC, int disp);
  static void InstrukcijaZamene(int regB, int regC);
  static void InstrukcijaAritmetika(int mod, int regA, int regB, int regC, int disp);
  static void InstrukcijaLogika(int mod, int regA, int regB, int regC, int disp);
  static void InstrukcijaShift(int mod, int regA, int regB, int regC, int disp);
  static void InstrukcijaStore(int mod, int regA, int regB, int regC, int disp);
  static void InstrukcijaLoad(int mod, int regA, int regB, int regC, int disp);








  static uint32_t procitajIzMemorije(int regA, int regB, int regC, int disp);
  static void smestiPodatakUMemoriju(uint32_t adresa, int podatak);
  static void ispisiRegistre();

  static int gprX[16];
  static int csrX[3];


  static char* inputFileName;

  enum OP_CODE{
    HALT = 0,
    INT,
    CALL,
    JMP,
    EXCHANGE,
    ARITHMETIC,
    LOGIC,
    SHIFT,
    STORE,
    LOAD
  };
};