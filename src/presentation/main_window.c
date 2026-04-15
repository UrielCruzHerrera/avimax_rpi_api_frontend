#include "main_window.h"
#include "dashboard_view.h"
#include "menu_view.h"
#include "table_view.h"

GtkWidget *main_window_create(AppState *state) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "AviMax");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 24);
    gtk_style_context_add_class(gtk_widget_get_style_context(window), "app-window");

    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 220);

    DashboardView *dashboard = dashboard_view_new(state, GTK_STACK(stack));
    MenuView *menu = menu_view_new(GTK_STACK(stack));
    TableView *vent = table_view_new(state, GTK_STACK(stack), TABLE_VENTILACION, "Ventilacion", 0, 7);
    TableView *cria = table_view_new(state, GTK_STACK(stack), TABLE_CRIADORAS, "Criadoras", 1, 5);
    TableView *bomb = table_view_new(state, GTK_STACK(stack), TABLE_BOMBAS, "Bombas", 1, 2);

    gtk_stack_add_named(GTK_STACK(stack), dashboard_view_get_widget(dashboard), "dashboard");
    gtk_stack_add_named(GTK_STACK(stack), menu_view_get_widget(menu), "menu");
    gtk_stack_add_named(GTK_STACK(stack), table_view_get_widget(vent), "ventilacion");
    gtk_stack_add_named(GTK_STACK(stack), table_view_get_widget(cria), "criadoras");
    gtk_stack_add_named(GTK_STACK(stack), table_view_get_widget(bomb), "bombas");
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "dashboard");

    gtk_container_add(GTK_CONTAINER(window), stack);
    return window;
}
