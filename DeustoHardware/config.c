#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int cargar_configuracion(const char* ruta_archivo, Configuracion* cfg) {
    FILE* f = fopen(ruta_archivo, "r");
    if (!f) {
        printf("ERROR: No se pudo abrir %s\n", ruta_archivo);
        printf("Usando valores por defecto...\n");

        strcpy(cfg->admin_usuario, "admin");
        strcpy(cfg->admin_contrasena, "admin123");
        strcpy(cfg->bd_ruta, "data/deusto_hardware.sqlite");
        strcpy(cfg->ruta_importacion, "data/importar/");
        strcpy(cfg->ruta_exportacion, "data/exportar/");
        strcpy(cfg->log_ruta, "logs/servidor.log");
        return -1;
    }

    strcpy(cfg->admin_usuario, "admin");
    strcpy(cfg->admin_contrasena, "admin123");
    strcpy(cfg->bd_ruta, "data/deusto_hardware.sqlite");
    strcpy(cfg->ruta_importacion, "data/importar/");
    strcpy(cfg->ruta_exportacion, "data/exportar/");
    strcpy(cfg->log_ruta, "logs/servidor.log");

    char linea[256];
    char clave[100], valor[200];

    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '#' || linea[0] == '\n') continue;

        linea[strcspn(linea, "\n")] = 0;

        char* igual = strchr(linea, '=');
        if (igual == NULL) continue;

        *igual = '\0';
        strcpy(clave, linea);
        strcpy(valor, igual + 1);

        char* p = clave;
        while (*p == ' ') p++;
        char* end = p + strlen(p) - 1;
        while (end > p && *end == ' ') end--;
        *(end + 1) = '\0';

        p = valor;
        while (*p == ' ') p++;
        end = p + strlen(p) - 1;
        while (end > p && *end == ' ') end--;
        *(end + 1) = '\0';

        if (strcmp(p, "ADMIN_USUARIO") == 0) {
            strcpy(cfg->admin_usuario, valor);
        } else if (strcmp(p, "ADMIN_CONTRASENA") == 0) {
            strcpy(cfg->admin_contrasena, valor);
        } else if (strcmp(p, "BD_RUTA") == 0) {
            strcpy(cfg->bd_ruta, valor);
        } else if (strcmp(p, "RUTA_IMPORTACION") == 0) {
            strcpy(cfg->ruta_importacion, valor);
        } else if (strcmp(p, "RUTA_EXPORTACION") == 0) {
            strcpy(cfg->ruta_exportacion, valor);
        } else if (strcmp(p, "LOG_RUTA") == 0) {
            strcpy(cfg->log_ruta, valor);
        }
    }

    fclose(f);
    return 0;
}

void mostrar_configuracion(const Configuracion* cfg) {
    printf("\n=== CONFIGURACIÓN CARGADA ===\n");
    printf("Admin usuario: %s\n", cfg->admin_usuario);
    printf("Admin contraseña: %s\n", cfg->admin_contrasena);
    printf("Ruta base de datos: %s\n", cfg->bd_ruta);
    printf("Ruta importación: %s\n", cfg->ruta_importacion);
    printf("Ruta exportación: %s\n", cfg->ruta_exportacion);
    printf("Ruta logs: %s\n", cfg->log_ruta);
    printf("==============================\n\n");
}
