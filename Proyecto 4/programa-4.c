/*
                Project 4: Calculating the SIMPLEX Algorithm
                Made by: Carmen Hidalgo Paz, Melissa Carvajal Charpentier
                y Josué Soto González
                Date: Wednesday, November 12, 2025

                This section contains the interface of the program used to
                calculate the SIMPLEX algorithm. It will then create a LATEX
                document with all the tables calculated to get to the result.

*/

#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <glib/gstdio.h>
#include  "simplex.c"

// - - - - - TABLE IN THE INTERFACE - - - - -
// Structure to be able to create a dynamic matrix
typedef enum {
    OP_LEQ = 0,
    OP_GEQ = 1,
    OP_EQ  = 2
} ConstraintOp;

typedef struct {
    GtkBuilder *builder;

    // Widgets
    GtkWidget  *spin_vars;          // amount of variables
    GtkWidget  *spin_restr;         // amount of restrictions
    GtkWidget  *name_problem;       // name of the problem
    GtkWidget  *rb_max;             // maximize
    GtkWidget  *rb_min;             // minimize
    GtkWidget  *cb_intermediate;    // intermediate-tables

    // Containers
    GtkWidget  *scroll;     // the single scrolled window from Glade
    GtkWidget  *vbox;       // lives inside the scroll's viewport, holds both grids stacked

    // Current grids
    GtkWidget  *grid_obj;
    GtkWidget  *grid_con;

    // Counts
    gint n_vars;    // from spin_vars
    gint n_restr;   // from spin_restr

    // Objective
    GPtrArray  *obj_coef_entries;   // GtkEntry* for coefficients
    GPtrArray  *var_name_entries;   // GtkEntry* for variable names

    // Constraints
    GPtrArray  *con_coef_entries;
    GPtrArray  *con_op_combos;
    GPtrArray  *con_rhs_entries;
    GPtrArray *con_header_labels;

    // validation counter
    gint invalid_count;
} SimplexUI;

// Check if cell is blank
static gboolean is_blank(const gchar *s) {
    if (!s) return TRUE;
    for (const gchar *p = s; *p; ++p) if (!g_ascii_isspace(*p)) return FALSE;
    return TRUE;
}

static void clear_child(GtkWidget *container) {
    GList *kids = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *l = kids; l; l = l->next) gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(kids);
}

static void clear_box_children(GtkWidget *box) {
    GList *kids = gtk_container_get_children(GTK_CONTAINER(box));
    for (GList *l = kids; l; l = l->next) gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(kids);
}

static void ensure_scroll_box(SimplexUI *ui) {
    if (ui->vbox) return;

    // Clear any previous child of the scrolled window
    clear_child(ui->scroll);

    // Create viewport and vbox
    GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(ui->scroll), viewport);

    ui->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(viewport), ui->vbox);
}

// Accepts real numbers that are positive
static gboolean parse_nonneg_real(const gchar *txt, double *out) {
    if (!txt) return FALSE;
    gchar *endptr = NULL;
    errno = 0;
    double v = g_ascii_strtod(txt, &endptr);
    if (errno != 0) return FALSE;
    if (endptr == txt) return FALSE;
    // Skip trailing spaces
    while (*endptr && g_ascii_isspace((guchar)*endptr)) endptr++;
    if (*endptr != '\0') return FALSE;
    if (v < 0.0) return FALSE;
    if (out) *out = v;
    return TRUE;
}

static void set_entry_error(GtkWidget *entry, gboolean is_error, const gchar *tip) {
    GtkStyleContext *ctx = gtk_widget_get_style_context(entry);
    if (is_error) {
        gtk_style_context_add_class(ctx, "entry-error");
        gtk_widget_set_tooltip_text(entry, tip ? tip : "Invalid value.");
    } else {
        gtk_style_context_remove_class(ctx, "entry-error");
        gtk_widget_set_tooltip_text(entry, NULL);
    }
}

// Accepts any real number
static gboolean parse_real(const gchar *txt, double *out) {
    if (!txt) return FALSE;
    gchar *endptr = NULL;
    errno = 0;
    double v = g_ascii_strtod(txt, &endptr);
    if (errno != 0) return FALSE;
    if (endptr == txt) return FALSE;
    while (*endptr && g_ascii_isspace((guchar)*endptr)) endptr++;
    if (*endptr != '\0') return FALSE;
    if (out) *out = v;
    return TRUE;
}

// Accept any real number for constraint coefficients
static gboolean coef_focus_out_cb(GtkWidget *entry, GdkEvent *e, gpointer data) {
    (void)e;
    const gchar *t = gtk_entry_get_text(GTK_ENTRY(entry));
    double v;
    gboolean bad = is_blank(t) || !parse_real(t, &v);
    set_entry_error(entry, bad, "Enter a real number.");
    return FALSE;
}

// Require positive real numbers for RHS only
static gboolean rhs_focus_out_cb(GtkWidget *entry, GdkEvent *e, gpointer data) {
    (void)e;
    const gchar *t = gtk_entry_get_text(GTK_ENTRY(entry));
    double v;
    gboolean bad = is_blank(t) || !parse_nonneg_real(t, &v);
    set_entry_error(entry, bad, "Enter a real number ≥ 0 (required).");
    return FALSE;
}

// Naming the varibales
static gchar *default_var_name(gint j) {
    static const gunichar subs[] = {0x2080,0x2081,0x2082,0x2083,0x2084,0x2085,0x2086,0x2087,0x2088,0x2089};
    GString *s = g_string_new("x");
    gint k = j + 1;
    // build subscript digits
    GString *sub = g_string_new(NULL);
    if (k == 0) g_string_append_unichar(sub, subs[0]);
    while (k > 0) { g_string_prepend_unichar(sub, subs[k % 10]); k /= 10; }
    g_string_append(sub, "");
    g_string_append(s, sub->str);
    g_string_free(sub, TRUE);
    return g_string_free(s, FALSE);
}

// When the name of the variables changes
static void on_var_name_changed(GtkEditable *e, gpointer user_data) {
    SimplexUI *ui = (SimplexUI*)user_data;
    gpointer pidx = g_object_get_data(G_OBJECT(e), "index");
    if (!pidx) return;
    gint j = GPOINTER_TO_INT(pidx) - 1;

    if (!ui->con_header_labels) return;
    if (j < 0 || j >= (gint)ui->con_header_labels->len) return;

    GtkWidget *hdr = g_ptr_array_index(ui->con_header_labels, j);
    if (hdr && GTK_IS_LABEL(hdr)) {
        const gchar *name = gtk_entry_get_text(GTK_ENTRY(e));
        gtk_label_set_text(GTK_LABEL(hdr), name);
    }
}

