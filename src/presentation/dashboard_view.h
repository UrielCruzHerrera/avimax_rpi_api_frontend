#ifndef DASHBOARD_VIEW_H
#define DASHBOARD_VIEW_H

#include <gtk/gtk.h>
#include "../shared/app_state.h"

typedef struct {
    GtkWidget *root;
    GtkWidget *lbl_temp;
    GtkWidget *lbl_hum;
    GtkWidget *lbl_nh3;
    GtkWidget *lbl_dia;
    GtkWidget *lbl_status;
    GtkStack *stack;
    AppState *state;
} DashboardView;

DashboardView *dashboard_view_new(AppState *state, GtkStack *stack);
GtkWidget *dashboard_view_get_widget(DashboardView *view);

#endif
