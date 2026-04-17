#include "table_view.h"
#include <stdio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

static void on_go_to(GtkButton *button, gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);
    const char *page = (const char *)g_object_get_data(G_OBJECT(button), "page-name");
    gtk_stack_set_visible_child_name(stack, page);
}

static GtkWidget *create_nav_button(const char *text, const char *page_name, GtkStack *stack) {
    GtkWidget *button = gtk_button_new_with_label(text);
    gtk_style_context_add_class(gtk_widget_get_style_context(button), "nav-btn");
    g_object_set_data_full(G_OBJECT(button), "page-name", g_strdup(page_name), g_free);
    g_signal_connect(button, "clicked", G_CALLBACK(on_go_to), stack);
    return button;
}

static GtkWidget *load_icon_or_placeholder(const char *path, const char *fallback, int width, int height) {
    if (g_file_test(path, G_FILE_TEST_EXISTS)) {
        GError *error = NULL;
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(path, width, height, TRUE, &error);

        if (pixbuf) {
            GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
            gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
            g_object_unref(pixbuf);
            return image;
        }

        if (error) {
            g_error_free(error);
        }
    }

    return gtk_label_new(fallback);
}

static GtkWidget *create_cell_label(const char *text, const char *css_class) {
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(label), css_class);
    return label;
}

static GtkWidget *create_pill_label(const char *text, const char *css_class) {
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *label = gtk_label_new(text);

    gtk_widget_set_margin_top(frame, 2);
    gtk_widget_set_margin_bottom(frame, 2);
    gtk_widget_set_margin_start(frame, 4);
    gtk_widget_set_margin_end(frame, 4);

    gtk_container_add(GTK_CONTAINER(frame), label);
    gtk_style_context_add_class(gtk_widget_get_style_context(frame), css_class);
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "table-pill-text");
    return frame;
}

static void fill_row(TableView *view, int row_index, DeviceRow row) {
    char text[64];

    snprintf(text, sizeof(text), "%d", row.id);
    GtkWidget *col_id = create_cell_label(text, "table-cell");

    snprintf(text, sizeof(text), "%.1f", row.entrada);
    GtkWidget *col_entrada = create_pill_label(text, "pill-green");

    snprintf(text, sizeof(text), "%.1f", row.salida);
    GtkWidget *col_salida = create_pill_label(text, "pill-red");

    gtk_grid_attach(GTK_GRID(view->rows_grid), col_id, 0, row_index, 1, 1);
    gtk_grid_attach(GTK_GRID(view->rows_grid), col_entrada, 1, row_index, 1, 1);
    gtk_grid_attach(GTK_GRID(view->rows_grid), col_salida, 2, row_index, 1, 1);

    if (view->show_time) {
        int minutes = row.tiempo_segundos / 60;
        int seconds = row.tiempo_segundos % 60;
        snprintf(text, sizeof(text), "%d:%02d s", minutes, seconds);
        GtkWidget *col_tiempo = create_pill_label(text, "pill-blue");
        gtk_grid_attach(GTK_GRID(view->rows_grid), col_tiempo, 3, row_index, 1, 1);
    }
}

static void rebuild_rows(TableView *view) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(view->rows_grid));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    GtkWidget *h1 = create_cell_label(
        view->type == TABLE_VENTILACION ? "Ventilador" :
        (view->type == TABLE_CRIADORAS ? "Criadora" : "Bomba"),
        "table-header"
    );
    GtkWidget *h2 = create_cell_label("Entrada", "table-header");
    GtkWidget *h3 = create_cell_label("Salida", "table-header");

    gtk_grid_attach(GTK_GRID(view->rows_grid), h1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(view->rows_grid), h2, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(view->rows_grid), h3, 2, 0, 1, 1);

    if (view->show_time) {
        GtkWidget *h4 = create_cell_label("Tiempo", "table-header");
        gtk_grid_attach(GTK_GRID(view->rows_grid), h4, 3, 0, 1, 1);
    }

    DeviceTable table;
    if (view->type == TABLE_VENTILACION) {
        table = app_state_get_ventilacion(view->state);
    } else if (view->type == TABLE_CRIADORAS) {
        table = app_state_get_criadoras(view->state);
    } else {
        table = app_state_get_bombas(view->state);
    }

    int limit = table.count < view->max_rows ? table.count : view->max_rows;
    for (int i = 0; i < limit; i++) {
        fill_row(view, i + 1, table.rows[i]);
    }

    gtk_widget_show_all(view->rows_grid);
}

static gboolean refresh_table(gpointer user_data) {
    TableView *view = (TableView *)user_data;
    rebuild_rows(view);
    return G_SOURCE_CONTINUE;
}

TableView *table_view_new(AppState *state, GtkStack *stack, TableType type, const char *title, int show_time, int max_rows) {
    TableView *view = g_malloc(sizeof(TableView));
    view->state = state;
    view->stack = stack;
    view->type = type;
    view->show_time = show_time;
    view->max_rows = max_rows;

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *brand = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *logo = load_icon_or_placeholder("assets/icons/logo.png", "AviMax", 40, 40);
    GtkWidget *title_label = gtk_label_new(title);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *btn_edit = gtk_button_new_with_label("Editar");
    GtkWidget *btn_home = create_nav_button("⌂ Inicio", "dashboard", stack);
    GtkWidget *btn_back = create_nav_button("← Atras", "menu", stack);
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *table_frame = gtk_frame_new(NULL);
    GtkWidget *rows_grid = gtk_grid_new();

    gtk_style_context_add_class(gtk_widget_get_style_context(root), "screen-root");
    gtk_style_context_add_class(gtk_widget_get_style_context(title_label), "screen-title");
    gtk_style_context_add_class(gtk_widget_get_style_context(table_frame), "table-frame");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_edit), "primary-btn");

    gtk_widget_set_margin_top(root, 4);
    gtk_widget_set_margin_bottom(root, 4);
    gtk_widget_set_margin_start(root, 4);
    gtk_widget_set_margin_end(root, 4);

    gtk_grid_set_row_spacing(GTK_GRID(rows_grid), 4);
    gtk_grid_set_column_spacing(GTK_GRID(rows_grid), 8);

    gtk_container_add(GTK_CONTAINER(table_frame), rows_grid);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_container_add(GTK_CONTAINER(scroll), table_frame);

    gtk_box_pack_start(GTK_BOX(brand), logo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(brand), title_label, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(header), brand, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(actions), btn_edit, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(actions), btn_home, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(actions), btn_back, FALSE, FALSE, 0);
    gtk_widget_set_halign(actions, GTK_ALIGN_CENTER);

    gtk_box_pack_start(GTK_BOX(root), header, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), actions, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), scroll, TRUE, TRUE, 0);

    view->root = root;
    view->rows_grid = rows_grid;

    rebuild_rows(view);
    g_timeout_add(800, refresh_table, view);
    return view;
}

GtkWidget *table_view_get_widget(TableView *view) {
    return view->root;
}