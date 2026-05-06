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
void eliminarLineaCarritoProducto(sqlite3 *db, int idProducto){
	char sql[] = "delete from LINEA_CARRITO where id_producto = ?";
	int result;
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idProducto);
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
	bool continuar = true;
	char str[MaxLine];
	char c;
	int *idCarrito = malloc(sizeof(int));
	double *total = (double*)malloc(sizeof(double));
	printf("Para añadir un Pedido tendrá que crear un carrito, y añadirle productos\n");
	printf("Esto es un proceso de 3 partes, se le avisará cuando complete cada una\n");
	printf("Por seguridad y para evitar redundancia, cada vez que haya un error, habrá que volver a empezar desde el principio\n");
	while(continuar){
		printf("Para empezar, tiene que crear un carrito\n");
		correcto = crearCarrito(db, idCarrito);
		if(correcto){
			printf("Fase 1/3 completado. Ahora tiene que añadirle productos al carrito\n");
			correcto = correcto && anyadirProductoCarrito(db, idCarrito, total);
		}else{
			printf("Error en la creación del carrito, regresando\n");
			continuar = false;
			free(idCarrito);
			free(total);
			gestionarPedidos(db);
		}
		if(correcto){
			printf("Fase 2/3 completado. Ahora se creara el pedido\n");
			correcto = correcto && crearPedido(db, idCarrito, total);
		}else {
			printf("Error en la creación de linea-carrito, eliminando carrito\n");
			continuar = false;
			eliminarCarrito(db, idCarrito);
			free(idCarrito);
			free(total);
			printf("Regresando\n");
			gestionarPedidos(db);
		}
		if(correcto){
			printf("Fase 3/3 completado. Pedido listo\n");
			printf("Desea continuar?\n");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			sscanf(str, "%c", &c);
			if(c == 'n' || c == 'N'){
				continuar = false;
				free(idCarrito);
				free(total);
			}
		}else{
			continuar = false;
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
void anyadirProductos(sqlite3 *db){
	sqlite3_stmt *stmt;
	char str[MaxLine];
	char nombre[MaxLine];
	char categoria[MaxLine];
	char proveedor[MaxLine];
	char descripcion[MaxLine];
	char marca[MaxLine];
	float precio;
	int stock;
	int result;
	int idProv = 0;
	int idCat = 0;
	printf("Por favor, introduzca el nombre del producto: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	str[strcspn(str, "\n")] = '\0';
	strncpy(nombre, str, MaxLine);
	printf("\n");
	printf("Por favor, introduzca el nombre del proveedor: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%s", proveedor);
	printf("\n");
	printf("Por favor, introduzca la descripcion del producto: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%s", descripcion);
	printf("\n");
	printf("Por favor, introduzca la marca del producto: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%s", marca);
	printf("\n");
	printf("Por favor, introduzca la categoria del producto: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%s", categoria);
	printf("\n");
	printf("Por favor, introduzca el stock del producto: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%d", &stock);
	printf("\n");
	printf("Por favor, introduzca el precio unitario del producto: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%f", &precio);
	printf("\n");
	char sql1[] = "select id_proveedor from PROVEEDOR where nombre = ?";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, proveedor, strlen(proveedor), SQLITE_STATIC);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			idProv = sqlite3_column_int(stmt, 0);
			printf("%d\n", sqlite3_column_int(stmt, 0));
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	if(idProv == 0){
		printError(db, 1, "Proveedor");
	}
	char sql2[] = "select id_categoria from CATEGORIA where nombre = ?";
	sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, categoria, strlen(categoria), SQLITE_STATIC);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			idCat = sqlite3_column_int(stmt, 0);
			printf("%d\n", sqlite3_column_int(stmt, 0));
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	if(idCat == 0){
		printError(db, 1, "Categoria");
	}
	char sql3[] = "insert or ignore into PRODUCTO (nombre, descripcion, precio, stock, marca, id_categoria, id_proveedor) values (?, ?, ?, ?, ?, ?, ?)";
	sqlite3_prepare_v2(db, sql3, strlen(sql3), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, nombre, strlen(nombre), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, descripcion, strlen(descripcion), SQLITE_STATIC);
	sqlite3_bind_double(stmt, 3, round((double)precio * 100) / 100);
	sqlite3_bind_int(stmt, 4, stock);
	sqlite3_bind_text(stmt, 5, marca, strlen(marca), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 6, idCat);
	sqlite3_bind_int(stmt, 7, idProv);
	result = sqlite3_step(stmt);
	if(result != SQLITE_DONE){
		printError(db, 0, "Producto");
	}else{
		printf("Producto insertado\n");
	}
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
void eliminarProductos(sqlite3 *db){
	int idProd;
	sqlite3_stmt *stmt;
	int result;
	char str[MaxLine];
	char c;
	visualizarProductos(db);
	printf("Cual desea eliminar?: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%i", &idProd);
	printf("%i\n", idProd);
	printf("Desea eliminar el producto %i?\n", idProd);
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	sscanf(str, "%c", &c);
	if(c == 's' || c == 'S'){
		eliminarLineaCarritoProducto(db, idProd);
		char sql3[] = "delete from PRODUCTO where id_producto = ?";
		sqlite3_prepare_v2(db, sql3, strlen(sql3), &stmt, NULL);
		sqlite3_bind_int(stmt, 1, idProd);
		result = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		if(result !=SQLITE_DONE){
			printError(db, 2, "Producto");
		}else{
			printf("Producto eliminado\n");
		}
	}
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
void getProducto(sqlite3 *db, int *idProd, double *precio, double *precioAntiguo){
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
void cambiarCantidad(sqlite3 *db, int idCar){
	char str[MaxLine];
	sqlite3_stmt *stmt;
	int result;
	double precioTemp;
	int *idProd = (int*)malloc(sizeof(int));
	double *precio = (double*)malloc(sizeof(double));
	double *precioAntiguo = (double*)malloc(sizeof(double));
	int cantidadProd;
	getProducto(db, idProd, precio, precioAntiguo);
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
		actualizarTotal(db, idCar, precio, precioAntiguo);
	}
	free(precioAntiguo);
	free(idProd);
	free(precio);
}
void actualizarTotal(sqlite3 *db, int idCar, double *precio, double *precioAntiguo){
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
	char sql2[] = "update PEDIDO SET total = ? where id_carrito = ?";
	sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
	sqlite3_bind_double(stmt, 1, total);
	sqlite3_bind_int(stmt, 2, idCar);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Pedido");
	}else{
		printf("Total actualizado\n");
	}
}
void eliminarProductoCarrito(sqlite3 *db, int idCar){
	sqlite3_stmt *stmt;
	int result;
	int *idProd = (int*)malloc(sizeof(int));
	double *precio = (double*)malloc(sizeof(double));
	double *precioAntiguo = (double*)malloc(sizeof(double));
	getProducto(db, idProd, precio, precioAntiguo);
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
		actualizarTotal(db, idCar, precio, precioAntiguo);
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
void modificarProductos(sqlite3 *db){
	printf("modificando\n");
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
		printf("Admin %s insertado\n", username);
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
			printf("Administrador %s registrado correctamente\n", username);
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
