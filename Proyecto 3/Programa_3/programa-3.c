/*
                Project 2: Calculating the Knapsack Problem
                Made by: Carmen Hidalgo Paz, Melissa Carvajal Charpentier
                y Josué Soto González
                Date: Friday, September 19, 2025

                This section contains the program used to calculate the Knapsack
                problem using Dynamic Programming. It then creates a LATEX
                document with all the tables calculated to get to the result.

*/

#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <glib/gstdio.h>
#include "reemplazo.c"

// - - - - - TABLE IN THE INTERFACE - - - - -
// Structure to be able to create a dynamic matrix
typedef struct {
    GtkBuilder *builder;
    GtkWidget  *grid;
    GPtrArray  *row_headers;
    GPtrArray  *col_headers;
    GPtrArray  *cells;
    gint        rows;              // number of periods (n)
    gint        cols;              // 3 (Period, Maintenance, Resell)
    gint        invalid_count;

    // Spin values captured from UI
    gint        amount_price;      // equipment price
    gint        amount_period;     // number of periods
    gint        amount_lifespan;   // equipment lifespan
    gint        amount_profit;     // profit per use/period
    gint        amount_inflation;  // inflation percent
} Matrix;

typedef struct {
    gint        rows;       // number of periods
    gint        cols;       // 3 (Period, Maintenance, Resell)
    GPtrArray  *row_names;  // period labels / names
    GPtrArray  *col_names;  // {"Period","Maintenance","Resell"}
    GArray     *values;     // row-major double array of size rows*cols

    // Tool Replacement parameters (from spin buttons)
    gint        price;      // amount-price
    gint        lifespan;   // amount-lifespan
    gint        profit;     // amount-profit
    gint        inflation;  // amount-inflation
    gint        periods;    // amount-period
} MatrixData;

// Fixed column names
static const char *FIXED_COLS[3] = {"Period", "Maintenance", "Resell"};
// Fixed amount of columns
static const guint FIXED_COL_COUNT = 3;
#define PERIOD_COL_IDX 0

// Color of the text of the column names
static GtkCssProvider* get_header_css_provider(void) {
    static GtkCssProvider *prov = NULL;
    if (!prov) {
        prov = gtk_css_provider_new();
        gtk_css_provider_load_from_data(
            prov,
            ".col-header-yellow { background-color: rgba(249, 228, 183, 0.67); color: #000000; font-weight: 700; }",
            -1, NULL);
    }
    return prov;
}

// Calculate the default names of rows
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

// Checks if the cell is empty
static gboolean is_blank(const gchar *s) {
    if (!s) return TRUE;
    for (const gchar *p = s; *p; ++p)
        if (!g_ascii_isspace(*p)) return FALSE;
    return TRUE;
}

// Checks its only positive integers only
static gboolean integer_digits_only(const char *s_in) {
    if (!s_in) return FALSE;
    gchar *s = g_strdup(s_in);
    g_strstrip(s);
    if (*s == '\0') { g_free(s); return FALSE; }
    for (const char *p = s; *p; ++p) {
        if (!g_ascii_isdigit((guchar)*p)) { g_free(s); return FALSE; }
    }
    g_free(s);
    return TRUE;
}

// Checks if the cell is valid; updates the CSS and the invalid cell counter
static gboolean mark_cell_validity(GtkWidget *entry, Matrix *ui) {
    // if it's the Period column, it's read-only and always valid
    gpointer pcol = g_object_get_data(G_OBJECT(entry), "col");
    int j = pcol ? (GPOINTER_TO_INT(pcol)-1) : -1;
    if (j == PERIOD_COL_IDX) return TRUE;

    const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry));
    GtkStyleContext *ctx = gtk_widget_get_style_context(entry);

    gboolean now_invalid = FALSE;

    if (is_blank(txt)) {
        now_invalid = TRUE;
    } else {
        // Only allow whole non-negative integers (0,1,2,...)
        now_invalid = !integer_digits_only(txt);
    }

    gboolean was_invalid = g_object_get_data(G_OBJECT(entry), "invalid") != NULL;

    // update the invalid counter
    if (now_invalid && !was_invalid) {
        ui->invalid_count++;
        g_object_set_data(G_OBJECT(entry), "invalid", GINT_TO_POINTER(1));
    } else if (!now_invalid && was_invalid) {
        ui->invalid_count = MAX(0, ui->invalid_count - 1);
        g_object_set_data(G_OBJECT(entry), "invalid", NULL);
    }

    if (now_invalid) {
        gtk_widget_set_tooltip_text(entry, "Enter a whole number (0,1,2,…).");
        gtk_style_context_add_class(ctx, "entry-error");
    } else {
        gtk_style_context_remove_class(ctx, "entry-error");
        gtk_widget_set_tooltip_text(entry, NULL);
    }

    return !now_invalid;
}

