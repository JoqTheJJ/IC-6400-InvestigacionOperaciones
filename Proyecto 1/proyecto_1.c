/*
                Project 1: Dynamic Programming Menu with Floyd Algorithm
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 12, 2025

                This section contains the main of a multiple algorithms program, where
                each element in the interface shows what the corresponding algorithm does.
                This involves the radio buttons in the menu that redirect to the
                corresponding programs and their own interfaces.

*/


#include <gtk/gtk.h>
#include <cairo.h>
#include <ctype.h>
#include <pthread.h>

typedef struct {
    const char *window_id;  // Window name
    const char *button_id;  // Exit button name
} NewWindow;

//Threads used for allocating other programs
pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;

// Initialize pending.c program
static void *initialize_pending(void *arg){
    /*
    Initializes the pending.c program that consists of a button to exit
    and a "Coming Soon" message for other algorithms of lineal programming
    later to be programmed
    */

    const char *filename = "pending.c";
    const char *compile_cmd = "make pending";

    //printf("Compiling file using: %s\n", compile_cmd);
    system(compile_cmd);

    //printf("\nRunning file: %s\n", filename);
    system("./pending");
    pthread_exit(NULL);

}

// Initialize pragrama-1.c program
static void *initialize_program_1(void *arg){
    /*
    Initializes the programa-1.c program that consists of a way to obtain
    the shortest path between any pair of nodes in a graph with weighted
    distances. This program will do this utilizing the Floyd Algorithm and
    then create a LATEX document with all the tables calculated to get to
    the result.
    */

    const char *filename = "programa-1.c";
    const char *compile_cmd = "make programa-1";

    //printf("Compiling file using: %s\n", compile_cmd);
    system(compile_cmd);

    //printf("\nRunning file: %s\n", filename);
    system("./programa-1");
    pthread_exit(NULL);

}

// Detect button pressed
static void option_clicked(GtkButton *btn, gpointer user_data) {
    int is_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn));
    if (!is_active) {
        return;
    }

    const char* id_string = ((const NewWindow *)user_data)->window_id;
    int id = id_string[6] - 48; //Ascii to integer


     
    // Open the corresponding program to the button pressed
    switch(id){
        case 1:
            pthread_create(&thread1, NULL, initialize_program_1, NULL);
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
            //printf("Invalid Id");
    }
}
    

int main(int argc, char *argv[]) {
    GtkBuilder *builder;        // Used to obtain the objects from glade
    GtkWidget *ventana;         // Window
    GtkWidget *boton_salida;    // Exit button
    // Radio buttons
    GtkWidget *radio1;
    GtkWidget *radio2;
    GtkWidget *radio3;
    GtkWidget *radio4;


    gtk_init(&argc, &argv);
    // Load glade interface
    builder = gtk_builder_new_from_file("interfaz.glade");

    // Programs ids
    static NewWindow ids[] = {
        { "window1", "terminate-1", },
        { "window2", "terminate-2", },
        { "window3", "terminate-3", },
        { "window4", "terminate-4", },
    };

    // Radio buttons
    radio1 = GTK_WIDGET(gtk_builder_get_object(builder, "option-1"));
    radio2 = GTK_WIDGET(gtk_builder_get_object(builder, "option-2"));
    radio3 = GTK_WIDGET(gtk_builder_get_object(builder, "option-3"));
    radio4 = GTK_WIDGET(gtk_builder_get_object(builder, "option-4"));
    // Click-connection for each button
    g_signal_connect(radio1, "clicked", G_CALLBACK(option_clicked), &ids[0]);
    g_signal_connect(radio2, "clicked", G_CALLBACK(option_clicked), &ids[1]);
    g_signal_connect(radio3, "clicked", G_CALLBACK(option_clicked), &ids[2]);
    g_signal_connect(radio4, "clicked", G_CALLBACK(option_clicked), &ids[3]);

    // Window Initialize
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Exit button
    boton_salida = GTK_WIDGET(gtk_builder_get_object(builder, "terminate"));
    g_signal_connect(boton_salida, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    // Show window
    gtk_widget_show_all(ventana);
    // Maximize window
    gtk_window_maximize(GTK_WINDOW(ventana));

    // Run GTK
    gtk_main();

    // Clean memory
    g_object_unref(builder);

    return 0;
}