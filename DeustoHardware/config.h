#ifndef CONFIG_H_
#define CONFIG_H_

#define MAX_RUTA 256
#define MAX_VALOR 100

typedef struct {
    char admin_usuario[MAX_VALOR];
    char admin_contrasena[MAX_VALOR];
    char bd_ruta[MAX_RUTA];
    char ruta_importacion[MAX_RUTA];
    char ruta_exportacion[MAX_RUTA];
    char log_ruta[MAX_RUTA];
} Configuracion;

int cargar_configuracion(const char* ruta_archivo, Configuracion* cfg);
void mostrar_configuracion(const Configuracion* cfg);

extern Configuracion config;

#endif