// Wrappers for signals
static void on_cell_changed(GtkEditable *editable, gpointer user_data) {
    mark_cell_validity(GTK_WIDGET(editable), (Matrix*)user_data);
}

static gboolean on_cell_focus_out(GtkWidget *entry, GdkEvent *e, gpointer user_data) {
    mark_cell_validity(entry, (Matrix*)user_data);
    return FALSE;
}

// Row names can be edited
static GtkWidget* make_row_header_entry(gint idx) {
    GtkWidget *e = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(e), 14);
    gtk_widget_set_hexpand(e, FALSE);
    gtk_entry_set_alignment(GTK_ENTRY(e), 0.5);
    gchar *lbl = alpha_label(idx);
    gtk_entry_set_text(GTK_ENTRY(e), lbl);
    g_free(lbl);
    gtk_entry_set_max_length(GTK_ENTRY(e), 32);
    return e;
}

// Column headers are non-editable labels
static GtkWidget* make_col_header_label(const char *text) {
    GtkWidget *l = gtk_label_new(text);
    gtk_widget_set_hexpand(l, FALSE);
    gtk_label_set_xalign(GTK_LABEL(l), 0.5);

    GtkStyleContext *ctx = gtk_widget_get_style_context(l);
    gtk_style_context_add_class(ctx, "col-header-yellow");
    gtk_style_context_add_provider(
        ctx, GTK_STYLE_PROVIDER(get_header_css_provider()),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    return l;
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
    return e;
}

// Creating boxes for the cells of the matrix
static GtkWidget* make_cell_entry(Matrix *ui, gint i, gint j) {
    GtkWidget *e = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(e), 6);
    gtk_entry_set_alignment(GTK_ENTRY(e), 0.5);

    g_object_set_data(G_OBJECT(e), "row", GINT_TO_POINTER(i+1));
    g_object_set_data(G_OBJECT(e), "col", GINT_TO_POINTER(j+1));

    if (j == PERIOD_COL_IDX) {
        gchar buf[16];
        g_snprintf(buf, sizeof(buf), "%d", i + 1);
        gtk_entry_set_text(GTK_ENTRY(e), buf);

        // Make it read-only but keep normal look
        gtk_editable_set_editable(GTK_EDITABLE(e), FALSE);
        gtk_widget_set_can_focus(e, FALSE);

        return e;
    }

    // Editable numeric cells for Maintenance / Resell
    gtk_entry_set_text(GTK_ENTRY(e), "0");
    mark_cell_validity(e, ui);
    g_signal_connect(e, "changed", G_CALLBACK(on_cell_changed), ui);
    g_signal_connect(e, "focus-out-event", G_CALLBACK(on_cell_focus_out), ui);
    return e;
}

// Drawing the matrix again if the node value is changed
static void rebuild_matrix_ui(Matrix *ui, gint n_rows) {
    ui->rows = MAX(1, n_rows);
    ui->cols = 3;

    if (ui->row_headers) { g_ptr_array_free(ui->row_headers, TRUE); ui->row_headers = NULL; }
    if (ui->col_headers) { g_ptr_array_free(ui->col_headers, TRUE); ui->col_headers = NULL; }
    if (ui->cells)       { g_ptr_array_free(ui->cells, TRUE);       ui->cells = NULL; }

    ui->row_headers = g_ptr_array_new_with_free_func(g_object_unref);
    ui->col_headers = g_ptr_array_new_with_free_func(g_object_unref);
    ui->cells       = g_ptr_array_new_with_free_func(g_object_unref);

    GtkWidget *scrolled = GTK_WIDGET(gtk_builder_get_object(ui->builder, "scroll"));

    // Clear old child (table) and rebuild
    GList *children = gtk_container_get_children(GTK_CONTAINER(scrolled));
    if (children) { gtk_widget_destroy(GTK_WIDGET(children->data)); g_list_free(children); }

    GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), viewport);

    ui->grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(ui->grid), 2);
    gtk_grid_set_column_spacing(GTK_GRID(ui->grid), 2);
    gtk_container_add(GTK_CONTAINER(viewport), ui->grid);

    // Top-left corner
    GtkWidget *corner = gtk_label_new(" ");
    gtk_grid_attach(GTK_GRID(ui->grid), corner, 0, 0, 1, 1);

    // Column headers
    for (gint j = 0; j < ui->cols; ++j) {
        GtkWidget *h = make_col_header_label(FIXED_COLS[j]);
        g_ptr_array_add(ui->col_headers, g_object_ref(h));
        gtk_grid_attach(GTK_GRID(ui->grid), h, j + 1, 0, 1, 1);
    }

    // Rows: header + cells
    for (gint i = 0; i < ui->rows; ++i) {
        GtkWidget *rh = make_row_header_entry(i);
        g_ptr_array_add(ui->row_headers, g_object_ref(rh));
        gtk_grid_attach(GTK_GRID(ui->grid), rh, 0, i + 1, 1, 1);

        for (gint j = 0; j < ui->cols; ++j) {
            GtkWidget *cell = make_cell_entry(ui, i, j);
            g_ptr_array_add(ui->cells, g_object_ref(cell));
            gtk_grid_attach(GTK_GRID(ui->grid), cell, j + 1, i + 1, 1, 1);
        }
    }

    gtk_widget_show_all(scrolled);
}

