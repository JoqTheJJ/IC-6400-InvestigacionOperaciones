#include <stdio.h>
#include <stdlib.h>

int main() {

    const char *filename = "pending.c";


    const char *compile_cmd = "gcc `pkg-config --cflags gtk+-3.0` -o a pending.c `pkg-config --libs gtk+-3.0`";

    printf("Compiling file using: %s\n", compile_cmd);
    system(compile_cmd);

    printf("\nRunning file:\n");
    system("./a");

    return 0;
}
