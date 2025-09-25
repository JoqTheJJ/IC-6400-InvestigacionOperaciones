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

// - - - - - TABLE IN THE INTERFACE - - - - -
// Structure to be able to create a dynamic matrix
typedef struct {
    GtkBuilder *builder;
    GtkWidget  *grid;
    GPtrArray  *row_headers;
    GPtrArray  *col_headers;
    GPtrArray  *cells;
    gint        rows;          // n
    gint        cols;          // 3
    gint        invalid_count;
    gint        knapsack_value;
} Matrix;

typedef struct {
    gint        rows;       // n rows
    gint        cols;       // 3 (fixed)
    GPtrArray  *row_names;  // n items
    GPtrArray  *col_names;  // 3 items
    GArray     *values;     // table with data
    gint        knapsack;   // capacity of the knapsack
} MatrixData;

// Fixed column names
static const char *FIXED_COLS[3] = {"Cost", "Value", "Amount"};
// Fixed amount of columns
static const guint FIXED_COL_COUNT = 3;
// Column that can take the infinity symbols
#define AMOUNT_COL_IDX 2

// Color of the text of the column names
static GtkCssProvider* get_header_css_provider(void) {
    static GtkCssProvider *prov = NULL;
    if (!prov) {
        prov = gtk_css_provider_new();
        gtk_css_provider_load_from_data(
            prov,
            ".col-header-white { color: white; font-weight: 600; }",
            -1, NULL);
    }
    return prov;
}

// Function for amount a knapsack can hold being changed
static void on_knapsack_changed(GtkSpinButton *spin, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    ui->knapsack_value = gtk_spin_button_get_value_as_int(spin);
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
    gpointer prow = g_object_get_data(G_OBJECT(entry), "row");
    gpointer pcol = g_object_get_data(G_OBJECT(entry), "col");
    int i = prow ? (GPOINTER_TO_INT(prow)-1) : -1;
    int j = pcol ? (GPOINTER_TO_INT(pcol)-1) : -1;

    const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry));
    GtkStyleContext *ctx = gtk_widget_get_style_context(entry);

    gboolean now_invalid = FALSE;

    if (is_blank(txt)) {
        now_invalid = TRUE;
    } else if (is_inf_text(txt)) {
        // ∞ is only allowed in the Amount column
        if (j == AMOUNT_COL_IDX) {
            if (g_strcmp0(txt, "∞") != 0)
                gtk_entry_set_text(GTK_ENTRY(entry), "∞");
            now_invalid = FALSE;
        } else {
            now_invalid = TRUE;
        }
    } else {
        // Only allows positive integers with no other characters
        now_invalid = !integer_digits_only(txt);
    }

    // previous state
    gboolean was_invalid = g_object_get_data(G_OBJECT(entry), "invalid") != NULL;

    // update the invalid cell counter
    if (now_invalid && !was_invalid) {
        ui->invalid_count++;
        g_object_set_data(G_OBJECT(entry), "invalid", GINT_TO_POINTER(1));
    } else if (!now_invalid && was_invalid) {
        ui->invalid_count = MAX(0, ui->invalid_count - 1);
        g_object_set_data(G_OBJECT(entry), "invalid", NULL);
    }

    // Tooltips for invalid cells are tailored by column
    if (now_invalid) {
        if (j == AMOUNT_COL_IDX) {
            gtk_widget_set_tooltip_text(entry,
                "Invalid value. Enter a whole number (0,1,2,…) or 'i'/'I' for ∞ in Amount.");
        } else {
            gtk_widget_set_tooltip_text(entry,
                "Invalid value. Enter a whole number (0,1,2,…).");
        }
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
    gtk_style_context_add_class(ctx, "col-header-white");
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

    // Default value is 0 for all columns
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

    // Clear the old child (table) and rebuild it
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

// Check if the number on the spin button of the amount of nodes has been changed
static void on_spin_value_changed(GtkSpinButton *spin, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;
    gint n = gtk_spin_button_get_value_as_int(spin);
    if (n < 1) n = 1;
    rebuild_matrix_ui(ui, n);  // n rows × 3 cols
}
// - - - - - END OF TABLE IN THE INTERFACE - - - - -

// - - - - - SAVING THE FILE - - - - -

// Tkes care of extra spaces
static gchar* trimmed_copy(const gchar *s) {
    if (!s) return g_strdup("");
    gchar *cpy = g_strdup(s);
    return g_strstrip(cpy);
}

// Makes sure infinities are the symbol ∞ and that the numbers have no weird spaces
static void cell_string_for_export(GtkEntry *entry, gint i, gint j, gchar **out) {
    (void)i; (void)j;

    const gchar *txt0 = gtk_entry_get_text(entry);
    gchar *txt = trimmed_copy(txt0);

    if (is_blank(txt)) { g_free(txt); *out = g_strdup(""); return; }

    // Normalize any accepted infinity token to the symbol
    if (is_inf_text(txt)) { g_free(txt); *out = g_strdup("∞"); return; }

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

    // Check there aren't any invalid cells. If there are, show message saying so
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
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save",   GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Costs-Values-Amounts.txt");

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

    // Compute widths
    size_t *W = g_new0(size_t, (size_t)cols + 1);

    // Row-name width (names column)
    for (gint i = 0; i < rows; ++i) {
        GtkEntry *rh = GTK_ENTRY(g_ptr_array_index(ui->row_headers, i));
        W[0] = MAX(W[0], ulen(gtk_entry_get_text(rh)));
    }

    // Corner text: K=<value>
    gchar *corner = g_strdup_printf("K=%d", ui->knapsack_value);
    W[0] = MAX(W[0], ulen(corner));

    // Column header and the cells widths
    for (gint j = 0; j < cols; ++j) {
        // headers are labels now
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
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Could not open file for writing:\n%s", filename);
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        g_free(filename);
        g_free(W);
        return;
    }

    // Header row: the corner + column labels
    fprint_padded(fp, corner, W[0], FALSE);
    fputc(' ', fp);
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
        fputc(' ', fp);

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
    g_free(corner);

    GtkWidget *done = gtk_message_dialog_new(
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
        "Table saved successfully.");
    gtk_dialog_run(GTK_DIALOG(done));
    gtk_widget_destroy(done);
}
// - - - - - END OF SAVING THE FILE - - - - -

