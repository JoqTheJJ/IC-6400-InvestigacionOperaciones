/*
                Project 2: Calculating the Knapsack Problem
                Made by: Carmen Hidalgo Paz, Melissa Carvajal Charpentier
                y Josué Soto González
                Date: Friday, September 19, 2025

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

// Initialize pending.c program
static void initialize_pending(){
    /*
    Initializes the pending.c program that consists of a button to exit
    and a "Coming Soon" message for other algorithms of lineal programming
    later to be programmed
    */

    //printf("\nRunning file: %s\n", filename);
    system("./pending &");

}

// Initialize pragrama-1.c program
static void initialize_program_1(){
    /*
    Initializes the programa-1.c program that consists of a way to obtain
    the shortest path between any pair of nodes in a graph with weighted
    distances. This program will do this utilizing the Floyd Algorithm and
    then create a LATEX document with all the tables calculated to get to
    the result.
    */

    //printf("\nRunning file: %s\n", filename);
    system("(cd Programa_1 && ./programa-1 &)");

}

static void initialize_program_2(){
    /*
    Initializes the programa-1.c program that consists of a way to obtain
    the shortest path between any pair of nodes in a graph with weighted
    distances. This program will do this utilizing the Floyd Algorithm and
    then create a LATEX document with all the tables calculated to get to
    the result.
    */

    //printf("\nRunning file: %s\n", filename);
    system("(cd Programa_2 && ./programa-2 &)");

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
            initialize_program_1();
            break;

        case 2:
            initialize_program_2();
            break;

        case 3:
            initialize_pending();
            break;

        case 4:
            initialize_pending();
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

    // CSS
    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_path(prov, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(prov),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(prov);
    
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

    // Maximize window
    gtk_window_fullscreen(GTK_WINDOW(ventana));
    
    // Show window
    gtk_widget_show_all(ventana);

    // Run GTK
    gtk_main();

    // Clean memory
    g_object_unref(builder);

    return 0;
}