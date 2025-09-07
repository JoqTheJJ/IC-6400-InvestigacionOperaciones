SRC := $(wildcard *.c)

EXE := $(SRC:.c=)

# Add programas
#EXE += Programa_1/programa-1
EXE += Programa_2/knapsack

all: $(EXE)


%: %.c
	gcc -o $@ $< `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

#Programa_1/%: Programa_1/%.c
#	gcc -o $@ $< `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

Programa_2/%: Programa_2/%.c
	gcc -o $@ $< `pkg-config --cflags --libs gtk+-3.0 cairo` -lm




clean:
	rm -f $(EXE)

run:
	./proyecto
