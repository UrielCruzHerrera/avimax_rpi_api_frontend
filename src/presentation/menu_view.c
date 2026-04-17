#include "menu_view.h"
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
            gtk_widget_set_margin_top(image, 6);
            gtk_widget_set_margin_bottom(image, 6);
            g_object_unref(pixbuf);
            return image;
        }

        if (error) {
            g_error_free(error);
        }
    }

    GtkWidget *label = gtk_label_new(fallback);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    return label;
}

static GtkWidget *create_menu_card(const char *title,
                                   const char *icon_path,
                                   const char *fallback,
                                   const char *page_name,
                                   const char *css_class,
                                   GtkStack *stack) {
    GtkWidget *button = gtk_button_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *title_label = gtk_label_new(title);
    GtkWidget *icon = load_icon_or_placeholder(icon_path, fallback, 46, 46);

    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(icon, GTK_ALIGN_CENTER);

    gtk_style_context_add_class(gtk_widget_get_style_context(button), "menu-card");
    gtk_style_context_add_class(gtk_widget_get_style_context(button), css_class);
    gtk_style_context_add_class(gtk_widget_get_style_context(title_label), "menu-card-title");

    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);

    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(button), box);

    g_object_set_data_full(G_OBJECT(button), "page-name", g_strdup(page_name), g_free);
    g_signal_connect(button, "clicked", G_CALLBACK(on_go_to), stack);

    return button;
}

MenuView *menu_view_new(GtkStack *stack) {
    MenuView *view = g_malloc(sizeof(MenuView));

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *brand = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *brand_logo = load_icon_or_placeholder("assets/icons/logo.png", "AviMax", 42, 42);
    GtkWidget *brand_name = gtk_label_new("Panel de control");
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *btn_home = gtk_button_new_with_label("⌂ Inicio");

    gtk_style_context_add_class(gtk_widget_get_style_context(root), "screen-root");
    gtk_style_context_add_class(gtk_widget_get_style_context(brand_name), "screen-title");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_home), "nav-btn");

    g_object_set_data_full(G_OBJECT(btn_home), "page-name", g_strdup("dashboard"), g_free);
    g_signal_connect(btn_home, "clicked", G_CALLBACK(on_go_to), stack);

    gtk_widget_set_margin_top(root, 4);
    gtk_widget_set_margin_bottom(root, 4);
    gtk_widget_set_margin_start(root, 4);
    gtk_widget_set_margin_end(root, 4);

    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    GtkWidget *card_vent = create_menu_card("Ventilacion", "assets/icons/ventilacion.png", "[vent]", "ventilacion", "menu-vent", stack);
    GtkWidget *card_cria = create_menu_card("Criadoras", "assets/icons/criadoras.png", "[cria]", "criadoras", "menu-cria", stack);
    GtkWidget *card_bomb = create_menu_card("Bombas", "assets/icons/bombas.png", "[bomb]", "bombas", "menu-bomb", stack);
    GtkWidget *card_parv = create_menu_card("Gestion de parvada", "assets/icons/parvada.png", "[parvada]", "dashboard", "menu-parvada", stack);

    gtk_grid_attach(GTK_GRID(grid), card_vent, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), card_cria, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), card_bomb, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), card_parv, 1, 1, 1, 1);

    gtk_box_pack_start(GTK_BOX(brand), brand_logo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(brand), brand_name, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(header), brand, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(header), btn_home, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(root), header, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), scroll, TRUE, TRUE, 0);

    view->root = root;
    return view;
}

GtkWidget *menu_view_get_widget(MenuView *view) {
    return view->root;
}