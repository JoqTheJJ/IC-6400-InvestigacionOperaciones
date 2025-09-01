/*
                Project 1: Dynamic Programming Menu
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 12, 2025

                This section contains the pending program which is a placeholder
                for future implementations of different dynamic programming
                algorithms. The program is very simple and only consists of a simple
                message and an exit button that terminates it.

*/

#include <gtk/gtk.h>
#include <cairo.h>

typedef struct {
    const char *window_id;  // Window name
    const char *button_id;  // Exit button name
} NewWindow;


//Main pending program
int main(int argc, char *argv[]) {

    GtkBuilder *builder;        // Used to obtain the objects from glade
    GtkWidget *ventana;         // Window
    GtkWidget *boton_salida;    // Exit button


    gtk_init(&argc, &argv);
    // Load glade interface
    builder = gtk_builder_new_from_file("pending.glade");



    // Window
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Termination button
    boton_salida = GTK_WIDGET(gtk_builder_get_object(builder, "terminate-1"));
    g_signal_connect(boton_salida, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    // Show window
    gtk_widget_show_all(ventana);

    // Run GTK
    gtk_main();

    // Cleanup
    g_object_unref(builder);

    return 0;
}

