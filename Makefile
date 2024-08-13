# Definišete imena datoteka
BISON_FILE = bison2.y
FLEX_FILE = flex2.l
BISON_OUTPUT = bison2.tab.c
BISON_HEADER = bison2.tab.h
FLEX_OUTPUT = lex.yy.c
CXX_FILES = Asembler.cpp Direktive.cpp Instrukcije.cpp Linker.cpp
OUTPUT_ASSEMBLER = asembler
OUTPUT_LINKER = linker

# Pravilo za generisanje bison i flex fajlova
all: $(OUTPUT_ASSEMBLER) $(OUTPUT_LINKER)

$(BISON_OUTPUT) $(BISON_HEADER): $(BISON_FILE)
	bison -d $(BISON_FILE)

$(FLEX_OUTPUT): $(FLEX_FILE)
	flex $(FLEX_FILE)

$(OUTPUT_ASSEMBLER): $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES) $(BISON_HEADER)
	g++ $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES) -lfl -o $(OUTPUT_ASSEMBLER)

$(OUTPUT_LINKER): $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES)
	g++ $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES) -lfl -o $(OUTPUT_LINKER)

# Pravilo za pokretanje programa sa argumentima
run: $(OUTPUT_ASSEMBLER)
	./$(OUTPUT_ASSEMBLER) -o handler.o handler.bin fajlovi_asembliranje/handler.s

link: $(OUTPUT_LINKER)
	./$(OUTPUT_LINKER) -hex -place=data@0x4000F000 -place=text@0x40000000 -o mem_content.hex ulaz1.o ulaz2.o

# Pravilo za čišćenje generisanih fajlova
clean:
	rm -f $(BISON_OUTPUT) $(BISON_HEADER) $(FLEX_OUTPUT) $(OUTPUT_ASSEMBLER) $(OUTPUT_LINKER)

.PHONY: all run link clean
