SRC := $(wildcard *.c)

EXE := $(SRC:.c=)

# Add programa-1
EXE += Programa_1/programa-1

all: $(EXE)


%: %.c
	gcc -o $@ $< `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

Programa_1/%: Programa_1/%.c
	gcc -o $@ $< `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

Programa_1/programa-1: Programa_1/programa-1.c




clean:
	rm -f $(EXE)

run:
	./proyecto_1