static GtkWidget* make_coef_entry(SimplexUI *ui, gint j) {
    GtkWidget *e = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(e), 0.5);
    gtk_entry_set_width_chars(GTK_ENTRY(e), 8);
    gtk_entry_set_text(GTK_ENTRY(e), "0");
    g_signal_connect(e, "focus-out-event", G_CALLBACK(coef_focus_out_cb), ui);
    return e;
}

static GtkWidget* make_name_entry(SimplexUI *ui, gint j) {
    GtkWidget *e = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(e), 0.5);
    gtk_entry_set_width_chars(GTK_ENTRY(e), 8);
    gchar *def = default_var_name(j);
    gtk_entry_set_text(GTK_ENTRY(e), def);
    g_free(def);
    g_object_set_data(G_OBJECT(e), "index", GINT_TO_POINTER(j+1));
    g_signal_connect(e, "changed", G_CALLBACK(on_var_name_changed), ui);
    return e;
}

// Show the objective function table
static void rebuild_objective(SimplexUI *ui) {
    ensure_scroll_box(ui);

    // Remove old the objective function grid if it's present
    if (ui->grid_obj) gtk_widget_destroy(ui->grid_obj);

    ui->grid_obj = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(ui->grid_obj), 4);
    gtk_grid_set_column_spacing(GTK_GRID(ui->grid_obj), 6);
    gtk_box_pack_start(GTK_BOX(ui->vbox), ui->grid_obj, FALSE, FALSE, 0);

    if (ui->obj_coef_entries) { g_ptr_array_free(ui->obj_coef_entries, TRUE); ui->obj_coef_entries = NULL; }
    if (ui->var_name_entries) { g_ptr_array_free(ui->var_name_entries, TRUE); ui->var_name_entries = NULL; }
    ui->obj_coef_entries = g_ptr_array_new_with_free_func(g_object_unref);
    ui->var_name_entries = g_ptr_array_new_with_free_func(g_object_unref);

    GtkWidget *lbl = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl), "<b>Z</b> =");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl), "instructions");
    gtk_grid_attach(GTK_GRID(ui->grid_obj), lbl, 0, 0, 1, 1);

    int col = 1;
    for (gint j = 0; j < ui->n_vars; ++j) {
        GtkWidget *e_coef = make_coef_entry(ui, j);
        GtkWidget *e_name = make_name_entry(ui, j);

        g_ptr_array_add(ui->obj_coef_entries, g_object_ref(e_coef));
        g_ptr_array_add(ui->var_name_entries, g_object_ref(e_name));

        gtk_grid_attach(GTK_GRID(ui->grid_obj), e_coef, col++, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(ui->grid_obj), e_name, col++, 0, 1, 1);

        if (j != ui->n_vars - 1) {
            GtkWidget *plus = gtk_label_new("+");
            gtk_style_context_add_class(gtk_widget_get_style_context(plus), "instructions");
            gtk_grid_attach(GTK_GRID(ui->grid_obj), plus, col++, 0, 1, 1);
        }
    }

    gtk_widget_show_all(ui->grid_obj);
}

static GtkWidget* make_operator_combo(void) {
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_BOOLEAN); // text, sensitive
    GtkTreeIter it;

    // ≤ enabled
    gtk_list_store_append(store, &it);
    gtk_list_store_set(store, &it, 0, "≤", 1, TRUE, -1);

    // ≥ disabled
    gtk_list_store_append(store, &it);
    gtk_list_store_set(store, &it, 0, "≥", 1, FALSE, -1);

    // = disabled
    gtk_list_store_append(store, &it);
    gtk_list_store_set(store, &it, 0, "=", 1, FALSE, -1);

    GtkWidget *combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    GtkCellRenderer *rend = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), rend, TRUE);
    gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combo), rend, "text", 0);
    gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combo), rend, "sensitive", 1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0); // default ≤
    gtk_widget_set_tooltip_text(combo, "Only ≤ is enabled for now.");

    return combo;
}

static GtkWidget* make_rhs_entry(SimplexUI *ui) {
    GtkWidget *e = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(e), 0.5);
    gtk_entry_set_width_chars(GTK_ENTRY(e), 10);
    g_signal_connect(e, "focus-out-event", G_CALLBACK(rhs_focus_out_cb), ui);
    return e;
}

static GtkWidget* make_coef_entry_con(SimplexUI *ui) {
    GtkWidget *e = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(e), 0.5);
    gtk_entry_set_width_chars(GTK_ENTRY(e), 8);
    gtk_entry_set_text(GTK_ENTRY(e), "0");
    g_signal_connect(e, "focus-out-event", G_CALLBACK(coef_focus_out_cb), ui);
    return e;
}

