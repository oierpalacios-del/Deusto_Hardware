#include <stdio.h>
#include "sqlite3.h"
#include "db_manager.h"
#include "config.h"

int server(void) {
    sqlite3 *db;

    printf("Arrancando el servidor de Deusto Hardware...\n");

    // Esto abre o crea el fichero de la BD
    printf("Usando base de datos: %s\n", config.bd_ruta);
    int resultado = sqlite3_open(config.bd_ruta, &db);

    if (resultado != SQLITE_OK) {
        printf("Error fatal al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Crea las tablas de la BD
    inicializar_base_datos(db);
    rellenar_base_datos(db);
    // TODO Código para la gestión de conexión

    sqlite3_close(db);

    return 0;
}
