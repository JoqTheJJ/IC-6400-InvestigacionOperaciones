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

runn:
	env -i DISPLAY=$$DISPLAY XAUTHORITY=$${XAUTHORITY:-$$HOME/.Xauthority} PATH=/usr/bin:/bin LD_LIBRARY_PATH=/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 ./programa-5


test:
	rm -f $(EXE)
	gcc -o simplex simplex.c
	./simplex