// Check if the number on the spin button has changed
static void on_amount_price_changed(GtkSpinButton *spin, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    ui->amount_price = gtk_spin_button_get_value_as_int(spin);
}

// Check if the number on the spin button has changed
static void on_amount_period_changed(GtkSpinButton *spin, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    gint n = gtk_spin_button_get_value_as_int(spin);
    ui->amount_period = MAX(1, n);
}

// Check if the number on the spin button has changed
static void on_amount_lifespan_changed(GtkSpinButton *spin, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    gint n = gtk_spin_button_get_value_as_int(spin);
    ui->amount_lifespan = gtk_spin_button_get_value_as_int(spin);
    rebuild_matrix_ui(ui, ui->amount_lifespan);
}

// Check if the number on the spin button has changed
static void on_amount_profit_changed(GtkSpinButton *spin, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    ui->amount_profit = gtk_spin_button_get_value_as_int(spin);
}

// Check if the number on the spin button has changed
static void on_amount_inflation_changed(GtkSpinButton *spin, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    ui->amount_inflation = gtk_spin_button_get_value_as_int(spin);
}
// - - - - - END OF TABLE IN THE INTERFACE - - - - -

// - - - - - SAVING THE FILE - - - - -

// Tkes care of extra spaces
static gchar* trimmed_copy(const gchar *s) {
    if (!s) return g_strdup("");
    gchar *cpy = g_strdup(s);
    return g_strstrip(cpy);
}

// Makes sure that the numbers have no weird spaces
static void cell_string_for_export(GtkEntry *entry, gint i, gint j, gchar **out) {
    (void)i; (void)j;

    const gchar *txt0 = gtk_entry_get_text(entry);
    gchar *txt = trimmed_copy(txt0);

    if (is_blank(txt)) { g_free(txt); *out = g_strdup(""); return; }

    *out = txt;
}

// Helps align the numbers for each column
static size_t ulen(const gchar *s) {
    return g_utf8_strlen(s ? s : "", -1);
}

// So that the text table looks cleanly lined up with padding when needed
static void fprint_padded(FILE *fp, const gchar *s, size_t width, gboolean right_align) {
    size_t len = ulen(s);
    size_t pad = (width > len) ? (width - len) : 0;
    if (right_align) { for (size_t k=0;k<pad;k++) fputc(' ', fp); fputs(s, fp); }
    else { fputs(s, fp); for (size_t k=0;k<pad;k++) fputc(' ', fp); }
}

