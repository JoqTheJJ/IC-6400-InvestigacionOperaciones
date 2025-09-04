SRC := $(wildcard *.c)

EXE := $(SRC:.c=)


all: $(EXE)


%: %.c
	gcc -o $@ $< `pkg-config --cflags --libs gtk+-3.0 cairo` -lm


clean:
	rm -f $(EXE)

run:
	./proyecto_1
