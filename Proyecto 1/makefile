all: proyecto_1 pending programa-1

proyecto_1: proyecto_1.c
	gcc -o proyecto_1 proyecto_1.c `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

pending: pending.c
	gcc -o pending pending.c `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

programa-1: programa-1.c
	gcc -o programa-1 programa-1.c `pkg-config --cflags --libs gtk+-3.0 cairo` -lm

clean:
	rm -f ./proyecto_1 || true
	