// Save the table as a .txt file
static void save_file(GtkButton *btn, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    const gint rows = ui->rows;
    const gint cols = ui->cols;

    // Validate current focus
    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(btn));
    if (GTK_IS_WINDOW(toplevel)) {
        GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(toplevel));
        if (focus && GTK_IS_ENTRY(focus))
            mark_cell_validity(focus, ui);
    }
    if (ui->invalid_count > 0) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(toplevel),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "There are invalid cells. Please fix highlighted entries before saving.");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        return;
    }

    // Ask where to save
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Save Table As:",
        GTK_WINDOW(toplevel),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save",   GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Tool-Replacement-Table.txt");

    GtkFileFilter *flt = gtk_file_filter_new();
    gtk_file_filter_set_name(flt, "Text files");
    gtk_file_filter_add_pattern(flt, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), flt);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT) {
        gtk_widget_destroy(dialog);
        return;
    }
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    gtk_widget_destroy(dialog);

    // Compute column widths for pretty table
    size_t *W = g_new0(size_t, (size_t)cols + 1);

    // Row-name width (names column)
    for (gint i = 0; i < rows; ++i) {
        GtkEntry *rh = GTK_ENTRY(g_ptr_array_index(ui->row_headers, i));
        W[0] = MAX(W[0], ulen(gtk_entry_get_text(rh)));
    }

    // Column headers + cell widths
    for (gint j = 0; j < cols; ++j) {
        GtkWidget *hdr = g_ptr_array_index(ui->col_headers, j);
        const gchar *h = gtk_label_get_text(GTK_LABEL(hdr));
        W[j+1] = MAX(W[j+1], ulen(h));

        for (gint i = 0; i < rows; ++i) {
            GtkEntry *cell = GTK_ENTRY(g_ptr_array_index(ui->cells, i*cols + j));
            gchar *s = NULL;
            cell_string_for_export(cell, i, j, &s);
            W[j+1] = MAX(W[j+1], ulen(s));
            g_free(s);
        }
    }

    // Write file
    FILE *fp = g_fopen(filename, "w");
    if (!fp) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(toplevel),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Could not open file for writing:\n%s", filename);
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        g_free(filename);
        g_free(W);
        return;
    }

    // Stored in Matrix: amount_price, amount_lifespan, amount_profit, amount_inflation, amount_period
    fprintf(fp, "# Tool Replacement Parameters\n");
    fprintf(fp, "Price: %d\n",      ui->amount_price);
    fprintf(fp, "Lifespan: %d\n",   ui->amount_lifespan);
    fprintf(fp, "Profit: %d\n",     ui->amount_profit);
    fprintf(fp, "Inflation: %d\n",  ui->amount_inflation);
    fprintf(fp, "Periods: %d\n",    ui->rows);
    fputc('\n', fp);

    // Print an empty slot for the row-name column (keeps alignment with saved row names)
    fprint_padded(fp, "", W[0], FALSE);
    if (cols > 0) fputc(' ', fp);
    for (gint j = 0; j < cols; ++j) {
        GtkWidget *hdr = g_ptr_array_index(ui->col_headers, j);
        const gchar *h = gtk_label_get_text(GTK_LABEL(hdr));
        fprint_padded(fp, h ? h : "", W[j+1], FALSE);
        if (j < cols-1) fputc(' ', fp);
    }
    fputc('\n', fp);

    // Rows
    for (gint i = 0; i < rows; ++i) {
        GtkEntry *rh = GTK_ENTRY(g_ptr_array_index(ui->row_headers, i));
        const gchar *rname = gtk_entry_get_text(rh);
        fprint_padded(fp, rname ? rname : "", W[0], FALSE);
        if (cols > 0) fputc(' ', fp);

        for (gint j = 0; j < cols; ++j) {
            GtkEntry *cell = GTK_ENTRY(g_ptr_array_index(ui->cells, i*cols + j));
            gchar *s = NULL;
            cell_string_for_export(cell, i, j, &s);
            fprint_padded(fp, s, W[j+1], TRUE);
            if (j < cols-1) fputc(' ', fp);
            g_free(s);
        }
        fputc('\n', fp);
    }

    fclose(fp);
    g_free(W);
    g_free(filename);

    GtkWidget *done = gtk_message_dialog_new(
        GTK_WINDOW(toplevel),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
        "Table saved successfully.");
    gtk_dialog_run(GTK_DIALOG(done));
    gtk_widget_destroy(done);
}

// - - - - - END OF SAVING THE FILE - - - - -

// - - - - - UPLOADING A FILE - - - - -

// Checks the data at the top that contains the info of the spinbuttons
static gboolean parse_int_field(const gchar *line, const gchar *key, gint *out) {
    if (!line || !key || !out) return FALSE;
    gchar *prefix = g_strdup_printf("%s:", key);
    gboolean ok = FALSE;

    // Trim a copy to be robust
    gchar *t = g_strdup(line);
    g_strstrip(t);

    // Case-sensitive match on prefix, then parse the number
    if (g_str_has_prefix(t, prefix)) {
        const gchar *p = t + strlen(prefix);
        while (*p && g_ascii_isspace(*p)) p++;
        // digits only
        if (*p) {
            ok = TRUE;
            for (const gchar *q=p; *q; ++q) {
                if (!g_ascii_isdigit(*q)) { ok = FALSE; break; }
            }
            if (ok) *out = (gint)g_ascii_strtoll(p, NULL, 10);
        }
    }

    g_free(prefix);
    g_free(t);
    return ok;
}

// Splits a line into tokens on runs of whitespace after trimming the ends
static gchar** split_ws(const gchar *line) {
    if (!line) return g_new0(gchar*, 1);
    gchar *t = g_strdup(line);
    g_strstrip(t);
    gchar **v = g_regex_split_simple("\\s+", t, 0, 0);
    g_free(t);
    return v;
}

// Header must end with the 3 fixed labels
static gboolean header_matches_fixed(gchar **head, guint headc) {
    if (headc < 3) return FALSE;
    return g_ascii_strcasecmp(head[headc-3], "Period")      == 0 &&
           g_ascii_strcasecmp(head[headc-2], "Maintenance") == 0 &&
           g_ascii_strcasecmp(head[headc-1], "Resell")      == 0;
}

