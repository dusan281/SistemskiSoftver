# Definišete imena datoteka
BISON_FILE = bison2.y
FLEX_FILE = flex2.l
BISON_OUTPUT = bison2.tab.c
BISON_HEADER = bison2.tab.h
FLEX_OUTPUT = lex.yy.c
ASEMBLER_FILES = Asembler.cpp Direktive.cpp Instrukcije.cpp
CXX_FILES = Asembler.cpp Direktive.cpp Instrukcije.cpp Linker.cpp Emulator.cpp
OUTPUT_ASSEMBLER = asembler
OUTPUT_LINKER = linker
OUTPUT_EMULATOR = emulator

# Pravilo za generisanje bison i flex fajlova
all: $(OUTPUT_ASSEMBLER) $(OUTPUT_LINKER) $(OUTPUT_EMULATOR)

$(BISON_OUTPUT) $(BISON_HEADER): $(BISON_FILE)
	bison -d $(BISON_FILE)

$(FLEX_OUTPUT): $(FLEX_FILE)
	flex $(FLEX_FILE)

$(OUTPUT_ASSEMBLER): $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES) $(BISON_HEADER)
	g++ $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES) -lfl -o $(OUTPUT_ASSEMBLER)

$(OUTPUT_LINKER): $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES)
	g++ $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES) -lfl -o $(OUTPUT_LINKER)

$(OUTPUT_EMULATOR): $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES)
	g++ $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES) -lfl -o $(OUTPUT_EMULATOR)

# Pravilo za pokretanje programa sa argumentima
run: $(OUTPUT_ASSEMBLER)
	./$(OUTPUT_ASSEMBLER) -o handler.o fajlovi_asembliranje/handler.s
	./$(OUTPUT_ASSEMBLER) -o main.o fajlovi_asembliranje/main.s
	./$(OUTPUT_ASSEMBLER) -o math.o fajlovi_asembliranje/math.s
	./$(OUTPUT_ASSEMBLER) -o isr_timer.o fajlovi_asembliranje/isr_timer.s
	./$(OUTPUT_ASSEMBLER) -o isr_terminal.o fajlovi_asembliranje/isr_terminal.s
	./$(OUTPUT_ASSEMBLER) -o isr_software.o fajlovi_asembliranje/isr_software.s

link: $(OUTPUT_LINKER)
	./$(OUTPUT_LINKER) -hex handler.o math.o main.o isr_terminal.o isr_timer.o isr_software.o -place=my_code@0x40000000 -place=math@0xF0000000 -o emulator.hex

run_emulator: $(OUTPUT_EMULATOR)
	./$(OUTPUT_EMULATOR) emulator.hex

# Pravilo za čišćenje generisanih fajlova
clean:
	rm -f $(BISON_OUTPUT) $(BISON_HEADER) $(FLEX_OUTPUT) $(OUTPUT_ASSEMBLER) $(OUTPUT_LINKER) $(OUTPUT_EMULATOR)
	rm -f *.txt *.o *.hex
