#include <gtk/gtk.h>

GtkWidget	*window;
GtkWidget	*button;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("a.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
        
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_builder_connect_signals(builder, NULL);

    
    gtk_main();

    return 0;
}

