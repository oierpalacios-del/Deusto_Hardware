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

Configuracion config;

int main(int argc, char **argv) {
	printf("Bienvenido a Deusto Hardware\n");

    if (cargar_configuracion("config/config.txt", &config) == 0) {
        printf("Configuración cargada correctamente.\n");
        mostrar_configuracion(&config);
    } else {
        printf("ADVERTENCIA: Usando configuración por defecto.\n");
    }

	serverOAdmin();
}
void serverOAdmin(){
	int opcion;
	char str[50];
	printf("1. Servidor\n2. Admin\n");
	printf("Opcion: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%d", &opcion);
	printf("%d\n", opcion);
	if(opcion == 1){
		server();
	}else if(opcion == 2){
		inicio();
	}else{
		printf("no es valido\n");
		serverOAdmin();
	}
}