// Show constraints table
static void rebuild_constraints(SimplexUI *ui) {
    ensure_scroll_box(ui);
    if (ui->grid_con) gtk_widget_destroy(ui->grid_con);

    ui->grid_con = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(ui->grid_con), 3);
    gtk_grid_set_column_spacing(GTK_GRID(ui->grid_con), 6);
    gtk_box_pack_start(GTK_BOX(ui->vbox), ui->grid_con, FALSE, FALSE, 0);

    if (ui->con_coef_entries)  { g_ptr_array_free(ui->con_coef_entries, TRUE);  ui->con_coef_entries  = NULL; }
    if (ui->con_op_combos)     { g_ptr_array_free(ui->con_op_combos, TRUE);     ui->con_op_combos     = NULL; }
    if (ui->con_rhs_entries)   { g_ptr_array_free(ui->con_rhs_entries, TRUE);   ui->con_rhs_entries   = NULL; }
    if (ui->con_header_labels) { g_ptr_array_free(ui->con_header_labels, TRUE); ui->con_header_labels = NULL; }

    ui->con_coef_entries  = g_ptr_array_new_with_free_func(g_object_unref);
    ui->con_op_combos     = g_ptr_array_new_with_free_func(g_object_unref);
    ui->con_rhs_entries   = g_ptr_array_new_with_free_func(g_object_unref);
    ui->con_header_labels = g_ptr_array_new_with_free_func(g_object_unref);

    // Header row: names with plus signs between them
    gtk_grid_attach(GTK_GRID(ui->grid_con), gtk_label_new(" "), 0, 0, 1, 1);
    for (gint j = 0; j < ui->n_vars; ++j) {
        const gchar *nm = "x";
        if (ui->var_name_entries && j < (gint)ui->var_name_entries->len)
            nm = gtk_entry_get_text(GTK_ENTRY(g_ptr_array_index(ui->var_name_entries, j)));

        GtkWidget *lbl = gtk_label_new(nm);
        g_ptr_array_add(ui->con_header_labels, g_object_ref(lbl));
        gtk_grid_attach(GTK_GRID(ui->grid_con), lbl, (j*2)+1, 0, 1, 1);
        gtk_style_context_add_class(gtk_widget_get_style_context(lbl), "instructions");

        if (j != ui->n_vars - 1) {
            GtkWidget *plus = gtk_label_new("+");
            gtk_style_context_add_class(gtk_widget_get_style_context(plus), "instructions");
            gtk_grid_attach(GTK_GRID(ui->grid_con), plus, (j*2)+2, 0, 1, 1);
        }
    }

    // Operator and RHS columns shifted right
    gint base = ui->n_vars * 2 + 1;
    GtkWidget *lbl_op = gtk_label_new("Operator");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_op), "instructions");
    gtk_grid_attach(GTK_GRID(ui->grid_con), lbl_op, base, 0, 1, 1);

    GtkWidget *lbl_num = gtk_label_new("Number");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_num), "instructions");
    gtk_grid_attach(GTK_GRID(ui->grid_con), lbl_num, base + 1, 0, 1, 1);

    // Rows
    for (gint r = 0; r < ui->n_restr; ++r) {
        gchar *rname = g_strdup_printf("R%d", r+1);
        gtk_grid_attach(GTK_GRID(ui->grid_con), gtk_label_new(rname), 0, r+1, 1, 1);
        g_free(rname);

        for (gint j = 0; j < ui->n_vars; ++j) {
            GtkWidget *e = make_coef_entry_con(ui);
            g_ptr_array_add(ui->con_coef_entries, g_object_ref(e));

            gtk_grid_attach(GTK_GRID(ui->grid_con), e, (j*2)+1, r+1, 1, 1);

            // Plus sign between coefficients
            if (j != ui->n_vars - 1) {
                GtkWidget *plus = gtk_label_new("+");
                gtk_style_context_add_class(gtk_widget_get_style_context(plus), "instructions");
                gtk_grid_attach(GTK_GRID(ui->grid_con), plus, (j*2)+2, r+1, 1, 1);
            }
        }

        GtkWidget *op  = make_operator_combo();
        GtkWidget *rhs = make_rhs_entry(ui);
        g_ptr_array_add(ui->con_op_combos, g_object_ref(op));
        g_ptr_array_add(ui->con_rhs_entries, g_object_ref(rhs));

        gtk_grid_attach(GTK_GRID(ui->grid_con), op,  base,   r+1, 1, 1);
        gtk_grid_attach(GTK_GRID(ui->grid_con), rhs, base+1, r+1, 1, 1);
    }

    gtk_widget_show_all(ui->grid_con);
}

// Show both tables
static void rebuild_all(SimplexUI *ui) {
    ui->n_vars  = MAX(2, MIN(15, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->spin_vars))));
    ui->n_restr = MAX(2, MIN(15, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->spin_restr))));
    ui->invalid_count = 0;

    ensure_scroll_box(ui);
    clear_box_children(ui->vbox);

    // wipe the previous section titles, the separator and the tables
    clear_box_children(ui->vbox);
    ui->grid_obj = ui->grid_con = NULL;

    // titles and tables
    GtkWidget *lbl_obj = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_obj), "<b>Objective Function</b>");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_obj), "instructions");
    gtk_box_pack_start(GTK_BOX(ui->vbox), lbl_obj, FALSE, FALSE, 2);

    rebuild_objective(ui);

    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_margin_top(sep, 8);
    gtk_widget_set_margin_bottom(sep, 8);
    gtk_box_pack_start(GTK_BOX(ui->vbox), sep, FALSE, FALSE, 0);

    GtkWidget *lbl_cons = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_cons), "<b>Constraints</b>");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_cons), "instructions");
    gtk_box_pack_start(GTK_BOX(ui->vbox), lbl_cons, FALSE, FALSE, 2);

    rebuild_constraints(ui);

    gtk_widget_show_all(ui->vbox);
}

static void on_spin_changed(GtkSpinButton *spin, gpointer user_data) {
    SimplexUI *ui = (SimplexUI*)user_data;
    rebuild_all(ui);
}

// - - - - - END OF TABLE IN THE INTERFACE - - - - -


// - - - - - SAVING THE FILE - - - - -

// Trim copy
static gchar* trimmed_copy(const gchar *s) {
    if (!s) return g_strdup("");
    gchar *cpy = g_strdup(s);
    return g_strstrip(cpy);
}

// UTF-8 char length (for padding)
static size_t ulen(const gchar *s) {
    return g_utf8_strlen(s ? s : "", -1);
}

// Padded print
static void fprint_padded(FILE *fp, const gchar *s, size_t width, gboolean right_align) {
    size_t len = ulen(s);
    size_t pad = (width > len) ? (width - len) : 0;
    if (right_align) { for (size_t k=0;k<pad;k++) fputc(' ', fp); fputs(s, fp); }
    else { fputs(s, fp); for (size_t k=0;k<pad;k++) fputc(' ', fp); }
}

static gboolean entry_to_nonneg_str(GtkEntry *e, gchar **out_err) {
    const gchar *t0 = gtk_entry_get_text(e);
    gchar *t = trimmed_copy(t0);
    double v;
    if (is_blank(t) || !parse_nonneg_real(t, &v)) {
        if (out_err) *out_err = g_strdup("Enter a real number ≥ 0.");
        g_free(t);
        return FALSE;
    }
    g_free(t);
    return TRUE;
}

static gboolean rhs_entry_to_nonneg_str(GtkEntry *e, gchar **out_err) {
    const gchar *t0 = gtk_entry_get_text(e);
    gchar *t = trimmed_copy(t0);
    double v;
    if (is_blank(t) || !parse_nonneg_real(t, &v)) {
        if (out_err) *out_err = g_strdup("RHS is required and must be a real number ≥ 0.");
        g_free(t);
        return FALSE;
    }
    g_free(t);
    return TRUE;
}

// Accept any real number
static gboolean entry_to_real_str(GtkEntry *e, gchar **out_err) {
    const gchar *t0 = gtk_entry_get_text(e);
    gchar *t = trimmed_copy(t0);
    double v;
    if (is_blank(t) || !parse_real(t, &v)) {
        if (out_err) *out_err = g_strdup("Enter a real number.");
        g_free(t);
        return FALSE;
    }
    g_free(t);
    return TRUE;
}