// Opening a file and seeing the table in the interface
static void file_chosen(GtkFileChooserButton *btn, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;

    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(btn));
    if (!filename) return;

    // Read whole file
    gchar *contents = NULL; gsize len = 0; GError *err = NULL;
    if (!g_file_get_contents(filename, &contents, &len, &err)) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Could not read file:\n%s\n\nError: %s",
            filename, err ? err->message : "unknown");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        if (err) g_error_free(err);
        g_free(filename);
        return;
    }

    // Split & keep non-empty lines (trimmed)
    gchar **lines = g_strsplit(contents, "\n", -1);
    GPtrArray *rows = g_ptr_array_new_with_free_func(g_free);
    for (guint k = 0; lines[k]; ++k) {
        gchar *t = g_strdup(lines[k]);
        g_strstrip(t);
        if (*t) g_ptr_array_add(rows, t); else g_free(t);
    }
    if (rows->len == 0) {
        g_strfreev(lines); g_free(contents); g_free(filename);
        g_ptr_array_free(rows, TRUE);
        return;
    }

    // Parse the info block at top
    // Accept either a block starting with "#" or plain "Key: value" lines.
    // Stop parsing at the first blank line in the original file
    // or when it hits a header-like line that ends with "Period Maintenance Resell".
    gint price = -1, lifespan = -1, profit = -1, inflation = -1, periods = -1;

    guint idx = 0;
    for (; idx < rows->len; ++idx) {
        const gchar *L = rows->pdata[idx];
        // Check if its the header line
        gchar **tok = split_ws(L);
        guint tokc = g_strv_length(tok);
        gboolean is_header = header_matches_fixed(tok, tokc);
        g_strfreev(tok);
        if (is_header) break;

        // Info line, starts with #
        const gchar *line = L;
        if (*line == '#') {
            line++; while (*line && g_ascii_isspace(*line)) line++;
        }

        gboolean any =
            parse_int_field(line, "Price",     &price)     ||
            parse_int_field(line, "Lifespan",  &lifespan)  ||
            parse_int_field(line, "Profit",    &profit)    ||
            parse_int_field(line, "Inflation", &inflation) ||
            parse_int_field(line, "Periods",   &periods);

        // If it's not the info and not a header, it keeps advancing until header is found.
    }

    if (idx >= rows->len) {
        // We a header line isnt found
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Could not find table header ending with: \"Period Maintenance Resell\".");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        g_strfreev(lines); g_free(contents); g_free(filename);
        g_ptr_array_free(rows, TRUE);
        return;
    }

    // Header
    gchar **head = split_ws(rows->pdata[idx]);
    guint headc  = g_strv_length(head);
    if (!header_matches_fixed(head, headc)) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Header does not end with the required columns:\n\"Period Maintenance Resell\".\n"
            "I'll proceed anyway and force those column names.");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
    }

    // Data rows
    guint data0 = idx + 1;
    guint nrows = (rows->len > data0) ? (rows->len - data0) : 0;
    if (nrows == 0) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "No data rows after the header.");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        g_strfreev(head);
        g_strfreev(lines); g_free(contents); g_free(filename);
        g_ptr_array_free(rows, TRUE);
        return;
    }

    // If the "Periods" metadata exists, check it. If not infer from rows
    gint target_rows = (periods > 0) ? periods : (gint)nrows;
    if (target_rows < 1) target_rows = (gint)nrows;

    // Build UI
    rebuild_matrix_ui(ui, target_rows);
    ui->invalid_count = 0;

    // Force fixed column labels
    for (guint j = 0; j < ui->cols; ++j) {
        GtkWidget *cw = g_ptr_array_index(ui->col_headers, j);
        gtk_label_set_text(GTK_LABEL(cw), FIXED_COLS[j]); // {"Period","Maintenance","Resell"}
    }

    // Fill rows
    guint to_fill = MIN(nrows, (guint)ui->rows);

    for (guint r = 0; r < to_fill; ++r) {
        gchar **tok = split_ws(rows->pdata[data0 + r]);
        guint tokc = g_strv_length(tok);

        if (tokc < 1 + 3) { // the rowname plus 3 values
            GtkWidget *d = gtk_message_dialog_new(
                GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Row %u has %u token(s); expected at least %u (name + 3 values).",
                r+1, tokc, 4);
            gtk_dialog_run(GTK_DIALOG(d));
            gtk_widget_destroy(d);
            g_strfreev(tok);
            continue;
        }

        guint vstart = tokc - 3;

        // The row name may contain spaces
        gchar *saved = tok[vstart];
        tok[vstart] = NULL;
        gchar *rname = g_strjoinv(" ", tok);
        tok[vstart] = saved;

        // Set row header text
        GtkEntry *rh = GTK_ENTRY(g_ptr_array_index(ui->row_headers, r));
        gtk_entry_set_text(rh, rname ? rname : "");
        g_free(rname);

        // Fill cells
        for (guint j = 0; j < (guint)ui->cols; ++j) {
            GtkWidget *cell = GTK_WIDGET(g_ptr_array_index(ui->cells, r * ui->cols + j));

            if (j == PERIOD_COL_IDX) {
                // Ignore file value and force Period = i+1
                gchar buf[16];
                g_snprintf(buf, sizeof(buf), "%u", r + 1);
                gtk_entry_set_text(GTK_ENTRY(cell), buf);
            } else {
                const gchar *vstr = tok[vstart + j];
                // digits-only expected
                if (!integer_digits_only(vstr)) {
                    // still show whatever is there so user can fix it but mark as invalid
                    gtk_entry_set_text(GTK_ENTRY(cell), vstr ? vstr : "");
                    mark_cell_validity(cell, ui);
                } else {
                    gtk_entry_set_text(GTK_ENTRY(cell), vstr);
                    mark_cell_validity(cell, ui);
                }
            }
        }

        g_strfreev(tok);
    }

    // Reflect info in the spinbuttons
    // IDs: amount-price, amount-period, amount-lifespan, amount-profit, amount-inflation
    GtkSpinButton *sp_price     = GTK_SPIN_BUTTON(gtk_builder_get_object(ui->builder, "amount-price"));
    GtkSpinButton *sp_period    = GTK_SPIN_BUTTON(gtk_builder_get_object(ui->builder, "amount-period"));
    GtkSpinButton *sp_lifespan  = GTK_SPIN_BUTTON(gtk_builder_get_object(ui->builder, "amount-lifespan"));
    GtkSpinButton *sp_profit    = GTK_SPIN_BUTTON(gtk_builder_get_object(ui->builder, "amount-profit"));
    GtkSpinButton *sp_inflation = GTK_SPIN_BUTTON(gtk_builder_get_object(ui->builder, "amount-inflation"));

    if (price     >= 0 && sp_price)     gtk_spin_button_set_value(sp_price,     price);
