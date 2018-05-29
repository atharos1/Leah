include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)

SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

SOURCES_STDLIB=$(wildcard StandardLibrary/*.c)
OBJECTS_STDLIB=$(SOURCES_STDLIB:.asm=.o)

SOURCES_PROGRAMS=$(wildcard programs/*.c)
OBJECTS_PROGRAMS=$(SOURCES_PROGRAMS:.asm=.o)


all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS_ASM) $(OBJECTS_STDLIB) $(OBJECTS_PROGRAMS)
	$(GCC) $(GCCFLAGS) -I StandardLibrary -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(OBJECTS_STDLIB) $(OBJECTS_PROGRAMS) $(SOURCES) -o ../$(MODULE)

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o

.PHONY: all clean print
