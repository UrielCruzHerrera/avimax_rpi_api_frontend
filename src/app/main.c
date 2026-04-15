#include <gtk/gtk.h>
#include <stdlib.h>
#include "../shared/app_state.h"
#include "../infrastructure/api/api_service.h"
#include "../presentation/main_window.h"

static void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "assets/style.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    load_css();

    const char *base_url = getenv("AVIMAX_API_URL");
    if (!base_url) {
        base_url = "http://127.0.0.1:8080";
    }

    AppState state;
    app_state_init(&state);

    ApiService api_service;
    api_service_init(&api_service, &state, base_url, 2000);
    api_service_start(&api_service);

    GtkWidget *window = main_window_create(&state);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    app_state_stop(&state);
    api_service_stop(&api_service);
    app_state_destroy(&state);
    return 0;
}
