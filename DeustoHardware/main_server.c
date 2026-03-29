#include <stdio.h>
#include "sqlite3.h"
#include "db_manager.h"

int main(void) {
    sqlite3 *db;

    printf("Arrancando el servidor de Deusto Hardware...\n");

    // Esto abre o crea el fichero de la BD
    int resultado = sqlite3_open("data/deusto_hardware.sqlite", &db); //

    if (resultado != SQLITE_OK) { //
        printf("Error fatal al abrir la base de datos.\n");
        return 1;
    }

    // Crea las tablas de la BD
    inicializar_base_datos(db);

    // TODO Código para la gestión de conexión

    sqlite3_close(db);

    return 0;
}
