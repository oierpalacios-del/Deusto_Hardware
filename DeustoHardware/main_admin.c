#include <stdio.h>
#include <stdlib.h>
#include "db_manager.h"
#include "sqlite3.h"
#include "main_admin.h"
#include "config.h"
#include <string.h>
#include <stdbool.h>
#include "main.h"
#include <math.h>
#include "printer.h"
#include "gestionarPedidos.h"
#include "gestionarProductos.h"
#define MaxLine 50
void clearLines(char *str, int maxLine){
	if((strlen(str)==maxLine-1) && (str[maxLine-2]!='\n'))
		while(getchar() != '\n');
}
void menu(sqlite3 *db){
	bool permanecer = true;
	char str[MaxLine];
	char opcion;
	char c;
	while(permanecer){
		printf("Menu principal\n");
		printf("1. Importar catalogo desde fichero\n2. Gestionar pedidos\n3. Gestionar productos\n4. Borrar base de datos\n5. Cerrar Sesion\n6. Exportar datos\n");
		printf("Opcion: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%c", &opcion);
		printf("%c\n", opcion);
		if(opcion == '1'){
			importarFichero(db);
		}else if(opcion == '2'){
			gestionarPedidos(db);
		}else if(opcion == '3'){
			gestionarProductos(db);
		}else if(opcion == '4'){
			printf("Desea borrar la base de datos?\n");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			sscanf(str, "%c", &c);
			if(c == 's' || c == 'S'){
				printf("Borrando\n");
				borrarBase(db);
			}
		}else if(opcion == '5'){
			printf("Cerrando sesion\n");
			permanecer = false;
			inicio(db);
		}else if(opcion == '6'){
			exportarFichero(db);
		}else{
			printError(db, 6, "Opcion");
		}
	}
}
void importarFichero(sqlite3 *db){
    printf("Importando desde: %s\n", config.ruta_importacion);
}
void exportarFichero(sqlite3 *db){
	printf("Exportando a: %s\n", config.ruta_exportacion);
}
void gestionarPedidos(sqlite3 *db){
	bool permanecer = true;
	char str[MaxLine];
	char opcion;
	while(permanecer){
		printf("Gestion de Pedidos\n1. Visualizar pedidos\n2. Añadir pedidos\n3. Modificar pedidos\n4. Eliminar pedidos\n5. salir\n");
		printf("Opcion: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%c", &opcion);
		printf("%c\n", opcion);
		if(opcion == '1'){
			visualizarPedidos(db);
		}else if(opcion == '2'){
			anyadirPedidos(db);
		}else if(opcion == '3'){
			modificarPedidos(db);
		}else if(opcion == '4'){
			eliminarPedidos(db);
		}else if(opcion == '5'){
			printf("Saliendo\n");
			permanecer = false;
			menu(db);
		}else{
			printError(db, 6, "Opcion");
		}
	}
}
void gestionarProductos(sqlite3 *db){
	bool permanecer = true;
	char str[50];
	char opcion;
	while(permanecer){
		printf("Gestion de Productos\n1. Visualizar productos\n2. Añadir productos\n3. Modificar productos\n4. Eliminar productos\n5. salir\n");
		printf("Opcion: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		sscanf(str, "%c", &opcion);
		printf("%c\n", opcion);
		if(opcion == '1'){
			visualizarProductos(db);
		}else if(opcion == '2'){
			anyadirProductos(db);
		}else if(opcion == '3'){
			modificarProductos(db);
		}else if(opcion == '4'){
			eliminarProductos(db);
		}else if(opcion == '5'){
			printf("Saliendo\n");
			permanecer = false;
			menu(db);
		}else{
			printError(db, 6, "Opcion");
		}
	}
}
void visualizarPedidos(sqlite3 *db){
	sqlite3_stmt *stmt;
	int result;
	char sql[] = "select P.id_pedido, U.nombre, C.fecha_creacion, C.estado_compra, P.total from PEDIDO P, CARRITO C, USUARIO U where U.id_usuario = C.id_usuario and P.id_carrito = C.id_carrito";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			printPedido(sqlite3_column_int(stmt, 0), (char*)sqlite3_column_text(stmt, 1), (char*)sqlite3_column_text(stmt, 2), (char*)sqlite3_column_text(stmt, 3), sqlite3_column_double(stmt, 4));
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
}
void visualizarProveedores(sqlite3 *db){
	sqlite3_stmt *stmt;
	int result;
	char sql[] = "select * from Proveedor";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			printProveedor(sqlite3_column_int(stmt, 0), (char*)sqlite3_column_text(stmt, 1), (char*)sqlite3_column_text(stmt, 2), (char*)sqlite3_column_text(stmt, 3), (char*)sqlite3_column_text(stmt, 4), sqlite3_column_int(stmt, 5));
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
}
void visualizarProductos(sqlite3 *db){
	sqlite3_stmt *stmt;
	int result;
	char sql[] = "select P.id_producto, P.nombre, P.marca, P.precio, P.stock, C.nombre from PRODUCTO P, CATEGORIA C where C.id_categoria = P.id_categoria";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			printProducto(sqlite3_column_int(stmt, 0), (char*)sqlite3_column_text(stmt, 1), (char*)sqlite3_column_text(stmt, 2), sqlite3_column_double(stmt, 3), sqlite3_column_int(stmt, 4), (char*)sqlite3_column_text(stmt, 5));
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
}
void visualizarCarrito(sqlite3 *db, int id){
	sqlite3_stmt *stmt;
	int result;
	char sql[] = "select C.id_carrito from CARRITO C where C.id_usuario = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, id);
	printf("Hemos encontrado los siguientes pedidos: \n");
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			printf("%d\n", sqlite3_column_int(stmt, 0));
		}
	} while(result == SQLITE_ROW);
}
int getUsuarioId(sqlite3 *db){
	int result;
	char str[MaxLine];
	char username[MaxLine];
	sqlite3_stmt *stmt;
	int id = 0;
	printf("Introduce el nombre de usuario: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%s", username);
	printf("%s\n", username);
	char sql1[] = "select U.id_usuario from USUARIO U where U.nombre = ?";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, username, strlen(username), SQLITE_STATIC);
	printf("\n");
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			id = sqlite3_column_int(stmt, 0);
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	return id;
}
void getTotal(sqlite3 *db, int idCar, double *precio, double *precioAntiguo){
	int result;
	double total;
	double precioTemp = *precio;
	double precioAntiguoTemp = *precioAntiguo;
	sqlite3_stmt *stmt;
	char sql1[] = "select total from PEDIDO where id_carrito = ?";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idCar);
	printf("\n");
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			total = sqlite3_column_double(stmt, 0);
			total = round(total * 100.0) / 100.0;
			total = total - precioAntiguoTemp;
			total = total + precioTemp;
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	actualizarTotal(db, idCar, total);
}
void actualizarTotal(sqlite3 *db, int idCar, double total){
	int result;
	sqlite3_stmt *stmt;
	char sql[] = "update PEDIDO SET total = ? where id_carrito = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_double(stmt, 1, total);
	sqlite3_bind_int(stmt, 2, idCar);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Pedido");
	}else{
		printSucces(1, "Total");
		printf("Total actualizado\n");
	}
}
bool continuar(){
	char c;
	char str[MaxLine];
	printf("Desea continuar?\n");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%c", &c);
	if(c == 's' || c == 'S'){
		return true;
	}else{
		return false;
	}
}
bool comprobarUsuario(sqlite3 *db, char username[MaxLine], char contrasenya[MaxLine]){
	int result;
	sqlite3_stmt *stmt;
	char sql[] = "select U.nombre, U.contrasena from USUARIO U";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	printf("\n");
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			char*userSelect = (char*) sqlite3_column_text(stmt, 0);
			char*contraSelect = (char*) sqlite3_column_text(stmt, 1);
			if(strcmp(userSelect, username) == 0 && strcmp(contraSelect, contrasenya) == 0){
				return false;
			}
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	return true;
}
void iniciarSesion(sqlite3 *db){
	bool permanecer = true;
	int cantidad = 0;
	char str[MaxLine];
	char username[MaxLine];
	char contrasenya[MaxLine];
	while(permanecer){
		printf("Introduce el nombre de usuario: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%s", username);
		printf("%s\n", username);
		printf("Introduce tu contrasenya: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%s", contrasenya);
		printf("%s\n", contrasenya);
		permanecer = comprobarUsuario(db, username, contrasenya);

		if(permanecer && cantidad < 3){
			printError(db, 4, "");
			cantidad++;
		}
		if(!permanecer){
			printf("Bienvenido %s\n", username);
			menu(db);
		}
		if(cantidad == 3){
			printf("Cantidad maxima de oportunidades alcanzado. Regresando\n");
			permanecer = false;
			cantidad = 0;
			inicio(db);
		}
	}
}
bool registrarAdminDB(sqlite3 *db, char username[MaxLine], char apellido[MaxLine], int telefono, char email[MaxLine], char ciudad[MaxLine], char contrasenya[MaxLine]){
	sqlite3_stmt *stmt;
	int result;
	char admin[] = "ADMIN";
	char sql1[] = "select U.nombre from USUARIO U where U.rol = ?";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, admin, strlen(admin), SQLITE_STATIC);
	printf("\n");
	printf("Mostrando admins: \n");
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			char*adminSelect = (char*) sqlite3_column_text(stmt, 0);
			printf("%s\n", (char*) sqlite3_column_text(stmt, 0));
			if(strcmp(adminSelect, username) == 0){
				return false;
			}
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	char sql2[] = "select C.id_ciudad from CIUDAD C where C.nombre = ?";
	sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, ciudad, strlen(ciudad), SQLITE_STATIC);
	printf("\n");
	printf("Mostrando id de ciudades: \n");
	int id_ciudad;
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			id_ciudad = (int) sqlite3_column_int(stmt, 0);
			printf("%i\n", id_ciudad);
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	char sql3[] = "insert or ignore into USUARIO (nombre, apellidos, email, contrasena, rol, id_ciudad) values (?, ?, ?, ?, ?, ?)";
	sqlite3_prepare_v2(db, sql3, strlen(sql3), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, username, strlen(username), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, apellido, strlen(apellido), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, email, strlen(email), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, contrasenya, strlen(contrasenya), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, admin, strlen(admin), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 6, id_ciudad);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result != SQLITE_DONE){
		printError(db, 0, "Admin");
		return false;
	}
	else{
		printSucces(2, "Administrador");
		return true;
	}
}
void registrarAdmin(sqlite3 *db){
	bool permanecer = true;
	while(permanecer){
		bool sonNumeros = true;
		bool hayLetra = false;
		char str[MaxLine];
		char username[MaxLine];
		char apellido[MaxLine];
		char telefonoTxt[MaxLine];
		int telefono;
		char email[MaxLine];
		char ciudad[MaxLine];
		char contrasenya[MaxLine];
		printf("RegistrarAdmin\n");
		printf("Introduce el nombre de usuario: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%s", username);
		printf("%s\n", username);
		printf("introduce el apellido: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%s", apellido);
		printf("%s\n", apellido);
		printf("Introduce el telefono: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%s", telefonoTxt);
		printf("%s\n", telefonoTxt);
		for(int i = 0; i < strlen(telefonoTxt); i++){
			char c = telefonoTxt[i];
			if(!(c >= '0' && c <='9')){
				hayLetra = true;
				break;
			}
		}
		if(hayLetra){
			sonNumeros = false;
			hayLetra = false;
		}else{
			sonNumeros = true;
		}
		while(strlen(telefonoTxt) != 9 || !sonNumeros){
			if(!sonNumeros){
				printError(db, 5, "");
			}
			if(strlen(telefonoTxt) > 9){
				printError(db, 5, "");
			}
			if(strlen(telefonoTxt) < 9){
				printError(db, 5, "");
			}
			printf("Introduce el telefono: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			sscanf(str, "%s", telefonoTxt);
			printf("%s\n", telefonoTxt);
			for(int i = 0; i < strlen(telefonoTxt); i++){
				char c = telefonoTxt[i];
				if(!(c >= '0' && c <='9')){
					hayLetra = true;
					break;
				}
			}
			if(hayLetra){
				sonNumeros = false;
				hayLetra = false;
			}else{
				sonNumeros = true;
			}
		}
		telefono = (int) telefonoTxt;
		printf("Introduce tu Email: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%s", email);
		printf("%s\n", email);
		printf("Introduce tu ciudad: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%s", ciudad);
		printf("%s\n", ciudad);
		printf("Introduce tu contrasenya: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%s", contrasenya);
		printf("%s\n", contrasenya);
		permanecer = registrarAdminDB(db, username, apellido, telefono, email, ciudad, contrasenya);
		if(permanecer){
			printSucces(2, "Adiministrador");
			permanecer = false;
			inicio(db);
		}
	}
}

void inicio(sqlite3 *db){
	bool permanecer = true;
	char opcion;
	char str[MaxLine];
	while(permanecer){
		printf("Para moverte por el menu, introduce el numero correcto\n");
		printf("En campos de texto, por favor poner en mayusculas el texto\n");
		printf("Gestion de Tienda\n1. Iniciar Sesion\n2. Registrar Administrador\n3. Salir\n");
		printf("Opcion: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%c", &opcion);
		printf("%c\n", opcion);
		if(opcion == '1'){
			permanecer = false;
			iniciarSesion(db);
		}else if(opcion == '2'){
			permanecer = false;
			registrarAdmin(db);
		}else if(opcion == '3'){
			permanecer = false;
			serverOAdmin(db);
		}else{
			printError(db, 6, "Opcion");
		}
	}
}
