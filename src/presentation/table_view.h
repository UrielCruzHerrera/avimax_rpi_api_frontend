#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <gtk/gtk.h>
#include "../shared/app_state.h"

typedef enum {
    TABLE_VENTILACION,
    TABLE_CRIADORAS,
    TABLE_BOMBAS
} TableType;

typedef struct {
    GtkWidget *root;
    GtkWidget *rows_grid;
    GtkStack *stack;
    AppState *state;
    TableType type;
    int show_time;
    int max_rows;
} TableView;

TableView *table_view_new(AppState *state, GtkStack *stack, TableType type, const char *title, int show_time, int max_rows);
GtkWidget *table_view_get_widget(TableView *view);

#endif
