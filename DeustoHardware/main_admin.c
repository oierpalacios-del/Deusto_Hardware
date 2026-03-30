#include <stdio.h>
#include <stdlib.h>
#include "db_manager.h"
void menu(){
	char str[50];
	int opcion;
	printf("Menu principal\n");
	printf("1. Importar catalogo desde fichero\n 2. Gestionar pedidos\n 3. Gestionar productos\n 4. Borrar base de datos\n 5. Cerrar Sesion\n");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%d", &opcion);
	printf("Has escrito: %d\n", opcion);
	if(opcion == 1){
		importarFichero();
	}else if(opcion == 2){
		gestionarPedidos();
	}else if(opcion == 3){
		gestionarProductos();
	}else if(opcion == 4){
		borrarBase();
	}else if(opcion == 5){
		cerrarSesion();
	}else{
		printf("No es valido\n");
		menu();
	}
}
void importarFichero(){
	printf("importando\n");
}
void gestionarPedidos(){
	printf("gestionando\n");
}
void gestionarProductos(){
	printf("gestionando\n");
}
void borrarBase(){
	printf("borrando");
}
void cerrarSesion(){
	inicio();
}
void iniciarSesion(){

}
void registrarAdmin(){

}
void inicio(){
	int opcion;
	char str[50];
	printf("Para moverte por el menu, introduce el numero correcto\n");
	printf("Gestion de Tienda\n 1. Iniciar Sesion\n 2. Registrar Administrador\n 3. Salir\n");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%d", &opcion);
	printf("Has escrito: %d\n", opcion);
	if(opcion == 1){
		iniciarSesion();
	}else if(opcion == 2){
		registrarAdmin();
	}else if(opcion == 3){
		exit(0);
	}else{
		printf("No es valido\n");
		inicio();
	}
}