if (sp_period && periods >= 0) {
    // prevent the matrix from rebuilding
    g_signal_handlers_block_by_func(sp_period, G_CALLBACK(on_amount_period_changed), ui);
    gtk_spin_button_set_value(sp_period, periods);
    g_signal_handlers_unblock_by_func(sp_period, G_CALLBACK(on_amount_period_changed), ui);
}
if (lifespan  >= 0 && sp_lifespan)  gtk_spin_button_set_value(sp_lifespan,  lifespan);
if (profit    >= 0 && sp_profit)    gtk_spin_button_set_value(sp_profit,    profit);
if (inflation >= 0 && sp_inflation) gtk_spin_button_set_value(sp_inflation, inflation);

    // Clean up
    g_strfreev(head);
    g_ptr_array_free(rows, TRUE);
    g_strfreev(lines);
    g_free(contents);
    g_free(filename);
}

// - - - - - END OF UPLOADING A FILE - - - - -


// - - - - - CREATING MATRIX FOR CODE - - - - -

// Initializes the struct values
static void matrix_data_init(MatrixData *md, gint rows, gint cols) {
    md->rows = rows; md->cols = cols;
    md->row_names = g_ptr_array_new_with_free_func(g_free);
    md->col_names = g_ptr_array_new_with_free_func(g_free);
    md->values    = g_array_sized_new(FALSE, TRUE, sizeof(double), rows * cols);

    md->price = 0;
    md->lifespan = 0;
    md->profit = 0;
    md->inflation = 0;
    md->periods = rows;
}

// Clean up
static void matrix_data_clear(MatrixData *md) {
    if (!md) return;
    if (md->row_names) g_ptr_array_free(md->row_names, TRUE);
    if (md->col_names) g_ptr_array_free(md->col_names, TRUE);
    if (md->values)    g_array_free(md->values, TRUE);

    md->row_names = md->col_names = NULL;
    md->values = NULL;
    md->rows = md->cols = 0;

    md->price = md->lifespan = md->profit = md->inflation = md->periods = 0;
}

// Cleans up spaces so that there aren't any extra lying around
static inline gchar *trimdup(const gchar *s) {
    if (!s) return g_strdup("");
    gsize len = g_utf8_strlen(s, -1);
    const gchar *b = s; while (*b && g_ascii_isspace(*b)) b++;
    const gchar *e = s + strlen(s); while (e>b && g_ascii_isspace(e[-1])) e--;
    return g_strndup(b, (gsize)(e-b));
}

