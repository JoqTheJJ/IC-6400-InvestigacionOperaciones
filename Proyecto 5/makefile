SRC := $(filter-out simplex.c proyecto5.c, $(wildcard *.c))
%SRC := $(wildcard *.c)

EXE := $(SRC:.c=)




all: $(EXE)


%: %.c
	gcc -o $@ $< `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

clean:
	rm -f $(EXE)

run:
	./programa-5

test:
	rm -f $(EXE)
	gcc -o simplex simplex.c
	./simplex