// Save the file
static void save_simplex_file(GtkButton *btn, gpointer user_data) {
    SimplexUI *ui = (SimplexUI*)user_data;

    // Force validation on the currently focused entry
    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(btn));
    if (GTK_IS_WINDOW(toplevel)) {
        GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(toplevel));
        if (focus && GTK_IS_ENTRY(focus)) {
            // trigger focus-out handlers
            g_signal_emit_by_name(focus, "focus-out-event", NULL);
        }
    }

    // Validate all numeric entries (objective coeffs, constraint coeffs, RHS)
    for (guint j = 0; j < ui->obj_coef_entries->len; ++j) {
        GtkEntry *e = GTK_ENTRY(g_ptr_array_index(ui->obj_coef_entries, j));
        gchar *err = NULL;
        if (!entry_to_real_str(e, &err)) {
            GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(toplevel),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Invalid objective coefficient at position %u. %s", j+1, err ? err : "");
            gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
            g_free(err);
            return;
        }
        g_free(err);
    }
    for (guint idx = 0; idx < ui->con_coef_entries->len; ++idx) {
        GtkEntry *e = GTK_ENTRY(g_ptr_array_index(ui->con_coef_entries, idx));
        gchar *err = NULL;
        if (!entry_to_real_str(e, &err)) {
            GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(toplevel),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Invalid constraint coefficient at index %u. %s", idx+1, err ? err : "");
            gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
            g_free(err);
            return;
        }
        g_free(err);
    }
    for (guint r = 0; r < ui->con_rhs_entries->len; ++r) {
        GtkEntry *e = GTK_ENTRY(g_ptr_array_index(ui->con_rhs_entries, r));
        gchar *err = NULL;
        if (!rhs_entry_to_nonneg_str(e, &err)) {
            GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(toplevel),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Invalid right-hand side in row R%u. %s", r+1, err ? err : "");
            gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
            g_free(err);
            return;
        }
        g_free(err);
    }

    // the save dialog
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Save Simplex Problem As:",
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save",   GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "simplex-problem.txt");

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

    FILE *fp = g_fopen(filename, "w");
    if (!fp) {
        GtkWidget *d = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Could not open file for writing:\n%s", filename);
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        g_free(filename);
        return;
    }

    // ---------- HEADER ----------
    const gchar *pname = gtk_entry_get_text(GTK_ENTRY(ui->name_problem));
    gboolean is_max = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ui->rb_max));
    gboolean show_inter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ui->cb_intermediate));

    fprintf(fp, "# SIMPLEX-UI v1\n");
    fprintf(fp, "Name: %s\n", pname ? pname : "");
    fprintf(fp, "Sense: %s\n", is_max ? "MAX" : "MIN");
    fprintf(fp, "Variables: %d\n", ui->n_vars);
    fprintf(fp, "Constraints: %d\n", ui->n_restr);
    fprintf(fp, "IntermediateTables: %d\n", show_inter ? 1 : 0);
    fputc('\n', fp);

    // ---------- OBJECTIVE ----------
    fprintf(fp, "[Objective]\n");

    // Gather widths for pretty alignment
    size_t *W = g_new0(size_t, ui->n_vars);
    for (gint j = 0; j < ui->n_vars; ++j) {
        const gchar *name = gtk_entry_get_text(GTK_ENTRY(g_ptr_array_index(ui->var_name_entries, j)));
        const gchar *coef = gtk_entry_get_text(GTK_ENTRY(g_ptr_array_index(ui->obj_coef_entries, j)));
        W[j] = MAX(ulen(name), ulen(coef));
    }

    // Names row
    fputs("names: ", fp);
    for (gint j = 0; j < ui->n_vars; ++j) {
        const gchar *name = gtk_entry_get_text(GTK_ENTRY(g_ptr_array_index(ui->var_name_entries, j)));
        fprint_padded(fp, name, W[j], FALSE);
        if (j < ui->n_vars - 1) fputc(' ', fp);
    }
    fputc('\n', fp);

    // Coeffs row
    fputs("coeffs:", fp); fputc(' ', fp);
    for (gint j = 0; j < ui->n_vars; ++j) {
        const gchar *coef = gtk_entry_get_text(GTK_ENTRY(g_ptr_array_index(ui->obj_coef_entries, j)));
        fprint_padded(fp, coef, W[j], TRUE);
        if (j < ui->n_vars - 1) fputc(' ', fp);
    }
    fputc('\n', fp);
    fputc('\n', fp);

    // ---------- CONSTRAINTS ----------
    fprintf(fp, "[Constraints]\n");

    // Header line
    fputs("       ", fp);
    for (gint j = 0; j < ui->n_vars; ++j) {
        const gchar *name = gtk_entry_get_text(GTK_ENTRY(g_ptr_array_index(ui->var_name_entries, j)));
        fprint_padded(fp, name, W[j], FALSE);
        if (j < ui->n_vars - 1) fputc(' ', fp);
    }
    fputs("   | op | rhs\n", fp);

    // Rows
    for (gint r = 0; r < ui->n_restr; ++r) {
        // R label
        gchar *rname = g_strdup_printf("R%d", r+1);
        fprint_padded(fp, rname, 5, FALSE);
        g_free(rname);
        fputc(' ', fp);

        // coefficients
        for (gint j = 0; j < ui->n_vars; ++j) {
            GtkEntry *e = GTK_ENTRY(g_ptr_array_index(ui->con_coef_entries, r*ui->n_vars + j));
            const gchar *coef = gtk_entry_get_text(e);
            fprint_padded(fp, coef, W[j], TRUE);
            if (j < ui->n_vars - 1) fputc(' ', fp);
        }

        // operator
        fputs(" | ", fp);
        GtkComboBox *cmb = GTK_COMBO_BOX(g_ptr_array_index(ui->con_op_combos, r));
        gint active = gtk_combo_box_get_active(cmb);
        const char *op_txt = "≤";
        if (active == OP_GEQ) op_txt = "≥";
        else if (active == OP_EQ) op_txt = "=";
        fputs(op_txt, fp);

        // rhs
        fputs(" | ", fp);
        GtkEntry *rhs_e = GTK_ENTRY(g_ptr_array_index(ui->con_rhs_entries, r));
        const gchar *rhs = gtk_entry_get_text(rhs_e);
        fputs(rhs, fp);
        fputc('\n', fp);
    }

    // footer
    fputc('\n', fp);
    fprintf(fp, "# End\n");

    fclose(fp);
    g_free(W);
    g_free(filename);

    // the done dialog
    GtkWidget *done = gtk_message_dialog_new(
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
        "Simplex problem saved successfully.");
    gtk_dialog_run(GTK_DIALOG(done));
    gtk_widget_destroy(done);
}

