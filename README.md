# AviMax Frontend C + GTK para Raspberry Pi

## Qué incluye
- Dashboard principal
- Pantalla de menú
- Pantallas de Ventilación, Criadoras y Bombas
- Consumo de API REST con `libcurl`
- Parseo JSON con `cJSON`
- Espacios listos para iconos PNG en `assets/icons/`

## Iconos esperados
Coloca tus PNG con estos nombres:
- `assets/icons/logo.png`
- `assets/icons/temperatura.png`
- `assets/icons/humedad.png`
- `assets/icons/amoniaco.png`
- `assets/icons/ventilacion.png`
- `assets/icons/criadoras.png`
- `assets/icons/bombas.png`
- `assets/icons/parvada.png`

Si falta un icono, la interfaz mostrará un texto de respaldo.

## Endpoints esperados
### GET /api/dashboard
```json
{
  "diaLote": 45,
  "temperatura": 28.5,
  "humedad": 30.0,
  "amoniaco": 15.0
}
```

### GET /api/ventilacion
### GET /api/criadoras
### GET /api/bombas
```json
[
  { "id": 1, "entrada": 25.8, "salida": 25.8, "tiempoSegundos": 0, "activo": false },
  { "id": 2, "entrada": 25.8, "salida": 25.8, "tiempoSegundos": 0, "activo": false }
]
```

## Compilar
```bash
make
```

## Ejecutar
```bash
AVIMAX_API_URL=http://127.0.0.1:8080 make run
```
