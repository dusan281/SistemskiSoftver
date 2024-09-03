# Definišete imena datoteka
BISON_FILE = misc/bison2.y
FLEX_FILE = misc/flex2.l
BISON_OUTPUT = bison2.tab.c
BISON_HEADER = bison2.tab.h
FLEX_OUTPUT = lex.yy.c
ASEMBLER_FILES = src/main_asembler.cpp src/Asembler.cpp src/Direktive.cpp src/Instrukcije.cpp
LINKER_FILES = src/main_linker.cpp src/Linker.cpp 
EMULATOR_FILES = src/main_emulator.cpp src/Emulator.cpp
OUTPUT_ASSEMBLER = asembler
OUTPUT_LINKER = linker
OUTPUT_EMULATOR = emulator

# Definišete putanje za header fajlove
INCLUDE_DIR = inc

# Pravilo za generisanje bison i flex fajlova
all: $(OUTPUT_ASSEMBLER) $(OUTPUT_LINKER) $(OUTPUT_EMULATOR)

$(BISON_OUTPUT) $(BISON_HEADER): $(BISON_FILE)
	bison -d $(BISON_FILE)

$(FLEX_OUTPUT): $(FLEX_FILE)
	flex $(FLEX_FILE)

$(OUTPUT_ASSEMBLER): $(BISON_OUTPUT) $(FLEX_OUTPUT) $(ASEMBLER_FILES) $(BISON_HEADER)
	g++ -I$(INCLUDE_DIR) $(BISON_OUTPUT) $(FLEX_OUTPUT) $(ASEMBLER_FILES) -lfl -o $(OUTPUT_ASSEMBLER)

$(OUTPUT_LINKER): $(LINKER_FILES)
	g++ -I$(INCLUDE_DIR) $(LINKER_FILES) -lfl -o $(OUTPUT_LINKER)

$(OUTPUT_EMULATOR): $(EMULATOR_FILES)
	g++ -pthread -I$(INCLUDE_DIR) $(EMULATOR_FILES) -lfl -o $(OUTPUT_EMULATOR)

# Pravilo za pokretanje Bash skripte
run_scriptA: all
	./startA.sh

run_scriptB: all
	./startB.sh

# Pravilo za pokretanje programa sa argumentima
run: $(OUTPUT_ASSEMBLER)
	./$(OUTPUT_ASSEMBLER) -o handler.o tests/nivo_A/handler.s
	./$(OUTPUT_ASSEMBLER) -o main.o tests/nivo_A/main.s
	./$(OUTPUT_ASSEMBLER) -o isr_timer.o tests/nivo_A/isr_timer.s
	./$(OUTPUT_ASSEMBLER) -o isr_terminal.o tests/nivo_A/isr_terminal.s
	./$(OUTPUT_ASSEMBLER) -o isr_software.o tests/nivo_A/isr_software.s
	./$(OUTPUT_ASSEMBLER) -o math.o tests/nivo_A/math.s

link: $(OUTPUT_LINKER)
	./$(OUTPUT_LINKER) -hex  handler.o math.o main.o isr_terminal.o isr_timer.o isr_software.o -place=my_code@0x40000000 -place=math@0xF0000000 -o program.hex

run_emulator: $(OUTPUT_EMULATOR)
	./$(OUTPUT_EMULATOR) program.hex

# Pravilo za čišćenje generisanih fajlova
clean:
	rm -f $(BISON_OUTPUT) $(BISON_HEADER) $(FLEX_OUTPUT) $(OUTPUT_ASSEMBLER) $(OUTPUT_LINKER) $(OUTPUT_EMULATOR)
	rm -f *.txt *.o *.hex