// - - - - - END OF SAVING THE FILE - - - - -

// So that the panels created can't be moved around
void set_panel(GtkPaned *panel, GParamSpec *pspec, gpointer user_data) {
    // Position where division is set
    const int set_pos = 800;
    int current_pos = gtk_paned_get_position(panel);
    if (current_pos != set_pos) {
        gtk_paned_set_position(panel, set_pos);
    }
}

// - - - - - UPLOADING A FILE - - - - -

static gchar *str_trim_dup(const gchar *s) {
    if (!s) return g_strdup("");
    gchar *t = g_strdup(s);
    g_strstrip(t);
    return t;
}

static gboolean parse_int(const gchar *s, int *out) {
    if (!s) return FALSE;
    gchar *end = NULL; errno = 0;
    long v = strtol(s, &end, 10);
    if (errno || end == s) return FALSE;
    while (*end && g_ascii_isspace((guchar)*end)) end++;
    if (*end) return FALSE;
    if (out) *out = (int)v;
    return TRUE;
}

// any real number
static gboolean parse_real_str(const gchar *s, double *out) {
    return parse_real(s, out);
}

// positive real number
static gboolean parse_nonneg_real_str(const gchar *s, double *out) {
    return parse_nonneg_real(s, out);
}

static gchar **split_ws_trim(const gchar *line) {
    if (!line) return g_new0(gchar*,1);
    gchar *t = g_strdup(line);
    g_strstrip(t);
    gchar **v = g_regex_split_simple("\\s+", t, 0, 0);
    g_free(t);
    return v;
}

static gchar *dup_after_prefix(const gchar *line, const gchar *prefix) {
    if (!line || !g_str_has_prefix(line, prefix)) return NULL;
    return str_trim_dup(line + strlen(prefix));
}

// The loader
static void load_simplex_from_path(SimplexUI *ui, const char *filename) {
    // Resolve a parent window for dialogs
    GtkWindow *parent = NULL;
    if (ui && ui->name_problem) {
        GtkWidget *top = gtk_widget_get_toplevel(GTK_WIDGET(ui->name_problem));
        if (GTK_IS_WINDOW(top)) parent = GTK_WINDOW(top);
    }
    // --- Read file ---
    gchar *contents = NULL; gsize len = 0; GError *err = NULL;
    if (!g_file_get_contents(filename, &contents, &len, &err)) {
        GtkWidget *d = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Could not read file:\n%s\n\nError: %s",
            filename, err ? err->message : "unknown");
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        if (err) g_error_free(err);
        return;
    }

    gchar **lines_all = g_strsplit(contents, "\n", -1);
    GPtrArray *lines = g_ptr_array_new_with_free_func(g_free);
    for (guint i=0; lines_all[i]; ++i) {
        gchar *t = str_trim_dup(lines_all[i]);
        if (*t) g_ptr_array_add(lines, t); else g_free(t);
    }

    if (lines->len < 6 || !g_str_has_prefix(lines->pdata[0], "# SIMPLEX-UI")) {
        GtkWidget *d = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Unrecognized file format (missing '# SIMPLEX-UI ...').");
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        goto cleanup;
    }

    // Parse the header block
    gchar *name = NULL;
    gboolean is_max = TRUE;
    int n_vars = -1, n_restr = -1;
    gboolean inter_tables = FALSE;

    for (guint i=1; i<lines->len; ++i) {
        const char *L = lines->pdata[i];
        if (*L == '[') break;
        gchar *v = NULL;
        if ((v = dup_after_prefix(L, "Name: ")))              { g_free(name); name = v; continue; }
        if ((v = dup_after_prefix(L, "Sense: ")))             { is_max = (g_strcmp0(v,"MAX")==0); g_free(v); continue; }
        if ((v = dup_after_prefix(L, "Variables: ")))         { parse_int(v, &n_vars); g_free(v); continue; }
        if ((v = dup_after_prefix(L, "Constraints: ")))       { parse_int(v, &n_restr); g_free(v); continue; }
        if ((v = dup_after_prefix(L, "IntermediateTables: "))) { inter_tables = (atoi(v)!=0); g_free(v); continue; }
    }

    if (n_vars < 2 || n_vars > 15 || n_restr < 2 || n_restr > 15) {
        GtkWidget *d = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Invalid sizes in file (Variables=%d, Constraints=%d).", n_vars, n_restr);
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        goto cleanup;
    }

    // Locate sections
    gint idx_obj = -1, idx_con = -1;
    for (guint i=0; i<lines->len; ++i) {
        if (g_strcmp0(lines->pdata[i], "[Objective]") == 0)   idx_obj = (gint)i;
        if (g_strcmp0(lines->pdata[i], "[Constraints]") == 0) idx_con = (gint)i;
    }
    if (idx_obj < 0 || idx_con < 0 || idx_con <= idx_obj) {
        GtkWidget *d = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Missing [Objective] or [Constraints] section.");
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        goto cleanup;
    }

    // Parse the objective function
    gint obj_names_i  = idx_obj + 1;
    gint obj_coeffs_i = idx_obj + 2;
    if (obj_coeffs_i >= (gint)lines->len) {
        GtkWidget *d = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Incomplete [Objective] section.");
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        goto cleanup;
    }

    gchar *names_line  = dup_after_prefix(lines->pdata[obj_names_i], "names:");
    gchar *coeffs_line = dup_after_prefix(lines->pdata[obj_coeffs_i], "coeffs:");
    if (!names_line || !coeffs_line) {
        GtkWidget *d = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Malformed [Objective] lines (need 'names:' and 'coeffs:').");
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        g_free(names_line); g_free(coeffs_line);
        goto cleanup;
    }
    gchar **names_tok  = split_ws_trim(names_line);
    gchar **coeffs_tok = split_ws_trim(coeffs_line);
    g_free(names_line); g_free(coeffs_line);

    if ((int)g_strv_length(names_tok) != n_vars || (int)g_strv_length(coeffs_tok) != n_vars) {
        GtkWidget *d = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Objective lengths don't match Variables=%d.", n_vars);
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        g_strfreev(names_tok); g_strfreev(coeffs_tok);
        goto cleanup;
    }

    // Set the name, min/max button and the intermediate table button
    gtk_entry_set_text(GTK_ENTRY(ui->name_problem), name ? name : "");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->rb_max), is_max);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->rb_min), !is_max);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->cb_intermediate), inter_tables);

    // Set the spin buttons
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->spin_vars),  n_vars);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->spin_restr), n_restr);

    // Fill the objective function table
    for (int j=0; j<n_vars; ++j) {
        GtkEntry *name_e = GTK_ENTRY(g_ptr_array_index(ui->var_name_entries, j));
        GtkEntry *coef_e = GTK_ENTRY(g_ptr_array_index(ui->obj_coef_entries, j));
        gtk_entry_set_text(name_e, names_tok[j]);
        gtk_entry_set_text(coef_e, coeffs_tok[j]);
    }
    g_strfreev(names_tok); g_strfreev(coeffs_tok);

    // Parse the constraints
    GPtrArray *rows = g_ptr_array_new_with_free_func(g_free);
    for (guint i = idx_con + 1; i < lines->len; ++i) {
        const char *L = lines->pdata[i];
        if (*L == '#') break; // footer
        if (!*L) continue;
        if (g_str_has_prefix(L, "R")) {
            g_ptr_array_add(rows, g_strdup(L));
        }
    }
    if ((int)rows->len != n_restr) {
        GtkWidget *d = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Expected %d constraint rows, found %u.", n_restr, rows->len);
        gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
        g_ptr_array_free(rows, TRUE);
        goto cleanup;
    }

    // Each constraint row format
    for (int r=0; r<n_restr; ++r) {
        gchar *line = rows->pdata[r];

        // split around bars first
        gchar **parts = g_strsplit(line, "|", 3); // left, op, rhs
        if (g_strv_length(parts) < 3) {
            GtkWidget *d = gtk_message_dialog_new(
                parent,
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Malformed constraint row: %s", line);
            gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
            g_strfreev(parts);
            continue;
        }

        gchar *left  = str_trim_dup(parts[0]);
        gchar *opstr = str_trim_dup(parts[1]); // "≤" or "≥" or "="
        gchar *rhs   = str_trim_dup(parts[2]);

        gchar **left_tok = split_ws_trim(left);
        int Lc = (int)g_strv_length(left_tok);
        if (Lc < n_vars + 1) {
            GtkWidget *d = gtk_message_dialog_new(
                parent,
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Constraint row has too few coefficients: %s", line);
            gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
            g_strfreev(left_tok); g_free(left); g_free(opstr); g_free(rhs); g_strfreev(parts);
            continue;
        }

        for (int j=0; j<n_vars; ++j) {
            const gchar *cstr = left_tok[1 + j];
            double v;
            if (!parse_real_str(cstr, &v)) {
            }
            GtkEntry *e = GTK_ENTRY(g_ptr_array_index(ui->con_coef_entries, r*n_vars + j));
            gtk_entry_set_text(e, cstr);
        }

        // operator
        gint active = OP_LEQ;
        if (g_strcmp0(opstr, "≥") == 0 || g_strcmp0(opstr, ">=") == 0) active = OP_GEQ;
        else if (g_strcmp0(opstr, "=") == 0) active = OP_EQ;
        gtk_combo_box_set_active(GTK_COMBO_BOX(g_ptr_array_index(ui->con_op_combos, r)), active);

        // rhs (must be >= 0)
        double vr;
        if (!parse_nonneg_real_str(rhs, &vr)) {
        }
        gtk_entry_set_text(GTK_ENTRY(g_ptr_array_index(ui->con_rhs_entries, r)), rhs);

        g_strfreev(left_tok);
        g_free(left); g_free(opstr); g_free(rhs);
        g_strfreev(parts);
    }
    g_ptr_array_free(rows, TRUE);

    {
        GtkWidget *done = gtk_message_dialog_new(
            parent,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
            "File loaded successfully.");
        gtk_dialog_run(GTK_DIALOG(done));
        gtk_widget_destroy(done);
    }

cleanup:
    g_ptr_array_free(lines, TRUE);
    g_strfreev(lines_all);
    g_free(contents);
    g_free(name);
}

static void on_file_set(GtkFileChooserButton *btn, gpointer user_data) {
    SimplexUI *ui = (SimplexUI*)user_data;
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(btn));
    if (!filename) return;
    load_simplex_from_path(ui, filename);
    g_free(filename);
}

