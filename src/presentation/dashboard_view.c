#include "dashboard_view.h"
#include <stdio.h>
#include <glib.h>
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
            gtk_widget_set_valign(image, GTK_ALIGN_CENTER);

            gtk_widget_set_margin_top(image, 6);
            gtk_widget_set_margin_bottom(image, 6);
            gtk_widget_set_margin_start(image, 6);
            gtk_widget_set_margin_end(image, 6);

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
    gtk_widget_set_margin_top(label, 6);
    gtk_widget_set_margin_bottom(label, 6);
    gtk_widget_set_margin_start(label, 6);
    gtk_widget_set_margin_end(label, 6);
    return label;
}

static GtkWidget *create_metric_card(const char *title,
                                     const char *icon_path,
                                     const char *fallback,
                                     GtkWidget **value_label) {
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *title_label = gtk_label_new(title);
    GtkWidget *icon = load_icon_or_placeholder(icon_path, fallback, 42, 42);
    GtkWidget *value = gtk_label_new("--");

    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(value, GTK_ALIGN_CENTER);

    gtk_style_context_add_class(gtk_widget_get_style_context(frame), "metric-card");
    gtk_style_context_add_class(gtk_widget_get_style_context(title_label), "metric-title");
    gtk_style_context_add_class(gtk_widget_get_style_context(value), "metric-value");

    gtk_widget_set_margin_top(frame, 4);
    gtk_widget_set_margin_bottom(frame, 4);
    gtk_widget_set_margin_start(frame, 4);
    gtk_widget_set_margin_end(frame, 4);

    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 16);
    gtk_widget_set_margin_end(box, 16);

    gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), value, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(frame), box);

    *value_label = value;
    return frame;
}

static gboolean refresh_dashboard(gpointer user_data) {
    DashboardView *view = (DashboardView *)user_data;
    DashboardData data = app_state_get_dashboard(view->state);

    char status[128];
    char buffer[128];

    snprintf(buffer, sizeof(buffer), "Dia %d", data.dia_lote);
    gtk_label_set_text(GTK_LABEL(view->lbl_dia), buffer);

    snprintf(buffer, sizeof(buffer), "%.1f °C", data.temperatura);
    gtk_label_set_text(GTK_LABEL(view->lbl_temp), buffer);

    snprintf(buffer, sizeof(buffer), "%.1f %%HR", data.humedad);
    gtk_label_set_text(GTK_LABEL(view->lbl_hum), buffer);

    snprintf(buffer, sizeof(buffer), "%.1f ppm", data.amoniaco);
    gtk_label_set_text(GTK_LABEL(view->lbl_nh3), buffer);

    app_state_get_api_status(view->state, status, sizeof(status));
    gtk_label_set_text(GTK_LABEL(view->lbl_status), status);

    return G_SOURCE_CONTINUE;
}

DashboardView *dashboard_view_new(AppState *state, GtkStack *stack) {
    DashboardView *view = g_malloc(sizeof(DashboardView));
    view->state = state;
    view->stack = stack;

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    GtkWidget *logo = load_icon_or_placeholder("assets/icons/logo.png", "AviMax", 90, 90);
    GtkWidget *cards = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 24);
    GtkWidget *spacer = gtk_label_new("");
    GtkWidget *btn_menu = create_nav_button("☰ Menu", "menu", stack);

    view->lbl_dia = gtk_label_new("Dia --");
    view->lbl_status = gtk_label_new("Esperando API...");

    gtk_widget_set_margin_top(root, 12);
    gtk_widget_set_margin_bottom(root, 12);
    gtk_widget_set_margin_start(root, 12);
    gtk_widget_set_margin_end(root, 12);

    gtk_widget_set_halign(logo, GTK_ALIGN_START);
    gtk_widget_set_hexpand(view->lbl_dia, TRUE);
    gtk_widget_set_halign(view->lbl_dia, GTK_ALIGN_END);

    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_widget_set_vexpand(spacer, TRUE);

    gtk_widget_set_halign(btn_menu, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(btn_menu, 10);
    gtk_widget_set_margin_bottom(btn_menu, 10);

    gtk_widget_set_halign(view->lbl_status, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(view->lbl_status, 6);

    gtk_style_context_add_class(gtk_widget_get_style_context(view->lbl_dia), "top-info");
    gtk_style_context_add_class(gtk_widget_get_style_context(view->lbl_status), "api-status");

    gtk_box_pack_start(GTK_BOX(header), logo, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(header), view->lbl_dia, FALSE, FALSE, 0);

    GtkWidget *card_temp = create_metric_card(
        "Temperatura",
        "assets/icons/temperatura.png",
        "[temp]",
        &view->lbl_temp
    );

    GtkWidget *card_nh3 = create_metric_card(
        "Amoniaco",
        "assets/icons/amoniaco.png",
        "[nh3]",
        &view->lbl_nh3
    );

    GtkWidget *card_hum = create_metric_card(
        "Humedad",
        "assets/icons/humedad.png",
        "[hum]",
        &view->lbl_hum
    );

    gtk_box_pack_start(GTK_BOX(cards), card_temp, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(cards), card_nh3, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(cards), card_hum, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(root), header, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), cards, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(root), spacer, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(root), btn_menu, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(root), view->lbl_status, FALSE, FALSE, 0);

    view->root = root;

    g_timeout_add(500, refresh_dashboard, view);
    return view;
}

GtkWidget *dashboard_view_get_widget(DashboardView *view) {
    return view->root;
}