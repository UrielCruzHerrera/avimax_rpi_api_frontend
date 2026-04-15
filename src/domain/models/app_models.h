#ifndef APP_MODELS_H
#define APP_MODELS_H

#define AVIMAX_MAX_ROWS 16

typedef struct {
    float temperatura;
    float humedad;
    float amoniaco;
    int dia_lote;
} DashboardData;

typedef struct {
    int id;
    float entrada;
    float salida;
    int tiempo_segundos;
    int activo;
} DeviceRow;

typedef struct {
    DeviceRow rows[AVIMAX_MAX_ROWS];
    int count;
} DeviceTable;

#endif
