ASSEMBLER=./asembler
LINKER=./linker
EMULATOR=./emulator

${ASSEMBLER} -o main.o tests/nivo_B/main.s
${ASSEMBLER} -o handler.o tests/nivo_B/handler.s
${ASSEMBLER} -o isr_terminal.o tests/nivo_B/isr_terminal.s
${ASSEMBLER} -o isr_timer.o tests/nivo_B/isr_timer.s
${LINKER} -hex \
  -place=my_code@0x40000000 \
  -o program.hex \
  main.o isr_terminal.o isr_timer.o handler.o
${EMULATOR} program.hex