// So that the panels created can't be moved around
void set_panel(GtkPaned *panel, GParamSpec *pspec, gpointer user_data) {
    // Position where division is set
    const int set_pos = 600;
    int current_pos = gtk_paned_get_position(panel);
    if (current_pos != set_pos) {
        gtk_paned_set_position(panel, set_pos);
    }
}


// - - - - - UPLOADING A FILE - - - - -

// Check the number in the corner
static gboolean parse_knapsack_corner(const gchar *tok, gint *out_val) {
    if (!tok || !*tok) return FALSE;

    // Accept K=<Value> form
    if ((tok[0]=='K' || tok[0]=='k') && tok[1]=='=') {
        const gchar *p = tok + 2;
        for (const gchar *q=p; *q; ++q) if (!g_ascii_isdigit(*q)) return FALSE;
        if (*p=='\0') return FALSE;
        *out_val = (gint)g_ascii_strtoll(p, NULL, 10);
        return TRUE;
    }

    // Accept only an integer
    for (const gchar *q=tok; *q; ++q) if (!g_ascii_isdigit(*q)) return FALSE;
    *out_val = (gint)g_ascii_strtoll(tok, NULL, 10);
    return TRUE;
}

// It checks wether a string contains at least one ASCII digit
static gboolean has_ascii_digit(const gchar *s) {
    if (!s) return FALSE;
    for (const gchar *p = s; *p; ++p)
        if (g_ascii_isdigit(*p)) return TRUE;
    return FALSE;
}

// Checks if it's either the infinity symbol or a positive integer (digits only)
static gboolean is_value_token_strict(const gchar *t) {
    if (!t) return FALSE;
    gchar *s = g_strdup(t);
    g_strstrip(s);

    gboolean ok = FALSE;
    if (*s) {
        // Infinity symbol allowed
        if (g_strcmp0(s, "∞") == 0) {
            ok = TRUE;
        } else {
            // Check all characters are digits
            ok = TRUE;
            for (const char *p = s; *p; p++) {
                if (!g_ascii_isdigit(*p)) {
                    ok = FALSE;
                    break;
                }
            }
        }
    }

    g_free(s);
    return ok;
}

