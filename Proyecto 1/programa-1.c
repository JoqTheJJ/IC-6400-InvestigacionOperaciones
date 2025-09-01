/*
                Project 1: Calculating the Shortest Paths using the Floyd Algorithm
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 12, 2025

                This section contains the program used to calculate the shortest
                path between any pair of nodes in a graph with weighted distances.
                This program will do this utilizing the Floyd Algorithm and then
                create a LATEX document with all the tables calculated to get to
                the result.

*/

#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

// Structure to create the new window
typedef struct {
    const char *window_id;  // Window name
    const char *button_id;  // Exit button name
} NewWindow;

// Structure to be able to create a dynamic matrix
typedef struct {
    GtkBuilder *builder;
    GtkWidget  *grid;          // Current table inside the scrolled window
    GPtrArray  *row_headers;   // Names of the rows
    GPtrArray  *col_headers;   // Names of the columns
    GPtrArray  *cells;         // Cells for numbers
    gint        n;
} Matrix;

// Calculate the default names of columns and rows
static gchar *alpha_label(gint idx) {
    // 0 is "A", 1 is "B", ..., 25 is "Z", 26 is "AA" ...
    GString *s = g_string_new(NULL);
    idx += 1;
    while (idx > 0) {
        gint rem = (idx - 1) % 26;
        // Start repeating if alphabet is used up
        g_string_prepend_c(s, 'A' + rem);
        idx = (idx - 1) / 26;
    }
    return g_string_free(s, FALSE);
}

// Clear the past table if a new one is created
static void clear_container_children(GtkWidget *container) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *l = children; l != NULL; l = l->next)
        // Destroy all the cells
        gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(children);
}

// Values that can be accepted as infinity
static gboolean is_inf_text(const gchar *t) {
    if (!t) return FALSE;
    // "i", "I", "inf" and "∞" are accepted
    return g_ascii_strcasecmp(t, "i") == 0
        || g_ascii_strcasecmp(t, "inf") == 0
        || g_strstr_len(t, -1, "∞") != NULL;
}

// Checks if the cell is empty
static gboolean is_blank(const gchar *s) {
    if (!s) return TRUE;
    for (const gchar *p = s; *p; ++p)
        if (!g_ascii_isspace(*p)) return FALSE;
    return TRUE;
}

// Check if the values entered are correct
static gboolean on_cell_focus_out(GtkWidget *entry, GdkEvent *e, gpointer user_data) {
    const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry));
    GtkStyleContext *ctx = gtk_widget_get_style_context(entry);

    // If the cell is empty
    if (is_blank(txt)) {
        // Cell turns red
        gtk_style_context_add_class(ctx, "entry-error");
        // Tooltip shows up
        gtk_widget_set_tooltip_text(entry, "Invalid value. Enter a number or 'i'/'inf' for ∞.");
        return FALSE;
    }

    // If any of the infinity words are entered
    if (is_inf_text(txt)) {
        gtk_entry_set_text(GTK_ENTRY(entry), "∞");
        gtk_style_context_remove_class(ctx, "entry-error");
        gtk_widget_set_tooltip_text(entry, NULL);
        return FALSE;
    }

    // If another value is entered
    char *endp = NULL;
    gdouble v = g_ascii_strtod(txt, &endp);
    if (endp == txt || *endp != '\0') {
        // Cell turns red
        gtk_style_context_add_class(ctx, "entry-error");
        gtk_widget_set_tooltip_text(entry, "Invalid value. Enter a number or 'i'/'inf' for ∞.");
    } else {
        // Its a valid entry
        gtk_style_context_remove_class(ctx, "entry-error");
        gtk_widget_set_tooltip_text(entry, "Enter a number or 'i'/'inf' for ∞.");
    }
    return FALSE;
}

// Changes row name if column name is changed and vice versa
static void sync_partner_header(GtkEntry *changed, Matrix *ui, gboolean changed_is_row) {
    // Obtain the index of the name changed
    gpointer p_index = g_object_get_data(G_OBJECT(changed), "index");
    if (!p_index) return;
    gint idx = GPOINTER_TO_INT(p_index) - 1;
    if (idx < 0 || idx >= ui->n) return;
    // Obtain the text the user wrote
    const gchar *text = gtk_entry_get_text(changed);
    // Look for either the row or column in the same position
    GtkEntry *partner = changed_is_row
        ? g_ptr_array_index(ui->col_headers, idx)
        : g_ptr_array_index(ui->row_headers, idx);
    // If the names are different, then set both to the new name
    if (partner && g_strcmp0(gtk_entry_get_text(partner), text) != 0)
        gtk_entry_set_text(partner, text);
}

