/*
                Proyecto 0: Menú de Programas de Programación Dinámica
                Hecha por: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Fecha: Viernes 22 de agosto del 2025

                Esta sección contiene el main, donde se indica lo que tiene que hacer
                cada objeto mostrado en la interfaz. Esto involucra los botones de radio
                del menú y los botones de salida de cada programa y del programa general.
                Además, hay una función que abre una ventana cada vez que un botón de radio
                es escogido.
*/


#include <gtk/gtk.h>
#include <cairo.h>
#include <ctype.h>
#include <pthread.h>

typedef struct {
    const char *window_id;  // Nombre de la ventana
    const char *button_id;  // Nombre del botón de salida
} NewWindow;

pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;

// Initialize pending.c program
static void *initialize_pending(void *arg){

    const char *filename = "pending.c";
    const char *compile_cmd = "make pending";

    //printf("Compiling file using: %s\n", compile_cmd);
    system(compile_cmd);

    //printf("\nRunning file: %s\n", filename);
    system("./pending");
    pthread_exit(NULL);

}

// Crear ventana cuando se presiona un botón
static void option_clicked(GtkButton *btn, gpointer user_data) {
    int is_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn));
    if (!is_active) {
        return;
    }

    const char* id_string = ((const NewWindow *)user_data)->window_id;
    int id = id_string[6] - 48; //Ascii to integer

    //printf("El nombre del botón es: %s\n", id_string);
    //printf("El id del botón es: %d\n", id);
     

    switch(id){
        case 1:
            pthread_create(&thread1, NULL, initialize_pending, NULL);
            break;

        case 2:
            pthread_create(&thread2, NULL, initialize_pending, NULL);
            break;

        case 3:
            pthread_create(&thread3, NULL, initialize_pending, NULL);
            break;

        case 4:
            pthread_create(&thread4, NULL, initialize_pending, NULL);
            break;

        default:
            //printf("Id invalido");
    }
}
    



int main(int argc, char *argv[]) {
    GtkBuilder *builder;        // Utilizado para obtener los objetos de glade
    GtkWidget *ventana;         // La ventana
    GtkWidget *boton_salida;    // Botón para terminar el programa
    // Botones de radio
    GtkWidget *radio1;
    GtkWidget *radio2;
    GtkWidget *radio3;
    GtkWidget *radio4;


    gtk_init(&argc, &argv);
    // Cargar la interfaz de Glade
    builder = gtk_builder_new_from_file("interfaz.glade");

    static NewWindow ids[] = {
        { "window1", "terminate-1", },
        { "window2", "terminate-2", },
        { "window3", "terminate-3", },
        { "window4", "terminate-4", },
    };

    // Botones de radio
    radio1 = GTK_WIDGET(gtk_builder_get_object(builder, "option-1"));
    radio2 = GTK_WIDGET(gtk_builder_get_object(builder, "option-2"));
    radio3 = GTK_WIDGET(gtk_builder_get_object(builder, "option-3"));
    radio4 = GTK_WIDGET(gtk_builder_get_object(builder, "option-4"));
    // Conección de clicks en cada botón
    g_signal_connect(radio1, "clicked", G_CALLBACK(option_clicked), &ids[0]);
    g_signal_connect(radio2, "clicked", G_CALLBACK(option_clicked), &ids[1]);
    g_signal_connect(radio3, "clicked", G_CALLBACK(option_clicked), &ids[2]);
    g_signal_connect(radio4, "clicked", G_CALLBACK(option_clicked), &ids[3]);

    // La ventana
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // El bóton de terminación del programa
    boton_salida = GTK_WIDGET(gtk_builder_get_object(builder, "terminate"));
    g_signal_connect(boton_salida, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    // Mostrar ventana
    gtk_widget_show_all(ventana);
    // Que la ventana utilize toda la pantalla
    gtk_window_maximize(GTK_WINDOW(ventana));

    // Correr GTK
    gtk_main();

    // Limpiar la memoria
    g_object_unref(builder);

    return 0;
}