// - - - - - END OF UPLOADING A FILE - - - - -


// - - - - - CREATING MATRIX FOR CODE - - - - -
static double **alloc_dmatrix(int rows, int cols) {
    double **m = g_new0(double*, rows);
    double  *blk = g_new0(double, rows * cols);
    for (int i=0; i<rows; ++i) m[i] = blk + i*cols;
    return m; // free with free_dmatrix
}

static void free_dmatrix(double **m) {
    if (!m) return;
    g_free(m[0]);
    g_free(m);
}

static void force_focus_out_of_current_entry(GtkWidget *any_widget_in_window) {
    GtkWidget *top = gtk_widget_get_toplevel(any_widget_in_window);
    if (GTK_IS_WINDOW(top)) {
        GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(top));
        if (focus && GTK_IS_ENTRY(focus)) {
            // trigger "focus-out-event" so the validators run
            g_signal_emit_by_name(focus, "focus-out-event", NULL);
        }
    }
}

static void make_ascii_label(char prefix, int idx1_based, char out[32]) {
    g_snprintf(out, 32, "%c_%d", prefix, idx1_based);
}

static const char* op_to_str_ascii(int op) {
    switch (op) {
        case 0: return "<=";
        case 1: return ">=";
        case 2: return "=";
        default: return "?";
    }
}

static char* sanitize_name_ascii(const char *in) {
    if (!in) return g_strdup("");

    GString *g = g_string_new(NULL);
    const char *p = in;
    while (*p) {
        gunichar u = g_utf8_get_char(p);
        p = g_utf8_next_char(p);

        // Subscript digits U+2080..U+2089
        if (u >= 0x2080 && u <= 0x2089) {
            char d = '0' + (char)(u - 0x2080);
            g_string_append_c(g, d);
            continue;
        }

        // Allow common ASCII identifier chars; drop others
        if ((u >= 0x20 && u <= 0x7E)) { // printable ASCII
            // (optional) tighten to [A-Za-z0-9_]
            if (g_ascii_isalnum((int)u) || u=='_' ) {
                g_string_append_c(g, (char)u);
            } else {
                // keep dashes/spaces if you want; or convert to underscore:
                if (u=='-' || u==' ') g_string_append_c(g, '_');
                // else drop
            }
        }
        // else drop non-ASCII entirely
    }
    char *out = g_string_free(g, FALSE);
    return out;
}

static void free_names_array(char **names, int count) {
    if (!names) return;
    for (int i = 0; i < count; ++i) g_free(names[i]);
    g_free(names);
}

