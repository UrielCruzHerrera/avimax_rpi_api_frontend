#include "menu_view.h"
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

static void on_go_to(GtkButton *button, gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);
    const char *page = (const char *)g_object_get_data(G_OBJECT(button), "page-name");
    gtk_stack_set_visible_child_name(stack, page);
}

static GtkWidget *load_icon_or_placeholder(const char *path, const char *fallback, int width, int height) {
    if (g_file_test(path, G_FILE_TEST_EXISTS)) {
        GError *error = NULL;
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(path, width, height, TRUE, &error);

        if (pixbuf) {
            GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
            gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(image, GTK_ALIGN_CENTER);

            gtk_widget_set_margin_top(image, 8);
            gtk_widget_set_margin_bottom(image, 8);
            gtk_widget_set_margin_start(image, 8);
            gtk_widget_set_margin_end(image, 8);

            g_object_unref(pixbuf);
            return image;
        }

        if (error) {
            g_error_free(error);
        }
    }

    GtkWidget *label = gtk_label_new(fallback);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(label, 8);
    gtk_widget_set_margin_bottom(label, 8);
    gtk_widget_set_margin_start(label, 8);
    gtk_widget_set_margin_end(label, 8);
    return label;
}

static GtkWidget *create_menu_card(const char *title,
                                   const char *icon_path,
                                   const char *fallback,
                                   const char *page_name,
                                   GtkStack *stack) {
    GtkWidget *button = gtk_button_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *icon = load_icon_or_placeholder(icon_path, fallback, 64, 64);
    GtkWidget *label = gtk_label_new(title);

    gtk_style_context_add_class(gtk_widget_get_style_context(button), "menu-card");
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "menu-card-title");

    gtk_widget_set_hexpand(button, TRUE);
    gtk_widget_set_vexpand(button, TRUE);

    gtk_widget_set_margin_top(button, 6);
    gtk_widget_set_margin_bottom(button, 6);
    gtk_widget_set_margin_start(button, 6);
    gtk_widget_set_margin_end(button, 6);

    gtk_widget_set_margin_top(box, 14);
    gtk_widget_set_margin_bottom(box, 14);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);

    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);

    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(button), box);

    g_object_set_data_full(G_OBJECT(button), "page-name", g_strdup(page_name), g_free);
    g_signal_connect(button, "clicked", G_CALLBACK(on_go_to), stack);

    return button;
}

MenuView *menu_view_new(GtkStack *stack) {
    MenuView *view = g_malloc(sizeof(MenuView));

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    GtkWidget *title = gtk_label_new("Menu principal");
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *btn_inicio = gtk_button_new_with_label("← Inicio");

    gtk_style_context_add_class(gtk_widget_get_style_context(title), "menu-title");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_inicio), "nav-btn");

    gtk_widget_set_margin_top(root, 12);
    gtk_widget_set_margin_bottom(root, 12);
    gtk_widget_set_margin_start(root, 12);
    gtk_widget_set_margin_end(root, 12);

    gtk_widget_set_halign(title, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(title, 8);

    gtk_grid_set_row_spacing(GTK_GRID(grid), 14);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 14);

    GtkWidget *card_vent = create_menu_card(
        "Ventilacion",
        "assets/icons/ventilacion.png",
        "[vent]",
        "ventilacion",
        stack
    );

    GtkWidget *card_cria = create_menu_card(
        "Criadoras",
        "assets/icons/criadoras.png",
        "[cria]",
        "criadoras",
        stack
    );

    GtkWidget *card_bomb = create_menu_card(
        "Bombas",
        "assets/icons/bombas.png",
        "[bomb]",
        "bombas",
        stack
    );

    GtkWidget *card_parv = create_menu_card(
        "Gestion de parvada",
        "assets/icons/parvada.png",
        "[parvada]",
        "dashboard",
        stack
    );

    gtk_grid_attach(GTK_GRID(grid), card_vent, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), card_cria, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), card_bomb, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), card_parv, 1, 1, 1, 1);

    g_object_set_data_full(G_OBJECT(btn_inicio), "page-name", g_strdup("dashboard"), g_free);
    g_signal_connect(btn_inicio, "clicked", G_CALLBACK(on_go_to), stack);

    gtk_box_pack_start(GTK_BOX(root), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), grid, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(root), btn_inicio, FALSE, FALSE, 0);

    view->root = root;
    return view;
}

GtkWidget *menu_view_get_widget(MenuView *view) {
    return view->root;
}