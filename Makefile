# Makefile

# Imena fajlova
BISON_FILE = bison2.y
FLEX_FILE = flex2.l
BISON_OUTPUT = bison2.tab.c
FLEX_OUTPUT = lex.yy.c
CXX_FILES = Asembler.cpp Direktive.cpp Instrukcije.cpp
OUTPUT = asemble

# Pravilo za generisanje bison i flex fajlova
all: $(OUTPUT)

$(BISON_OUTPUT): $(BISON_FILE)
	bison -d $(BISON_FILE)

$(FLEX_OUTPUT): $(FLEX_FILE)
	flex $(FLEX_FILE)

$(OUTPUT): $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES)
	g++ $(BISON_OUTPUT) $(FLEX_OUTPUT) $(CXX_FILES) -lfl -o $(OUTPUT)

# Pravilo za pokretanje programa
run: $(OUTPUT)
	./$(OUTPUT)

# Pravilo za čišćenje generisanih fajlova
clean:
	rm -f $(BISON_OUTPUT) $(FLEX_OUTPUT) $(OUTPUT) *.tab.c *.tab.h

.PHONY: all run clean