// Checks if the symbol for infinity is used so that it doesn't also use things like Inf or I
static gboolean is_infinity_symbol_only(const gchar *t) {
    if (!t) return FALSE;
    gchar *s = g_strdup(t);
    g_strstrip(s);
    gboolean r = (g_strcmp0(s, "∞") == 0);
    g_free(s);
    return r;
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

// Block a specific handler while setting a text to avoid infinite loops
static void set_text_blocked(GtkEntry *entry, const gchar *text, GCallback handler,
                             gpointer handler_data) {
    g_signal_handlers_block_by_func(entry, handler, handler_data);
    gtk_entry_set_text(entry, text);
    g_signal_handlers_unblock_by_func(entry, handler, handler_data);
}

// Case-insensitive, trim-aware equality
static gboolean equal_ci_trim(const gchar *a, const gchar *b) {
    if (!a || !b) return FALSE;
    gchar *sa = g_strdup(a), *sb = g_strdup(b);
    g_strstrip(sa); g_strstrip(sb);
    gboolean ok = (g_ascii_strcasecmp(sa, sb) == 0);
    g_free(sa); g_free(sb);
    return ok;
}

// Check the header names are "Cost Value Amount"
static gboolean header_matches_fixed(gchar **head, guint headc) {
    if (headc < FIXED_COL_COUNT) return FALSE;

    return equal_ci_trim(head[headc-3], FIXED_COLS[0]) &&
           equal_ci_trim(head[headc-2], FIXED_COLS[1]) &&
           equal_ci_trim(head[headc-1], FIXED_COLS[2]);
}

// Opening a file and seeing the table in the interface
static void file_chosen(GtkFileChooserButton *btn, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;

    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(btn));
    if (!filename) return;

    // Read a file
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

    // Keep only non-empty lines
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

    // First line is the header
    gchar **head = split_ws(rows->pdata[0]);
    guint headc  = g_strv_length(head);

    // Get the knapsack caapcity from the corner value
    gint knap = -1;
    if (headc >= 1 && parse_knapsack_corner(head[0], &knap)) {
        // Update the stored value
        ui->knapsack_value = knap;

        // Also show it in the spin button
        GtkSpinButton *spin_knap = GTK_SPIN_BUTTON(
            gtk_builder_get_object(ui->builder, "amount-knapsack"));
        if (spin_knap) gtk_spin_button_set_value(spin_knap, knap);
    }

    if (!header_matches_fixed(head, headc)) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Header does not end with the required columns:\n\"Cost Value Amount\".\n"
            "I'll proceed anyway and force those column names.");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
    }

    // Data row count
    guint data0 = 1u;
    guint nrows = rows->len - data0;
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

    // Build UI as n rows × 3
    rebuild_matrix_ui(ui, (gint)nrows);
    ui->invalid_count = 0;

    // Force the fixed column labels just in case
    for (guint j = 0; j < ui->cols; ++j) {
        GtkWidget *cw = g_ptr_array_index(ui->col_headers, j);
        gtk_label_set_text(GTK_LABEL(cw), FIXED_COLS[j]);
    }

    // Check each data row
    for (guint r = 0; r < nrows; ++r) {
        gchar **tok = split_ws(rows->pdata[data0 + r]);
        guint tokc = g_strv_length(tok);

        if (tokc < (1 + FIXED_COL_COUNT)) {
            GtkWidget *d = gtk_message_dialog_new(
                GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Row %u has %u token(s); expected at least %u (name + 3 values).",
                r+1, tokc, 1 + FIXED_COL_COUNT);
            gtk_dialog_run(GTK_DIALOG(d));
            gtk_widget_destroy(d);
            g_strfreev(tok);
            continue;
        }

        guint vstart = tokc - FIXED_COL_COUNT;

        // Obtain the row names
        gchar *saved = tok[vstart];
        tok[vstart] = NULL;
        gchar *rname = g_strjoinv(" ", tok);
        tok[vstart] = saved;

        // Set the row header text
        GtkEntry *rh = GTK_ENTRY(g_ptr_array_index(ui->row_headers, r));
        gtk_entry_set_text(rh, rname ? rname : "");
        g_free(rname);

        // Fill the cells
        for (guint j = 0; j < ui->cols; ++j) {
            GtkWidget *cell = GTK_WIDGET(g_ptr_array_index(ui->cells, r * ui->cols + j));
            const gchar *vstr = tok[vstart + j];

            // Strict validation: its either a "∞" or digits only
            if (!is_value_token_strict(vstr)) {
                // Set the text anyway so the user sees the error
                gtk_entry_set_text(GTK_ENTRY(cell), vstr);
                mark_cell_validity(cell, ui);
            } else {
                if (is_infinity_symbol_only(vstr)) {
                    gtk_entry_set_text(GTK_ENTRY(cell), "∞");
                } else {
                    gtk_entry_set_text(GTK_ENTRY(cell), vstr);
                }
                // Check the cell validity to clear any previous error styling
                mark_cell_validity(cell, ui);
            }
        }

        g_strfreev(tok);
    }

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
    md->knapsack  = 0;
}

// Clean up
static void matrix_data_clear(MatrixData *md) {
    if (!md) return;
    if (md->row_names) g_ptr_array_free(md->row_names, TRUE);
    if (md->col_names) g_ptr_array_free(md->col_names, TRUE);
    if (md->values)    g_array_free(md->values, TRUE);
    md->rows = md->cols = 0;
    md->knapsack = 0;
    md->row_names = md->col_names = NULL;
    md->values = NULL;
}

