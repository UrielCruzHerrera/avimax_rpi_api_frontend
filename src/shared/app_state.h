#ifndef APP_STATE_H
#define APP_STATE_H

#include <glib.h>
#include "../domain/models/app_models.h"

typedef struct {
    DashboardData dashboard;
    DeviceTable ventilacion;
    DeviceTable criadoras;
    DeviceTable bombas;
    char api_status[128];
    GMutex mutex;
    int running;
} AppState;

void app_state_init(AppState *state);
void app_state_destroy(AppState *state);
void app_state_set_dashboard(AppState *state, const DashboardData *dashboard);
void app_state_set_ventilacion(AppState *state, const DeviceTable *table);
void app_state_set_criadoras(AppState *state, const DeviceTable *table);
void app_state_set_bombas(AppState *state, const DeviceTable *table);
DashboardData app_state_get_dashboard(AppState *state);
DeviceTable app_state_get_ventilacion(AppState *state);
DeviceTable app_state_get_criadoras(AppState *state);
DeviceTable app_state_get_bombas(AppState *state);
void app_state_set_api_status(AppState *state, const char *status);
void app_state_get_api_status(AppState *state, char *buffer, gsize size);
void app_state_stop(AppState *state);
int app_state_is_running(AppState *state);

#endif
