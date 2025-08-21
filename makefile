all: proyecto_0

proyecto_0: proyecto_0.c
	gcc -o proyecto_0 proyecto_0.c `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

clean:
	rm -f ./proyecto_0 || true