// If the row name has been changed
static void on_row_header_changed(GtkEditable *editable, gpointer user_data) {
    sync_partner_header(GTK_ENTRY(editable), (Matrix*)user_data, TRUE);
}

// If the column name has been changed
static void on_col_header_changed(GtkEditable *editable, gpointer user_data) {
    sync_partner_header(GTK_ENTRY(editable), (Matrix*)user_data, FALSE);
}

// Creating boxes for row and column names
static GtkWidget* make_header_entry(Matrix *ui, gint idx, gboolean is_row) {
    GtkWidget *e = gtk_entry_new();
    // About 14 characters are visible in the box
    gtk_entry_set_width_chars(GTK_ENTRY(e), 14);
    // Don't let the cells widen
    gtk_widget_set_hexpand(e, FALSE);
    // Center the text
    gtk_entry_set_alignment(GTK_ENTRY(e), 0.5);

    gchar *lbl = alpha_label(idx);
    gtk_entry_set_text(GTK_ENTRY(e), lbl);
    g_free(lbl);

    // The content can be longer than what's visible
    gtk_entry_set_max_length(GTK_ENTRY(e), 32);
    g_object_set_data(G_OBJECT(e), "index", GINT_TO_POINTER(idx + 1));
    // If a row was changed, change the column too and vice versa
    g_signal_connect(e, "changed", is_row ? G_CALLBACK(on_row_header_changed) : G_CALLBACK(on_col_header_changed), ui);
    return e;
}

// Creating boxes for the cells of the matrix
static GtkWidget* make_cell_entry(Matrix *ui, gint i, gint j) {
    GtkWidget *e = gtk_entry_new();
    // Amount of characters visible and centering them
    gtk_entry_set_width_chars(GTK_ENTRY(e), 6);
    gtk_entry_set_alignment(GTK_ENTRY(e), 0.5);
    gtk_widget_set_hexpand(e, FALSE);

    if (i == j) {
        // The diagonal values are locked on 0
        gtk_entry_set_text(GTK_ENTRY(e), "0");
        gtk_editable_set_editable(GTK_EDITABLE(e), FALSE);
        gtk_widget_set_can_focus(e, FALSE);
        gtk_widget_set_tooltip_text(e, "Diagonal is fixed to 0");
    } else {
        // The rest of the values start with the infinity symbol
        // The user can then change these values
        gtk_entry_set_text(GTK_ENTRY(e), "∞");
        gtk_widget_set_tooltip_text(e, "Enter a number or i/inf/∞ for infinity");
        // If a value is changed, check if it's a valid entry
        g_signal_connect(e, "focus-out-event", G_CALLBACK(on_cell_focus_out), NULL);
    }

    // Record the cell coordinates
    g_object_set_data(G_OBJECT(e), "row", GINT_TO_POINTER(i));
    g_object_set_data(G_OBJECT(e), "col", GINT_TO_POINTER(j));
    return e;
}