// Builds the Simplex matrix from the UI.
static gboolean build_simplex_payload(SimplexUI *ui,
                                      double ***matriz_out,
                                      char   **problemName_out,
                                      char  ***variableNames_out,
                                      int     *amountOfVariables_out,
                                      int     *saveMatrixes_out,
                                      int    **restrictions_out,
                                      int     *cols_out,
                                      int     *rows_out,
                                      int     *maximize_out) {
    g_return_val_if_fail(ui && matriz_out && problemName_out && variableNames_out &&
                         amountOfVariables_out && saveMatrixes_out && restrictions_out &&
                         cols_out && rows_out && maximize_out, FALSE);

    // sizes
    const int n_vars  = ui->n_vars;
    const int n_restr = ui->n_restr;
    const int rows    = 1 + n_restr;
    const int cols    = 1 /*Z*/ + n_vars + n_restr + 1 /*RHS*/;

    // column offsets
    const int z_col   = 0;
    const int var0    = 1;
    const int slack0  = 1 + n_vars;
    const int rhs_col = cols - 1;


    // parent window for any error dialogs
    GtkWindow *parent = NULL;
    if (ui->name_problem) {
        GtkWidget *top = gtk_widget_get_toplevel(GTK_WIDGET(ui->name_problem));
        if (GTK_IS_WINDOW(top)) parent = GTK_WINDOW(top);
    }

    // allocate outputs
    double **M = alloc_dmatrix(rows, cols);
    char   **varnames = g_new0(char*, n_vars);
    int    *restr_ops = g_new0(int, n_restr);

    // 1) problem name
    const gchar *pname = gtk_entry_get_text(GTK_ENTRY(ui->name_problem));
    *problemName_out = g_strdup(pname ? pname : "");

    // 2) variable names
    char **varnames_all = g_new0(char*, n_vars + n_restr);

    // first: decision variable names from the UI, sanitized
    for (int j = 0; j < n_vars; ++j) {
        const gchar *nm = gtk_entry_get_text(
            GTK_ENTRY(g_ptr_array_index(ui->var_name_entries, j)));
        varnames_all[j] = sanitize_name_ascii(nm ? nm : "");
    }

    // then: slack names
    for (int s = 0; s < n_restr; ++s) {
        char buf[32];
        make_ascii_label('S', s+1, buf);
        varnames_all[n_vars + s] = g_strdup(buf);
    }

    // 3) top controls
    *amountOfVariables_out = n_vars;
    *saveMatrixes_out = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ui->cb_intermediate)) ? 1 : 0;
    int is_max = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ui->rb_max)) ? 1 : 0;
    *maximize_out = is_max;

    // 4) OBJECTIVE row
    M[0][z_col] = 1.0;   // Z column
    for (int j = 0; j < n_vars; ++j) {
        GtkEntry *e = GTK_ENTRY(g_ptr_array_index(ui->obj_coef_entries, j));
        const gchar *t = gtk_entry_get_text(e);
        double v;
        if (!parse_real_str(t, &v)) {
            GtkWidget *d = gtk_message_dialog_new(parent,
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Invalid objective coefficient at column %d: “%s”.", j+1, t?t:"");
            gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
            free_dmatrix(M);
            free_names_array(varnames_all, n_vars + n_restr);
            g_free(restr_ops);
            g_free(*problemName_out); *problemName_out=NULL;
            return FALSE;
        }
        // Flip sign only for variables when maximizing
        M[0][var0 + j] = is_max ? -v : v;
    }
    // slack block in objective row = 0
    for (int s = 0; s < n_restr; ++s) M[0][slack0 + s] = 0.0;
    // RHS in objective row = 0
    M[0][rhs_col] = 0.0;

    // 5) CONSTRAINT rows
    for (int r = 0; r < n_restr; ++r) {
        // Z column = 0
        M[1 + r][z_col] = 0.0;

        // variable coefficients
        for (int j = 0; j < n_vars; ++j) {
            GtkEntry *e = GTK_ENTRY(g_ptr_array_index(ui->con_coef_entries, r*n_vars + j));
            const gchar *t = gtk_entry_get_text(e);
            double v;
            if (!parse_real_str(t, &v)) {
                GtkWidget *d = gtk_message_dialog_new(parent,
                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                    "Invalid constraint coefficient at R%d,C%d: “%s”.", r+1, j+1, t?t:"");
                gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
                free_dmatrix(M);
                free_names_array(varnames_all, n_vars + n_restr);
                g_free(restr_ops);
                g_free(*problemName_out); *problemName_out=NULL;
                return FALSE;
            }
            M[1 + r][var0 + j] = v;
        }

        // slack identity
        for (int s = 0; s < n_restr; ++s)
            M[1 + r][slack0 + s] = (s == r) ? 1.0 : 0.0;

        // operator (store as provided)
        GtkComboBox *cmb = GTK_COMBO_BOX(g_ptr_array_index(ui->con_op_combos, r));
        restr_ops[r] = gtk_combo_box_get_active(cmb);

        // RHS (>= 0)
        GtkEntry *rhs_e = GTK_ENTRY(g_ptr_array_index(ui->con_rhs_entries, r));
        const gchar *rt = gtk_entry_get_text(rhs_e);
        double b;
        if (!parse_nonneg_real_str(rt, &b)) {
            GtkWidget *d = gtk_message_dialog_new(parent,
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                "Invalid RHS at row R%d: “%s”. Must be ≥ 0.", r+1, rt?rt:"");
            gtk_dialog_run(GTK_DIALOG(d)); gtk_widget_destroy(d);
            free_dmatrix(M);
            free_names_array(varnames_all, n_vars + n_restr);
            g_free(restr_ops);
            g_free(*problemName_out); *problemName_out=NULL;
            return FALSE;
        }
        M[1 + r][rhs_col] = b;
    }

    // return outs
    *matriz_out        = M;
    *variableNames_out = varnames_all;
    *amountOfVariables_out = n_vars;
    *restrictions_out  = restr_ops;
    *cols_out          = cols;
    *rows_out          = rows;

    return TRUE;
}

static void print_simplex_matrix(double **A, int rows, int cols, const char **vnames) {
    int m = rows - 1;               // constraints
    int n = cols - 1 /*Z*/ - m - 1; // original decision vars (for layout math)

    printf("\n===== SIMPLEX MATRIX (%d x %d) =====\n", rows, cols);

    // Header row: Z, then *all* variable columns (n + m), then RHS
    printf("%10s", "Z");

    // total named variable columns = n + m
    int total_named = n + m;
    for (int j = 0; j < total_named; ++j) {
        const char *name = (vnames && vnames[j]) ? vnames[j] : "x";
        printf(" %8s", name);
    }

    printf(" %8s\n", "RHS");

    // Rows data
    for (int i = 0; i < rows; ++i) {
        printf("Row %2d |", i);
        for (int j = 0; j < cols; ++j)
            printf(" %8.3f", A[i][j]);
        printf("\n");
    }
    printf("====================================\n");
}

