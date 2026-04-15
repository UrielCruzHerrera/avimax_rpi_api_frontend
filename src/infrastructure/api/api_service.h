#ifndef API_SERVICE_H
#define API_SERVICE_H

#include <pthread.h>
#include "../../shared/app_state.h"

typedef struct {
    char base_url[256];
    int polling_ms;
} ApiConfig;

typedef struct {
    AppState *state;
    ApiConfig config;
    pthread_t thread;
    int started;
} ApiService;

void api_service_init(ApiService *service, AppState *state, const char *base_url, int polling_ms);
void api_service_start(ApiService *service);
void api_service_stop(ApiService *service);

#endif
