#include "api_service.h"
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <unistd.h>

typedef struct {
    char *memory;
    size_t size;
} MemoryBlock;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryBlock *mem = (MemoryBlock *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}

static int http_get_json(const char *url, char **response_out) {
    CURL *curl = curl_easy_init();
    if (!curl) return 0;

    MemoryBlock chunk = {0};
    long http_code = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || http_code >= 400 || !chunk.memory) {
        free(chunk.memory);
        return 0;
    }

    *response_out = chunk.memory;
    return 1;
}

static int parse_dashboard(const char *json_text, DashboardData *out) {
    cJSON *root = cJSON_Parse(json_text);
    if (!root) return 0;

    cJSON *dia = cJSON_GetObjectItemCaseSensitive(root, "diaLote");
    cJSON *temp = cJSON_GetObjectItemCaseSensitive(root, "temperatura");
    cJSON *hum = cJSON_GetObjectItemCaseSensitive(root, "humedad");
    cJSON *nh3 = cJSON_GetObjectItemCaseSensitive(root, "amoniaco");

    if (!cJSON_IsNumber(dia) || !cJSON_IsNumber(temp) || !cJSON_IsNumber(hum) || !cJSON_IsNumber(nh3)) {
        cJSON_Delete(root);
        return 0;
    }

    out->dia_lote = dia->valueint;
    out->temperatura = (float)temp->valuedouble;
    out->humedad = (float)hum->valuedouble;
    out->amoniaco = (float)nh3->valuedouble;

    cJSON_Delete(root);
    return 1;
}

static int parse_table(const char *json_text, DeviceTable *out) {
    cJSON *root = cJSON_Parse(json_text);
    if (!root || !cJSON_IsArray(root)) {
        cJSON_Delete(root);
        return 0;
    }

    out->count = 0;
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, root) {
        if (out->count >= AVIMAX_MAX_ROWS) break;
        cJSON *id = cJSON_GetObjectItemCaseSensitive(item, "id");
        cJSON *entrada = cJSON_GetObjectItemCaseSensitive(item, "entrada");
        cJSON *salida = cJSON_GetObjectItemCaseSensitive(item, "salida");
        cJSON *tiempo = cJSON_GetObjectItemCaseSensitive(item, "tiempoSegundos");
        cJSON *activo = cJSON_GetObjectItemCaseSensitive(item, "activo");

        if (!cJSON_IsNumber(id) || !cJSON_IsNumber(entrada) || !cJSON_IsNumber(salida)) {
            continue;
        }

        DeviceRow *row = &out->rows[out->count++];
        row->id = id->valueint;
        row->entrada = (float)entrada->valuedouble;
        row->salida = (float)salida->valuedouble;
        row->tiempo_segundos = cJSON_IsNumber(tiempo) ? tiempo->valueint : 0;
        row->activo = cJSON_IsBool(activo) ? cJSON_IsTrue(activo) : 0;
    }

    cJSON_Delete(root);
    return 1;
}

static void build_url(char *buffer, size_t size, const char *base, const char *path) {
    snprintf(buffer, size, "%s%s", base, path);
}

static void poll_dashboard(ApiService *service) {
    char url[384];
    char *response = NULL;
    DashboardData dashboard;

    build_url(url, sizeof(url), service->config.base_url, "/api/dashboard");
    if (!http_get_json(url, &response)) {
        app_state_set_api_status(service->state, "Error consultando /api/dashboard");
        return;
    }

    if (parse_dashboard(response, &dashboard)) {
        app_state_set_dashboard(service->state, &dashboard);
        app_state_set_api_status(service->state, "API conectada");
    } else {
        app_state_set_api_status(service->state, "JSON inválido en dashboard");
    }

    free(response);
}

static void poll_table(ApiService *service, const char *path, void (*setter)(AppState *, const DeviceTable *)) {
    char url[384];
    char *response = NULL;
    DeviceTable table;

    build_url(url, sizeof(url), service->config.base_url, path);
    if (!http_get_json(url, &response)) {
        return;
    }

    if (parse_table(response, &table)) {
        setter(service->state, &table);
    }

    free(response);
}

static void *api_loop(void *arg) {
    ApiService *service = (ApiService *)arg;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    while (app_state_is_running(service->state)) {
        poll_dashboard(service);
        poll_table(service, "/api/ventilacion", app_state_set_ventilacion);
        poll_table(service, "/api/criadoras", app_state_set_criadoras);
        poll_table(service, "/api/bombas", app_state_set_bombas);

        g_usleep((gulong)service->config.polling_ms * 1000);
    }

    curl_global_cleanup();
    return NULL;
}

void api_service_init(ApiService *service, AppState *state, const char *base_url, int polling_ms) {
    service->state = state;
    snprintf(service->config.base_url, sizeof(service->config.base_url), "%s", base_url);
    service->config.polling_ms = polling_ms;
    service->started = 0;
}

void api_service_start(ApiService *service) {
    if (service->started) return;
    service->started = 1;
    pthread_create(&service->thread, NULL, api_loop, service);
}

void api_service_stop(ApiService *service) {
    if (!service->started) return;
    pthread_join(service->thread, NULL);
    service->started = 0;
}