// Drawing the matrix again if the node value is changed
static void rebuild_matrix_ui(Matrix *ui, gint n) {
    // The new size is n
    ui->n = n;

    // Free old arrays if there were any
    if (ui->row_headers) { g_ptr_array_free(ui->row_headers, TRUE); ui->row_headers = NULL; }
    if (ui->col_headers) { g_ptr_array_free(ui->col_headers, TRUE); ui->col_headers = NULL; }
    if (ui->cells)       { g_ptr_array_free(ui->cells, TRUE);       ui->cells = NULL; }
    // Create the new arrays
    ui->row_headers = g_ptr_array_new_with_free_func(g_object_unref);
    ui->col_headers = g_ptr_array_new_with_free_func(g_object_unref);
    ui->cells       = g_ptr_array_new_with_free_func(g_object_unref);

    // Get the scrolled window and make sure it has a fresh grid inside a viewport
    GtkWidget *scrolled = GTK_WIDGET(gtk_builder_get_object(ui->builder, "scroll"));
    // Create the table
    GtkWidget *viewport = NULL;
    GList *children = gtk_container_get_children(GTK_CONTAINER(scrolled));
    if (children) {
        // Clear the previous content
        gtk_widget_destroy(GTK_WIDGET(children->data));
        g_list_free(children);
    }
    viewport = gtk_viewport_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), viewport);
    // Create a new table
    ui->grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(ui->grid), 2);
    gtk_grid_set_column_spacing(GTK_GRID(ui->grid), 2);
    gtk_container_add(GTK_CONTAINER(viewport), ui->grid);

    // The corner doesn't have any name
    GtkWidget *corner = gtk_label_new(" ");
    gtk_grid_attach(GTK_GRID(ui->grid), corner, 0, 0, 1, 1);

    // Create the column name cells
    for (gint j = 0; j < n; ++j) {
        GtkWidget *e = make_header_entry(ui, j, FALSE);
        g_ptr_array_add(ui->col_headers, g_object_ref(e));
        gtk_grid_attach(GTK_GRID(ui->grid), e, j + 1, 0, 1, 1);
    }

    // Create the row name cells
    for (gint i = 0; i < n; ++i) {
        GtkWidget *e = make_header_entry(ui, i, TRUE);
        g_ptr_array_add(ui->row_headers, g_object_ref(e));
        gtk_grid_attach(GTK_GRID(ui->grid), e, 0, i + 1, 1, 1);

        for (gint j = 0; j < n; ++j) {
            GtkWidget *cell = make_cell_entry(ui, i, j);
            g_ptr_array_add(ui->cells, g_object_ref(cell));
            gtk_grid_attach(GTK_GRID(ui->grid), cell, j + 1, i + 1, 1, 1);
        }
    }
    // Show the table
    gtk_widget_show_all(scrolled);
}

// Check if the number on the spin button of the amount of nodes has been changed
static void on_spin_value_changed(GtkSpinButton *spin, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    gint n = gtk_spin_button_get_value_as_int(spin);
    if (n < 1) n = 1;
    rebuild_matrix_ui(ui, n);
}

// So that the panels created can't be moved around
void set_panel(GtkPaned *panel, GParamSpec *pspec, gpointer user_data) {
    // Position where division is set
    const int set_pos = 800;
    int current_pos = gtk_paned_get_position(panel);
    if (current_pos != set_pos) {
        gtk_paned_set_position(panel, set_pos);
    }
}


//Main for program 1
int main(int argc, char *argv[]) {
    GtkBuilder *builder;        // Used to obtain the objects from glade
    GtkWidget *ventana;         // Window
    GtkWidget *boton_salida;    // Exit button
    GtkWidget *panel;           // Panel used to divide the menu and the table created
    GtkWidget *boton_nodos;     // Spin button where user enters amount of nodes


    gtk_init(&argc, &argv);

    // CSS - For the red cells
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    g_object_unref(provider);

    // Load glade interface
    builder = gtk_builder_new_from_file("programa-1.glade");

    // Initialize values in the Matrix struct
    Matrix *ui = g_new0(Matrix, 1);
    ui->builder = builder;

    // Window
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "program-1"));
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Division panel
    panel = GTK_WIDGET(gtk_builder_get_object(builder, "panel"));
    g_signal_connect(panel, "notify::position", G_CALLBACK(set_panel), NULL);

    // Spin button
    boton_nodos = GTK_WIDGET(gtk_builder_get_object(builder, "amount-nodes"));
    g_signal_connect(boton_nodos, "value-changed", G_CALLBACK(on_spin_value_changed), ui);

    // Termination button
    boton_salida = GTK_WIDGET(gtk_builder_get_object(builder, "exit"));
    g_signal_connect(boton_salida, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    // Initial build
    gint n0 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(boton_nodos));
    if (n0 < 1) n0 = 1;
    rebuild_matrix_ui(ui, n0);
    
    // Show window
    gtk_widget_show_all(ventana);

    // Maximize window
    gtk_window_maximize(GTK_WINDOW(ventana));

    // Run GTK
    gtk_main();

    // Cleanup
    if (ui->row_headers) g_ptr_array_free(ui->row_headers, TRUE);
    if (ui->col_headers) g_ptr_array_free(ui->col_headers, TRUE);
    if (ui->cells)       g_ptr_array_free(ui->cells, TRUE);
    g_free(ui);
    g_object_unref(builder);

    return 0;
}