// Checks its numbers
static gboolean parse_number_strict(const gchar *txt, double *out) {
    if (!txt) return FALSE;
    gchar *t = g_strdup(txt);
    g_strstrip(t);
    if (*t == '\0') { g_free(t); return FALSE; }

    // Accept numeric strings only
    gchar *endp = NULL;
    double v = g_ascii_strtod(t, &endp);
    gboolean ok = (endp && *endp == '\0');
    if (ok) *out = v;

    g_free(t);
    return ok;
}

// Collecting the matrix info from the UI (n rows × 3 cols with fixed headers)
gboolean collect_matrix_from_ui(Matrix *ui, MatrixData *out, GError **err) {
    g_return_val_if_fail(ui && out, FALSE);

    const gint rows = ui->rows;
    const gint cols = ui->cols;

    matrix_data_init(out, rows, cols);

    // Add Tool Replacement parameters from the UI spins
    out->price     = ui->amount_price;
    out->lifespan  = ui->amount_lifespan;
    out->profit    = ui->amount_profit;
    out->inflation = ui->amount_inflation;
    out->periods   = ui->amount_period;

    // Read the column names from the labels
    for (gint j = 0; j < cols; ++j) {
        GtkWidget *hdr = g_ptr_array_index(ui->col_headers, j);
        const gchar *h = gtk_label_get_text(GTK_LABEL(hdr));
        g_ptr_array_add(out->col_names, trimdup(h ? h : ""));
    }

    // Read the row names
    for (gint i = 0; i < rows; ++i) {
        GtkEntry *rh = GTK_ENTRY(g_ptr_array_index(ui->row_headers, i));
        const gchar *name = gtk_entry_get_text(rh);
        g_ptr_array_add(out->row_names, trimdup(name));
    }

    // Read the cell values
    for (gint i = 0; i < rows; ++i) {
        for (gint j = 0; j < cols; ++j) {
            GtkEntry *cell = GTK_ENTRY(g_ptr_array_index(ui->cells, i*cols + j));
            const gchar *raw = gtk_entry_get_text(cell);

            // Period column is read-only, but we still save it
            double v;
            if (!parse_number_strict(raw, &v)) {
                GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(cell));
                matrix_data_clear(out);
                g_set_error(err, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                            "Invalid number at row %d, column %d: “%s”",
                            i+1, j+1, raw ? raw : "");
                return FALSE;
            }
            g_array_append_val(out->values, v);
        }
    }

    return TRUE;
}

// Create a matrix for the Tool Replacement algorithm code
static void latex_file_clicked(GtkButton *btn, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;

    // Re-validate focused entry if any
    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(btn));
    if (GTK_IS_WINDOW(toplevel)) {
        GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(toplevel));
        if (focus && GTK_IS_ENTRY(focus))
            mark_cell_validity(focus, ui);
    }
    if (ui->invalid_count > 0) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(toplevel),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "There are invalid cells. Please fix highlighted entries.");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        return;
    }

    // Collect everything from UI
    MatrixData md = {0};
    GError *err = NULL;
    if (!collect_matrix_from_ui(ui, &md, &err)) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(toplevel),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Couldn't build the table:\n%s", err ? err->message : "Unknown error");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        if (err) g_clear_error(&err);
        return;
    }

    const int R = md.rows;          // periods
    const int C = md.cols;          // 3 (Period, Maintenance, Resell)
    if (C < 3) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(toplevel),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Unexpected column count (%d). Expected 3: Period, Maintenance, Resell.", C);
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        matrix_data_clear(&md);
        return;
    }

    // Extract Maintenance and Resell as integer arrays for the algorithm
    int *maintenance = g_new0(int, R);
    int *resell      = g_new0(int, R);

    for (int i = 0; i < R; ++i) {
        double m = g_array_index(md.values, double, i*C + 1);
        double s = g_array_index(md.values, double, i*C + 2);
        maintenance[i] = (int)m;
        resell[i]      = (int)s;
    }

    // Row names
    char **row_names   = (char **) md.row_names->pdata; // size R
    char **column_names= (char **) md.col_names->pdata; // size 3

    // ----- Call Tool Replacement function here -----
    //     int price, int lifespan, int profit, int inflation, int periods,
    //     const int *maintenance, const int *resell

    // runReplacement(years, lifespan, buyPrice, sellPrice, timeMaintenance, inflationPercentage, earnings);


    float* maintenanceCost = malloc(sizeof(float)*md.lifespan);
    float* sellingPrice = malloc(sizeof(float)*md.lifespan);
    for (int x = 0; x < md.lifespan; ++x){
        maintenanceCost[x] = (float)maintenance[x];
        sellingPrice[x]    = (float)resell[x];
    }

    runReplacement(md.periods, md.lifespan, md.price, sellingPrice, maintenanceCost, md.inflation, md.profit);
    //runReplacement(R, C, maintenance, resell);

    // Cleanup
    g_free(maintenance);
    g_free(resell);
    matrix_data_clear(&md);
}

