#ifndef MENU_VIEW_H
#define MENU_VIEW_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *root;
} MenuView;

MenuView *menu_view_new(GtkStack *stack);
GtkWidget *menu_view_get_widget(MenuView *view);

#endif
