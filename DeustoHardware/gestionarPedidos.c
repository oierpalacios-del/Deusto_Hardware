/*
 * gestionarPedidos.c
 *
 *  Created on: 6 may 2026
 *      Author: oipac
 */
#include "gestionarPedidos.h"
#include "gestionarProductos.h"
#define MaxLine 50
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "main_admin.h"
#include "printer.h"
#include <math.h>
bool crearCarrito(sqlite3 *db, int *idCarrito){
	sqlite3_stmt *stmt;
	char estado[MaxLine] = "en proceso";
	char str[MaxLine];
	char username[MaxLine];
	int result;
	int id = 0;
	char fecha[MaxLine];
	printf("Bienvenido a la creación del carrito, por favor introduzca los siguientes datos:\n");
	printf("Introduce el nombre de usuario: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%s", username);
	printf("%s\n", username);
	sqlite3_prepare_v2(db, "SELECT DATE('now');", -1, &stmt, NULL);
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		strcpy(fecha, (char*)sqlite3_column_text(stmt, 0));
	}
	sqlite3_finalize(stmt);
	char sql1[] = "select U.id_usuario from USUARIO U where U.nombre = ?";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, username, strlen(username), SQLITE_STATIC);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			id = sqlite3_column_int(stmt, 0);
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	if(id == 0){
		printError(db, 1, "Usuario");
		return false;
	}
	char sql2[] = "insert or ignore into CARRITO (fecha_creacion, estado_compra, id_usuario) values (?, ?, ?)";
	sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, fecha, strlen(fecha), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, estado, strlen(estado), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, id);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result != SQLITE_DONE){
		printError(db, 0, "Carrito");
		return false;
	}
	else{
		printf("Carrito creado exitosamente\n");
		char sql3[] = "select C.id_carrito from CARRITO C where fecha_creacion = ? and id_usuario = ?";
		sqlite3_prepare_v2(db, sql3, strlen(sql3), &stmt, NULL);
		sqlite3_bind_text(stmt, 1, fecha, strlen(fecha), SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, id);
		do{
			result = sqlite3_step(stmt);
			if(result == SQLITE_ROW){
				*idCarrito = sqlite3_column_int(stmt, 0);
			}
		} while(result == SQLITE_ROW);
		sqlite3_finalize(stmt);
		return true;
	}
}
bool anyadirProductoCarrito(sqlite3 *db, int *idCarrito, double *total){
	sqlite3_stmt *stmt;
	bool correcto = false;
	int result;
	int idProd = 0;
	int idCar = *idCarrito;
	int cantidad;
	int cantidadProd;
	double precio;
	double tot = 0;
	char str[MaxLine];
	char nomProd[MaxLine];
	printf("Bienvenido, cuantos productos diferentes quieres adquirir?\n");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%i", &cantidad);
	printf("%i\n", cantidad);
	for(int i = 0; i < cantidad; i++){
		printf("Introduce el nombre del producto: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		str[strcspn(str, "\n")] = '\0';
		strncpy(nomProd, str, MaxLine);
		printf("%s\n", nomProd);
		char sql1[] = "select P.id_producto, P.precio from PRODUCTO P where P.nombre = ?";
		sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
		sqlite3_bind_text(stmt, 1, nomProd, strlen(nomProd), SQLITE_STATIC);
		do{
			result = sqlite3_step(stmt);
			if(result == SQLITE_ROW){
				idProd = sqlite3_column_int(stmt, 0);
				precio = sqlite3_column_double(stmt, 1);
				precio = round(precio * 100.0) / 100.0;
			}
		} while(result == SQLITE_ROW);
		sqlite3_finalize(stmt);
		if(idProd == 0){
			printError(db, 1, "Producto");
			correcto = correcto || false;
		}else{
			correcto = correcto || true;
			printf("Introduce la cantidad del producto: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			sscanf(str, "%i", &cantidadProd);
			printf("%i\n", cantidadProd);
			precio = cantidadProd * precio;
			tot += precio;
			char sql2[] = "insert or ignore into LINEA_CARRITO (id_carrito, id_producto, cantidad, precio_unitrio) values (?, ?, ?, ?)";
			sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
			sqlite3_bind_int(stmt, 1, idCar);
			sqlite3_bind_int(stmt, 2, idProd);
			sqlite3_bind_int(stmt, 3, cantidadProd);
			sqlite3_bind_double(stmt, 4, precio);
			result = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			if(result != SQLITE_DONE){
				printError(db, 0, "Linea-carrito");
				return false;
			}
			else{
				printf("Linea-carrito creado exitosamente\n");
			}
		}
	}
	*total = tot;
	return correcto;
}
bool crearPedido(sqlite3 *db, int *idCarrito, double *total){
	sqlite3_stmt *stmt;
	int result;
	double tot = *total;
	int dias = rand() % 3650;
	int idCar = *idCarrito;
	char fecha[MaxLine];
	char sql1[] = "SELECT DATE('now', ?)";
	char modificador[20];
	snprintf(modificador, sizeof(modificador), "+%d days", dias);
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, modificador, strlen(modificador), SQLITE_STATIC);
	result = sqlite3_step(stmt);
	if(result == SQLITE_ROW){
		strcpy(fecha, (char*)sqlite3_column_text(stmt, 0));
	}
	sqlite3_finalize(stmt);
	char sql2[] = "insert or ignore into PEDIDO (fecha_envio, total, id_carrito) values (?, ?, ?)";
	sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, fecha, strlen(fecha), SQLITE_STATIC);
	sqlite3_bind_double(stmt, 2, tot);
	sqlite3_bind_int(stmt, 3, idCar);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result != SQLITE_DONE){
		printError(db, 0, "Pedido");
		return false;
	}
	else{
		printf("Pedido creado exitosamente\n");
		return true;
	}
}
void eliminarCarrito(sqlite3 *db, int *idCarrito){
	char sql[] = "delete from CARRITO where id_carrito = ?";
	int idCar = *idCarrito;
	int result;
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idCar);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result !=SQLITE_DONE){
		printError(db, 2, "Carrito");
	}else{
		printf("Carrito eliminado\n");
	}
}
void eliminarLineaCarrito(sqlite3 *db, int *idCarrito){
	char sql[] = "delete from LINEA_CARRITO where id_carrito = ?";
	int idCar = *idCarrito;
	int result;
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idCar);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result !=SQLITE_DONE){
		printError(db, 2, "Linea-carrito");
	}else{
		printf("Linea-carrito eliminado\n");
	}
}
void anyadirPedidos(sqlite3 *db){
	bool correcto;
	bool permanecer = true;
	int *idCarrito = malloc(sizeof(int));
	double *total = (double*)malloc(sizeof(double));
	printf("Para añadir un Pedido tendrá que crear un carrito, y añadirle productos\n");
	printf("Esto es un proceso de 3 partes, se le avisará cuando complete cada una\n");
	printf("Por seguridad y para evitar redundancia, cada vez que haya un error, habrá que volver a empezar desde el principio\n");
	while(permanecer){
		printf("Para empezar, tiene que crear un carrito\n");
		correcto = crearCarrito(db, idCarrito);
		if(correcto){
			printf("Fase 1/3 completado. Ahora tiene que añadirle productos al carrito\n");
			correcto = correcto && anyadirProductoCarrito(db, idCarrito, total);
		}else{
			printf("Error en la creación del carrito, regresando\n");
			permanecer = false;
			free(idCarrito);
			free(total);
			gestionarPedidos(db);
		}
		if(correcto){
			printf("Fase 2/3 completado. Ahora se creara el pedido\n");
			correcto = correcto && crearPedido(db, idCarrito, total);
		}else {
			printf("Error en la creación de linea-carrito, eliminando carrito\n");
			permanecer = false;
			eliminarCarrito(db, idCarrito);
			free(idCarrito);
			free(total);
			printf("Regresando\n");
			gestionarPedidos(db);
		}
		if(correcto){
			printf("Fase 3/3 completado. Pedido listo\n");
			permanecer = continuar();
			if(!permanecer){
				free(idCarrito);
				free(total);
			}
		}else{
			permanecer = false;
			printf("Error en la cracion del Pedido, eliminando linea-carrito\n");
			eliminarLineaCarrito(db, idCarrito);
			printf("Eliminando carrito\n");
			eliminarCarrito(db, idCarrito);
			free(idCarrito);
			free(total);
			printf("Regresando\n");
			gestionarPedidos(db);
		}
	}
}
void eliminarPedidos(sqlite3 *db){
	int *idCarrito;
	int idCar;
	sqlite3_stmt *stmt;
	int id = 0;
	id = getUsuarioId(db);
	int result;
	char str[MaxLine];
	char c;
	if(id != 0){
		visualizarCarrito(db, id);
		printf("Cual desea eliminar?: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%i", &idCar);
		printf("%i\n", idCar);
		idCarrito = &idCar;
		printf("Desea eliminar el pedido %i?\n", idCar);
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%c", &c);
		if(c == 's' || c == 'S'){
			char sql3[] = "delete from PEDIDO where id_carrito = ?";
			sqlite3_prepare_v2(db, sql3, strlen(sql3), &stmt, NULL);
			sqlite3_bind_int(stmt, 1, idCar);
			result = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			if(result !=SQLITE_DONE){
				printError(db, 2, "Pedido");
			}else{
				printf("Pedido eliminado\n");
			}
			eliminarLineaCarrito(db, idCarrito);
			eliminarCarrito(db, idCarrito);
		}
	}else{
		printError(db, 1, "Usuario");
	}
}
void cambiarCantidad(sqlite3 *db, int idCar){
	char str[MaxLine];
	sqlite3_stmt *stmt;
	int result;
	double precioTemp;
	int *idProd = (int*)malloc(sizeof(int));
	double *precio = (double*)malloc(sizeof(double));
	double *precioAntiguo = (double*)malloc(sizeof(double));
	int cantidadProd;
	getProductoCarrito(db, idProd, precio, precioAntiguo);
	precioTemp = *precio;
	printf("Introduce la cantidad nueva del producto: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%i", &cantidadProd);
	printf("%i\n", cantidadProd);
	precioTemp = cantidadProd * precioTemp;
	char sql[] = "update LINEA_CARRITO SET cantidad = ? , precio_unitrio = ? where id_producto = ? and id_carrito = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, cantidadProd);
	sqlite3_bind_double(stmt, 2, precioTemp);
	sqlite3_bind_int(stmt, 3, *idProd);
	sqlite3_bind_int(stmt, 4, idCar);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Linea-Carrito");
	}else{
		printf("Cantidad actualizada\n");
		*precio = precioTemp;
		getTotal(db, idCar, precio, precioAntiguo);
	}
	free(precioAntiguo);
	free(idProd);
	free(precio);
}
void eliminarProductoCarrito(sqlite3 *db, int idCar){
	sqlite3_stmt *stmt;
	int result;
	int *idProd = (int*)malloc(sizeof(int));
	double *precio = (double*)malloc(sizeof(double));
	double *precioAntiguo = (double*)malloc(sizeof(double));
	getProductoCarrito(db, idProd, precio, precioAntiguo);
	*precio = 0.0;
	char sql[] = "delete from LINEA_CARRITO where id_carrito = ? and id_producto = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idCar);
	sqlite3_bind_int(stmt, 2, *idProd);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result != SQLITE_DONE){
		printError(db, 2, "Producto");
	}else{
		printf("Producto eliminado\n");
		getTotal(db, idCar, precio, precioAntiguo);
	}
	free(precioAntiguo);
	free(idProd);
	free(precio);
}
void modificarPedidos(sqlite3 *db){
	char str[MaxLine];
	int idCar;
	int id = 0;
	int opcion;
	double total;
	bool permanecer = true;
	id = getUsuarioId(db);
	while(id == 0){
		printError(db, 1, "Usuario");
		id = getUsuarioId(db);
	}
	while(permanecer){
		printf("Que desea modificar?\n1. Cantidad de un producto\n2. Añadir Producto\n3. Eliminar Producto\n");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%i", &opcion);
		printf("%i\n", opcion);
		visualizarCarrito(db, id);
		if(opcion == 1){
			printf("Cual desea Modificar?: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			sscanf(str, "%i", &idCar);
			printf("%i\n", idCar);
			cambiarCantidad(db, idCar);
		}else if(opcion == 2){
			anyadirProductoCarrito(db, &idCar, &total);
			permanecer = false;
		}else if(opcion == 3){
			printf("Cual desea Eliminar?: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			sscanf(str, "%i", &idCar);
			printf("%i\n", idCar);
			eliminarProductoCarrito(db, idCar);
			permanecer = false;
		}else{
			printError(db, 6, "Opcion");
		}
	}
}
void getProductoCarrito(sqlite3 *db, int *idProd, double *precio, double *precioAntiguo){
	sqlite3_stmt *stmt;
	int result;
	char str[MaxLine];
	char nomProd[MaxLine];
	printf("Introduce el nombre del producto: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	str[strcspn(str, "\n")] = '\0';
	strncpy(nomProd, str, MaxLine);
	printf("%s\n", nomProd);
	char sql1[] = "select P.id_producto, P.precio from PRODUCTO P where P.nombre = ?";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, nomProd, strlen(nomProd), SQLITE_STATIC);
	printf("\n");
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			*idProd = sqlite3_column_int(stmt, 0);
			*precio = sqlite3_column_double(stmt, 1);
			*precio = round(*precio * 100.0) / 100.0;
			*precioAntiguo = *precio;
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
}