// - - - - - END OF CREATING MATRIX FOR CODE - - - - -

// So that the panels created can't be moved around
void set_panel(GtkPaned *panel, GParamSpec *pspec, gpointer user_data) {
    // Position where division is set
    const int set_pos = 600;
    int current_pos = gtk_paned_get_position(panel);
    if (current_pos != set_pos) {
        gtk_paned_set_position(panel, set_pos);
    }
}

//Main for program 3
int main(int argc, char *argv[]) {
    GtkBuilder *builder;              // Used to obtain the objects from glade
    GtkWidget *ventana;               // Window
    GtkWidget *boton_salida;          // Exit button
    GtkWidget *panel;                 // Panel used to divide the menu and the table created
    GtkWidget *boton_precio;          // Spin button where user enters price of tool
    GtkSpinButton *boton_periodo;     // Spin button where user enters amount of time periods
    GtkWidget *boton_vida;            // Spin button where user enters the tool's lifespan
    GtkSpinButton *boton_ganancia;    // Spin button where user enters the profit of using the tool
    GtkSpinButton *boton_inflacion;   // Spin button where user enters the inflation price
    GtkWidget *boton_guardar;         // Button to save the table in a text file
    GtkWidget *boton_cargar;          // Button to upload a .txt file so that it can be read
    GtkWidget *boton_crear_file;      // Button to create a LATEX and PDF file

    gtk_init(&argc, &argv);

    // CSS - For the red cells
    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_path(prov, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(prov),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(prov);

    // Load glade interface
    builder = gtk_builder_new_from_file("programa-3.glade");

    // Initialize values in the Matrix struct
    Matrix *ui = g_new0(Matrix, 1);
    ui->builder = builder;
    ui->invalid_count = 0;

    // Window
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "program-3"));
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Division panel
    panel = GTK_WIDGET(gtk_builder_get_object(builder, "panel"));
    g_signal_connect(panel, "notify::position", G_CALLBACK(set_panel), NULL);

    // Spin button for price of tool
    boton_precio = GTK_WIDGET(gtk_builder_get_object(builder, "amount-price"));
    g_signal_connect(boton_precio, "value-changed", G_CALLBACK(on_amount_price_changed), ui);
    
    // Spin button for amount of time periods
    boton_periodo = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "amount-period"));
    g_signal_connect(boton_periodo, "value-changed", G_CALLBACK(on_amount_period_changed), ui);

    // Spin button for tool's lifespan
    boton_vida = GTK_WIDGET(gtk_builder_get_object(builder, "amount-lifespan"));
    g_signal_connect(boton_vida, "value-changed", G_CALLBACK(on_amount_lifespan_changed), ui);
    
    // Spin button for profit gained from using tool
    boton_ganancia = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "amount-profit"));
    g_signal_connect(boton_ganancia, "value-changed", G_CALLBACK(on_amount_profit_changed), ui);

    // Spin button for inflation
    boton_inflacion = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "amount-inflation"));
    g_signal_connect(boton_inflacion, "value-changed", G_CALLBACK(on_amount_inflation_changed), ui);

    // Save file button
    boton_guardar = GTK_WIDGET(gtk_builder_get_object(builder, "file-saved"));
    g_signal_connect(boton_guardar, "clicked", G_CALLBACK(save_file), ui);

    // File chooser button
    boton_cargar = GTK_WIDGET(gtk_builder_get_object(builder, "file-chosen"));
    // Makes it an "Open" chooser and filters to only show .txt
    gtk_file_chooser_set_action(GTK_FILE_CHOOSER(boton_cargar), GTK_FILE_CHOOSER_ACTION_OPEN);
    GtkFileFilter *flt = gtk_file_filter_new();
    gtk_file_filter_set_name(flt, "Text files");
    gtk_file_filter_add_pattern(flt, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(boton_cargar), flt);
    // When a file is picked
    g_signal_connect(boton_cargar, "file-set", G_CALLBACK(file_chosen), ui);

    // Create LATEX file button
    boton_crear_file = GTK_WIDGET(gtk_builder_get_object(builder, "latex-file"));
    g_signal_connect(boton_crear_file, "clicked", G_CALLBACK(latex_file_clicked), ui);
    // Termination button
    boton_salida = GTK_WIDGET(gtk_builder_get_object(builder, "exit"));
    g_signal_connect(boton_salida, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    // Initial build
    gint n0 = 1;
    if (boton_periodo)
        n0 = gtk_spin_button_get_value_as_int(boton_periodo);
    if (n0 < 1) n0 = 1;
    rebuild_matrix_ui(ui, n0);

    // Maximize window
    gtk_window_fullscreen(GTK_WINDOW(ventana));

    // Show window
    gtk_widget_show_all(ventana);

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
