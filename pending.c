/*
                Proyecto 0: Menú de Programas de Programación Dinámica
                Hecha por: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Fecha: Viernes 22 de agosto del 2025

                Esta sección contiene el programa pending.
*/

#include <gtk/gtk.h>
#include <cairo.h>

typedef struct {
    const char *window_id;  // Nombre de la ventana
    const char *button_id;  // Nombre del botón de salida
} NewWindow;



int main(int argc, char *argv[]) {

    GtkBuilder *builder;        // Utilizado para obtener los objetos de glade
    GtkWidget *ventana;         // La ventana
    GtkWidget *boton_salida;    // Botón para terminar el programa


    gtk_init(&argc, &argv);
    // Cargar la interfaz de Glade
    builder = gtk_builder_new_from_file("pending.glade");



    // La ventana
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // El bóton de terminación del programa
    boton_salida = GTK_WIDGET(gtk_builder_get_object(builder, "terminate-1"));
    g_signal_connect(boton_salida, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    // Mostrar ventana
    gtk_widget_show_all(ventana);

    // Correr GTK
    gtk_main();

    return 0;
}

