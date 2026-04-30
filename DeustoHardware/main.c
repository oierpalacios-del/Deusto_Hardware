/*
 * main.c
 *
 *  Created on: 30 mar 2026
 *      Author: oipac
 */
#include <stdlib.h>
#include <stdio.h>
#include "main_server.h"
#include "main_admin.h"
#include "main.h"
#include "config.h"
#include <stdbool.h>
#include "sqlite3.h"
#include <string.h>
#include "db_manager.h"
#define MaxLine 50
Configuracion config;

int main(int argc, char **argv) {
	sqlite3 *db;



    if (cargar_configuracion("config/config.txt", &config) == 0) {
        printf("Configuración cargada correctamente.\n");
        mostrar_configuracion(&config);
    } else {
        printf("ADVERTENCIA: Usando configuración por defecto.\n");
    }
    printf("Arrancando el servidor de Deusto Hardware...\n");

    	// Esto abre o crea el fichero de la BD
    	printf("Usando base de datos: %s\n", config.bd_ruta);
    	int resultado = sqlite3_open(config.bd_ruta, &db);

    	if (resultado != SQLITE_OK) {
    	    printf("Error fatal al abrir la base de datos: %s\n", sqlite3_errmsg(db));
    	    return resultado;
    	}

    	// Crea las tablas de la BD
    	inicializar_base_datos(db);

    	rellenar_base_datos(db);
    	//TODO Código para la gestión de conexión

    	printf("Bienvenido a Deusto Hardware\n");
	serverOAdmin(db);
}
void serverOAdmin(sqlite3 *db){
	bool permanecer = true;
	int opcion;
	while(permanecer){
		char str[MaxLine];
		printf("1. Servidor\n2. Admin\n");
		printf("Opcion: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%d", &opcion);
		printf("%d\n", opcion);
		if(opcion == 1){
			server(db);
		}else if(opcion == 2){
			inicio(db);
		}else{
			printf("no es valido\n");
		}
	}
}


