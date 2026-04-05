#include <stdio.h>
#include <stdlib.h>
#include "db_manager.h"
#include "sqlite3.h"
#include "main_admin.h"
#include "config.h"

int baseDeDatos(){
	sqlite3 *db;

	    printf("Arrancando el servidor de Deusto Hardware...\n");

	    // Esto abre o crea el fichero de la BD
	    int resultado = sqlite3_open("data/deusto_hardware.sqlite", &db);

	    if (resultado != SQLITE_OK) {
	        printf("Error fatal al abrir la base de datos.\n");
	        return 1;
	    }

	    // Crea las tablas de la BD
	    inicializar_base_datos(db);

	    // TODO Código para la gestión de conexión

	    sqlite3_close(db);

	    return 0;
}
void menu(){
	char str[50];
	int opcion;
	printf("Menu principal\n");
	printf("1. Importar catalogo desde fichero\n2. Gestionar pedidos\n3. Gestionar productos\n4. Borrar base de datos\n5. Cerrar Sesion\n");
	printf("Opcion: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%d", &opcion);
	printf("%d\n", opcion);
	if(opcion == 1){
		importarFichero();
	}else if(opcion == 2){
		gestionarPedidos();
	}else if(opcion == 3){
		gestionarProductos();
	}else if(opcion == 4){
		borrarBase();
	}else if(opcion == 5){
		printf("Cerrando sesion\n");
		inicio();
	}else{
		printf("No es valido\n");
		menu();
	}
}
void importarFichero(){
    printf("Importando desde: %s\n", config.ruta_importacion);
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
void iniciarSesion(){
	char str[50];
	char username[50];
	char contrasenya[50];
	printf("Introduce el nombre de usuario: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%s", username);
	printf("%s\n", username);
	printf("Introduce tu contrasenya: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%s", contrasenya);
	printf("%s\n", contrasenya);
	//Comprobar el usuario
    if(strcmp(username, config.admin_usuario) == 0 &&
       strcmp(contrasenya, config.admin_contrasena) == 0){
        menu();
	}else{
		printf("error en el usuario o la contrasenya\n");
		iniciarSesion();
	}
}
void registrarAdmin(){
	char str[50];
	char username[50];
	char apellido[50];
	int telefono;
	char direccion[50];
	char ciudad[50];
	char contrasenya[50];
	printf("RegistrarAdmin\n");
	printf("Introduce el nombre de usuario: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%s", username);
	printf("%s\n", username);
	printf("introduce el apellido: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%s", apellido);
	printf("%s\n", apellido);
	printf("Introduce el telefono: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%d", &telefono);
	printf("%d\n", telefono);
	printf("Introduce tu direccion: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%s", direccion);
	printf("%s\n", direccion);
	printf("Introduce tu ciudad: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%s", ciudad);
	printf("%s\n", ciudad);
	printf("Introduce tu contrasenya: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%s", contrasenya);
	printf("%s\n", contrasenya);
	//registrarAdmin(username, apellido, telefono, direccion, ciudad, contrasenya);

    // TODO: Aquí iría el código para guardar en BD
    printf("Administrador %s registrado correctamente\n", username);
	inicio();
}

void inicio(){
	int opcion;
	char str[50];
	printf("Para moverte por el menu, introduce el numero correcto\n");
	printf("Gestion de Tienda\n1. Iniciar Sesion\n2. Registrar Administrador\n3. Salir\n");
	printf("Opcion: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	sscanf(str, "%d", &opcion);
	printf("%d\n", opcion);
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