static void print_simplex_payload(double **A, const char *pname, char **vnames,
                                  int nvars, int saveInter, int *ops,
                                  int cols, int rows, int maximize) {
    // Names
    printf("\n--- Payload ---\n");
    printf("Problem Name : %s\n", pname ? pname : "");
    printf("Sense        : %s\n", maximize ? "MAX" : "MIN");
    printf("Variables    : %d\n", nvars);
    printf("Constraints  : %d\n", rows - 1);
    printf("Save Interm. : %d\n", saveInter);
    printf("cols x rows  : %d x %d\n", cols, rows);

    printf("Var Names    : ");
    int total_names = nvars + (rows - 1);
    for (int j = 0; j < total_names; ++j) {
        printf("%s%s", vnames[j], (j == total_names - 1) ? "" : ", ");
    }
    printf("\n");

    // Operators
    printf("Ops per row  : ");
    for (int r = 0; r < rows-1; ++r) {
        printf("%s%s", op_to_str_ascii(ops[r]), (r==rows-2) ? "" : ", ");
    }
    printf("\n");

    // Numeric matrix
    print_simplex_matrix(A, rows, cols, (const char**)vnames);
    printf("--- End Payload ---\n\n");
}

static void on_latex_file_clicked(GtkButton *btn, gpointer user_data) {
    SimplexUI *ui = (SimplexUI*)user_data;

    force_focus_out_of_current_entry(GTK_WIDGET(btn));

    double **A = NULL;
    char *pname = NULL;
    char **vnames = NULL;
    int nvars = 0, saveInter = 0, *ops = NULL, cols = 0, rows = 0, maximize = 0;

    if (!build_simplex_payload(ui, &A, &pname, &vnames,
                               &nvars, &saveInter, &ops, &cols, &rows, &maximize)) {
        return;
    }

    // Print everything being sent
    print_simplex_payload(A, pname, vnames, nvars, saveInter, ops, cols, rows, maximize);

    // Call Simplex routine
    runSimplex(A, pname, vnames, nvars, saveInter, ops, cols, rows, maximize);

    // Cleanup
    free_dmatrix(A);
    for (int j = 0; j < nvars + rows - 1; ++j) g_free(vnames[j]);
    g_free(vnames);
    g_free(ops);
    g_free(pname);
}

// - - - - - END OF CREATING MATRIX FOR CODE - - - - -

//Main
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // CSS
    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_path(prov, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(), GTK_STYLE_PROVIDER(prov),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(prov);

    GtkBuilder *builder = gtk_builder_new_from_file("programa-4.glade");
    GtkWidget  *win     = GTK_WIDGET(gtk_builder_get_object(builder, "program-4"));
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    SimplexUI *ui = g_new0(SimplexUI, 1);
    ui->builder         = builder;
    ui->name_problem    = GTK_WIDGET(gtk_builder_get_object(builder, "name-problem"));
    ui->spin_vars       = GTK_WIDGET(gtk_builder_get_object(builder, "amount-variables"));
    ui->spin_restr      = GTK_WIDGET(gtk_builder_get_object(builder, "amount-restrictions"));
    ui->rb_max          = GTK_WIDGET(gtk_builder_get_object(builder, "maximize"));
    ui->rb_min          = GTK_WIDGET(gtk_builder_get_object(builder, "minimize"));
    ui->cb_intermediate = GTK_WIDGET(gtk_builder_get_object(builder, "intermediate-tables"));
    ui->scroll          = GTK_WIDGET(gtk_builder_get_object(builder, "scroll"));
    ui->vbox            = NULL;

    g_signal_connect(ui->spin_vars,  "value-changed", G_CALLBACK(on_spin_changed), ui);
    g_signal_connect(ui->spin_restr, "value-changed", G_CALLBACK(on_spin_changed), ui);

    // Division panel
    GtkWidget *panel = GTK_WIDGET(gtk_builder_get_object(builder, "panel"));
    g_signal_connect(panel, "notify::position", G_CALLBACK(set_panel), NULL);

    // Save file button
    GtkWidget *btn_save = GTK_WIDGET(gtk_builder_get_object(builder, "file-saved"));
    g_signal_connect(btn_save, "clicked", G_CALLBACK(save_simplex_file), ui);

    // Load file button
    GtkWidget *boton_load = GTK_WIDGET(gtk_builder_get_object(builder, "file-chosen"));
    // Makes it an "Open" chooser and filters to only show .txt
    gtk_file_chooser_set_action(GTK_FILE_CHOOSER(boton_load), GTK_FILE_CHOOSER_ACTION_OPEN);
    GtkFileFilter *flt = gtk_file_filter_new();
    gtk_file_filter_set_name(flt, "Text files");
    gtk_file_filter_add_pattern(flt, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(boton_load), flt);
    // When a file is picked
    g_signal_connect(boton_load, "file-set", G_CALLBACK(on_file_set), ui);

    // Create LATEX file
    GtkWidget *btn_latex = GTK_WIDGET(gtk_builder_get_object(builder, "latex-file"));
    g_signal_connect(btn_latex, "clicked", G_CALLBACK(on_latex_file_clicked), ui);

    // Termination button
    GtkWidget *boton_salida = GTK_WIDGET(gtk_builder_get_object(builder, "exit"));
    g_signal_connect(boton_salida, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    rebuild_all(ui);

    gtk_window_maximize(GTK_WINDOW(win));
    gtk_widget_show_all(win);
    gtk_main();

    // Cleanup (your existing frees)
    if (ui->obj_coef_entries) g_ptr_array_free(ui->obj_coef_entries, TRUE);
    if (ui->var_name_entries) g_ptr_array_free(ui->var_name_entries, TRUE);
    if (ui->con_coef_entries) g_ptr_array_free(ui->con_coef_entries, TRUE);
    if (ui->con_op_combos)    g_ptr_array_free(ui->con_op_combos, TRUE);
    if (ui->con_rhs_entries)  g_ptr_array_free(ui->con_rhs_entries, TRUE);
    if (ui->con_header_labels) g_ptr_array_free(ui->con_header_labels, TRUE);
    g_object_unref(builder);
    g_free(ui);

    return 0;
}