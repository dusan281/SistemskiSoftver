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
run_script: all
	./startB.sh

# Pravilo za pokretanje programa sa argumentima
run: $(OUTPUT_ASSEMBLER)
	./$(OUTPUT_ASSEMBLER) -o handler.o fajlovi_asembliranje/handler.s
	./$(OUTPUT_ASSEMBLER) -o main.o fajlovi_asembliranje/main.s
	./$(OUTPUT_ASSEMBLER) -o isr_timer.o fajlovi_asembliranje/isr_timer.s
	./$(OUTPUT_ASSEMBLER) -o isr_terminal.o fajlovi_asembliranje/isr_terminal.s

link: $(OUTPUT_LINKER)
	./$(OUTPUT_LINKER) -hex  main.o isr_terminal.o isr_timer.o handler.o -place=my_code@0x40000000 -o program.hex

run_emulator: $(OUTPUT_EMULATOR)
	./$(OUTPUT_EMULATOR) program.hex

# Pravilo za pokretanje Valgrind-a sa asemblerom
valgrind_asembler: $(OUTPUT_ASSEMBLER)
	@echo "Running Valgrind on $(OUTPUT_ASSEMBLER)..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(OUTPUT_ASSEMBLER) -o handler.o fajlovi_asembliranje/handler.s
	valgrind --leak-check=full --show-leak-kinds=all ./$(OUTPUT_ASSEMBLER) -o main.o fajlovi_asembliranje/main.s
	valgrind --leak-check=full --show-leak-kinds=all ./$(OUTPUT_ASSEMBLER) -o isr_timer.o fajlovi_asembliranje/isr_timer.s
	valgrind --leak-check=full --show-leak-kinds=all ./$(OUTPUT_ASSEMBLER) -o isr_terminal.o fajlovi_asembliranje/isr_terminal.s

# Pravilo za pokretanje Valgrind-a sa linkerom
valgrind_linker: $(OUTPUT_LINKER)
	@echo "Running Valgrind on $(OUTPUT_LINKER)..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(OUTPUT_LINKER) -hex main.o isr_terminal.o isr_timer.o handler.o -place=my_code@0x40000000 -o emulator.hex

# Pravilo za pokretanje Valgrind-a sa emulatorom
valgrind_emulator: $(OUTPUT_EMULATOR)
	@echo "Running Valgrind on $(OUTPUT_EMULATOR)..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(OUTPUT_EMULATOR) emulator.hex

# Pravilo za čišćenje generisanih fajlova
clean:
	rm -f $(BISON_OUTPUT) $(BISON_HEADER) $(FLEX_OUTPUT) $(OUTPUT_ASSEMBLER) $(OUTPUT_LINKER) $(OUTPUT_EMULATOR)
	rm -f *.txt *.o *.hex