// Cleans up spaces so that there aren't any extra lying around
static inline gchar *trimdup(const gchar *s) {
    if (!s) return g_strdup("");
    gsize len = g_utf8_strlen(s, -1);
    const gchar *b = s; while (*b && g_ascii_isspace(*b)) b++;
    const gchar *e = s + strlen(s); while (e>b && g_ascii_isspace(e[-1])) e--;
    return g_strndup(b, (gsize)(e-b));
}

static gboolean parse_number_or_inf(const gchar *txt, double *out) {
    // Accept: "∞"
    if (!txt) return FALSE;
    gchar *t = trimdup(txt);
    for (gchar *p = t; *p; ++p) *p = g_ascii_tolower(*p);

    gboolean is_inf = (g_strcmp0(t, "∞") == 0);
    if (is_inf) { *out = (double)__INT_MAX__;
        g_free(t);
        return TRUE;
    }

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

            gchar *s = NULL;
            cell_string_for_export(cell, i, j, &s);
            double v;
            if (!parse_number_or_inf(s, &v)) {
                GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(cell));
                matrix_data_clear(out);
                g_set_error(err, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                            "Invalid number at row %d, column %d: “%s”",
                            i+1, j+1, s ? s : "");
                g_free(s);
                return FALSE;
            }
            g_array_append_val(out->values, v);
            g_free(s);
        }
    }

    out->knapsack = ui->knapsack_value;

    return TRUE;
}

// Create a matrix for the Knapsack algorithm code
static void latex_file_clicked(GtkButton *btn, gpointer user_data) {
    Matrix *ui = (Matrix*)user_data;

    // Check if there are any invalid cells
    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(btn));
    if (GTK_IS_WINDOW(toplevel)) {
        GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(toplevel));
        if (focus && GTK_IS_ENTRY(focus)) {
            mark_cell_validity(focus, ui);
        }
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

    // Collect the matrix and names data
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
    // Number of items
    const int R = md.rows;
    // Amount of columns
    const int C = md.cols;

    // Extract the Cost/Value/Amount columns as integer arrays
    int *cost   = g_new0(int, R);
    int *value  = g_new0(int, R);
    int *amount = g_new0(int, R);

    for (int i = 0; i < R; ++i) {
        double c = g_array_index(md.values, double, i*C + 0);
        double v = g_array_index(md.values, double, i*C + 1);
        double a = g_array_index(md.values, double, i*C + 2);
        cost[i]   = (int)c;
        value[i]  = (int)v;
        amount[i] = (int)a;
    }

    // Row names (item names)
    char **row_names = (char **) md.row_names->pdata;
    // Column names
    char **column_names = (char **) md.col_names->pdata;

    // Call Knapsack algorithm here
    // int objects, int capacity, int* profits, int* costs, int* quantity
    // where:
    //objects = R
    //capacity = ui->knapsack_value
    //profits = value[]
    //costs = cost[]
    //quantity = amount[]

    // Cleanup
    g_free(cost);
    g_free(value);
    g_free(amount);
    matrix_data_clear(&md);
}

// - - - - - END OF CREATING MATRIX FOR CODE - - - - -

//Main for program 1
int main(int argc, char *argv[]) {
    GtkBuilder *builder;            // Used to obtain the objects from glade
    GtkWidget *ventana;             // Window
    GtkWidget *boton_salida;        // Exit button
    GtkWidget *panel;               // Panel used to divide the menu and the table created
    GtkWidget *boton_objetos;       // Spin button where user enters amount of objects
    GtkSpinButton *boton_mochila;   // Spin button where user enters weight of knapsack
    GtkWidget *boton_guardar;       // Button to save the table in a text file
    GtkWidget *boton_cargar;        // Button to upload a .txt file so that it can be read
    GtkWidget *boton_crear_file;    // Button to create a LATEX and PDF file

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

    // Spin button for objects
    boton_objetos = GTK_WIDGET(gtk_builder_get_object(builder, "amount-objects"));
    g_signal_connect(boton_objetos, "value-changed", G_CALLBACK(on_spin_value_changed), ui);
    
    // Spin button for knapsack size
    boton_mochila = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "amount-knapsack"));
    ui->knapsack_value = gtk_spin_button_get_value_as_int(boton_mochila);
    g_signal_connect(boton_mochila, "value-changed", G_CALLBACK(on_knapsack_changed), ui);

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
    gint n0 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(boton_objetos));
    if (n0 < 1) n0 = 1;
    rebuild_matrix_ui(ui, n0);

    GdkPixbuf *pix = gdk_pixbuf_new_from_file_at_scale(
        "backpack-map.png", // your image file
        64,                 // desired width
        -1,                 // height (-1 = auto to keep aspect ratio)
        TRUE,               // preserve aspect ratio
        NULL                // error
    );
    GtkWidget *img = gtk_image_new_from_pixbuf(pix);

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
    //g_object_unref(pix);
    g_object_unref(builder);

    return 0;
}
