#include "app_state.h"
#include <string.h>

static DeviceTable seed_table(int count, float entrada, float salida, int tiempo_base) {
    DeviceTable table;
    table.count = count;
    for (int i = 0; i < count; i++) {
        table.rows[i].id = i + 1;
        table.rows[i].entrada = entrada;
        table.rows[i].salida = salida;
        table.rows[i].tiempo_segundos = tiempo_base;
        table.rows[i].activo = 0;
    }
    return table;
}

void app_state_init(AppState *state) {
    state->dashboard.temperatura = 28.5f;
    state->dashboard.humedad = 30.0f;
    state->dashboard.amoniaco = 15.0f;
    state->dashboard.dia_lote = 45;
    state->ventilacion = seed_table(7, 25.8f, 25.8f, 0);
    state->criadoras = seed_table(5, 25.8f, 25.8f, 0);
    state->bombas = seed_table(2, 25.8f, 25.8f, 0);
    strncpy(state->api_status, "Esperando API...", sizeof(state->api_status) - 1);
    state->api_status[sizeof(state->api_status) - 1] = '\0';
    state->running = 1;
    g_mutex_init(&state->mutex);
}

void app_state_destroy(AppState *state) {
    g_mutex_clear(&state->mutex);
}

void app_state_set_dashboard(AppState *state, const DashboardData *dashboard) {
    g_mutex_lock(&state->mutex);
    state->dashboard = *dashboard;
    g_mutex_unlock(&state->mutex);
}

void app_state_set_ventilacion(AppState *state, const DeviceTable *table) {
    g_mutex_lock(&state->mutex);
    state->ventilacion = *table;
    g_mutex_unlock(&state->mutex);
}

void app_state_set_criadoras(AppState *state, const DeviceTable *table) {
    g_mutex_lock(&state->mutex);
    state->criadoras = *table;
    g_mutex_unlock(&state->mutex);
}

void app_state_set_bombas(AppState *state, const DeviceTable *table) {
    g_mutex_lock(&state->mutex);
    state->bombas = *table;
    g_mutex_unlock(&state->mutex);
}

DashboardData app_state_get_dashboard(AppState *state) {
    DashboardData copy;
    g_mutex_lock(&state->mutex);
    copy = state->dashboard;
    g_mutex_unlock(&state->mutex);
    return copy;
}

DeviceTable app_state_get_ventilacion(AppState *state) {
    DeviceTable copy;
    g_mutex_lock(&state->mutex);
    copy = state->ventilacion;
    g_mutex_unlock(&state->mutex);
    return copy;
}

DeviceTable app_state_get_criadoras(AppState *state) {
    DeviceTable copy;
    g_mutex_lock(&state->mutex);
    copy = state->criadoras;
    g_mutex_unlock(&state->mutex);
    return copy;
}

DeviceTable app_state_get_bombas(AppState *state) {
    DeviceTable copy;
    g_mutex_lock(&state->mutex);
    copy = state->bombas;
    g_mutex_unlock(&state->mutex);
    return copy;
}

void app_state_set_api_status(AppState *state, const char *status) {
    g_mutex_lock(&state->mutex);
    strncpy(state->api_status, status, sizeof(state->api_status) - 1);
    state->api_status[sizeof(state->api_status) - 1] = '\0';
    g_mutex_unlock(&state->mutex);
}

void app_state_get_api_status(AppState *state, char *buffer, gsize size) {
    g_mutex_lock(&state->mutex);
    strncpy(buffer, state->api_status, size - 1);
    buffer[size - 1] = '\0';
    g_mutex_unlock(&state->mutex);
}

void app_state_stop(AppState *state) {
    g_mutex_lock(&state->mutex);
    state->running = 0;
    g_mutex_unlock(&state->mutex);
}

int app_state_is_running(AppState *state) {
    int running;
    g_mutex_lock(&state->mutex);
    running = state->running;
    g_mutex_unlock(&state->mutex);
    return running;
}
