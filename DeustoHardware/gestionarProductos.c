/*
 * gestionarProductos.c
 *
 *  Created on: 6 may 2026
 *      Author: oipac
 */
#include "sqlite3.h"
#include <stdlib.h>
#include <stdio.h>
#define MaxLine 50
#include "gestionarProductos.h"
#include "gestionarPedidos.h"
#include "main_admin.h"
#include <string.h>
#include "printer.h"
#include <math.h>
void eliminarLineaCarritoProducto(sqlite3 *db, int idProducto){
	char sql1[] ="select L.id_carrito, L.preio_unitrio, P.total from PEDIO P, LINEA_CARRITO L where L.id_carrito = P.id_carrito and L.id_producto = ?";
	char sql2[] = "delete from LINEA_CARRITO where id_producto = ?";
	int result;
	int idCar;
	double total;
	double precio;
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idProducto);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			precio = sqlite3_column_double(stmt, 1);
			idCar = sqlite3_column_int(stmt, 0);
			total = sqlite3_column_double(stmt, 2);
			total = total - precio;
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	actualizarTotal(db, idCar, total);
	sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idProducto);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result !=SQLITE_DONE){
		printError(db, 2, "Linea-carrito");
	}else{
		printf("Linea-carrito eliminado\n");
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
void eliminarProductos(sqlite3 *db){
	int idProd;
	char nomProd[MaxLine];
	sqlite3_stmt *stmt;
	int result;
	char str[MaxLine];
	char c;
	visualizarProductos(db);
	printf("Introduce el nombre del Producto a modificar: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	str[strcspn(str, "\n")] = '\0';
	strncpy(nomProd, str, MaxLine);
	idProd = getProducto(db, nomProd);
	while(idProd == 0){
		visualizarProductos(db);
		printError(db, 1, "Producto");
		printf("Introduce el nombre del Producto a eliminar: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		str[strcspn(str, "\n")] = '\0';
		strncpy(nomProd, str, MaxLine);
		idProd = getProducto(db, nomProd);
	}
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
int getProducto(sqlite3 *db, char nomProd[MaxLine]){
	int result;
	int idProd = 0;
	sqlite3_stmt *stmt;
	char sql[] = "select id_producto from PRODUCTO where nombre = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, nomProd, strlen(nomProd), SQLITE_STATIC);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			idProd = sqlite3_column_int(stmt, 0);
			printf("%i\n", idProd);
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	return idProd;
}
void getAndPrintProducto(sqlite3 *db, int idProd){
	sqlite3_stmt *stmt;
	int result;
	int idProdTemp;
	int idProv;
	int idCat;
	int stock;
	double precio;
	char nomProv[MaxLine];
	char nomProd[MaxLine];
	char nomCat[MaxLine];
	char descripcion[MaxLine];
	char marca[MaxLine];
	char sql1[] = "select * from PRODUCTO where id_producto = ?";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idProd);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			idProdTemp = sqlite3_column_int(stmt, 0);
			strcpy(nomProd, (char*)sqlite3_column_text(stmt, 1));
			strcpy(descripcion, (char*)sqlite3_column_text(stmt, 2));
			strcpy(marca, (char*)sqlite3_column_text(stmt, 5));
			precio = sqlite3_column_double(stmt, 3);
			stock = sqlite3_column_int(stmt, 4);
			idCat = sqlite3_column_int(stmt, 6);
			idProv = sqlite3_column_int(stmt, 7);
		}
	}while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	char sql2[] = "select nombre from CATEGORIA where id_categoria = ?";
	sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idCat);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			strcpy(nomCat, (char*)sqlite3_column_text(stmt, 0));
		}
	}while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	char sql3[] = "select nombre from PROVEEDOR where id_proveedor = ?";
	sqlite3_prepare_v2(db, sql3, strlen(sql3), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idProv);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			strcpy(nomProv, (char*)sqlite3_column_text(stmt, 0));
		}
	}while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	printProductoCompleto(idProdTemp, nomProd, descripcion, marca, precio, stock, nomCat, nomProv);
}
void modificarProductos(sqlite3 *db){
	char str[MaxLine];
	int idProd;
	char opcion;
	int stock;
	float precio;
	char nuevoNomProd[MaxLine];
	char nomProd[MaxLine];
	char descripcion[MaxLine];
	char marca[MaxLine];
	char nomCat[MaxLine];
	char nomProv[MaxLine];
	bool permanecer = true;
	visualizarProductos(db);
	printf("Introduce el nombre del Producto a modificar: ");
	fflush(stdout);
	fgets(str, 50, stdin);
	clearLines(str, MaxLine);
	str[strcspn(str, "\n")] = '\0';
	strncpy(nomProd, str, MaxLine);
	idProd = getProducto(db, nomProd);
	while(idProd == 0){
		printError(db, 1, "Producto");
		visualizarProductos(db);
		printf("Introduce el nombre del Producto a modificar: ");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		str[strcspn(str, "\n")] = '\0';
		strncpy(nomProd, str, MaxLine);
		idProd = getProducto(db, nomProd);
	}
	while(permanecer){
		getAndPrintProducto(db, idProd);
		printf("Que desea modificar?\n1. Stock de un producto\n2. Precio de un producto\n3. Nombre de un producto\n4. Descripcion de un producto\n5. Marca de un producto\n6. Categoria de un Producto\n7. Proveedor del Producto\n");
		fflush(stdout);
		fgets(str, 50, stdin);
		clearLines(str, MaxLine);
		sscanf(str, "%c", &opcion);
		printf("%c\n", opcion);
		if(opcion == '1'){
			printf("Introduce el nuevo stock: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			sscanf(str, "%i", &stock);
			printf("%i\n", stock);
			cambiarStock(db, idProd, stock);
			permanecer = continuar();
		}else if(opcion == '2'){
			printf("Introduce el nuevo Precio: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			sscanf(str, "%f", &precio);
			printf("%.2f\n", precio);
			cambiarPrecio(db, idProd, (double)precio);
			permanecer = continuar();
		}else if(opcion == '3'){
			printf("Introduce el nuevo nombre del Producto: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			str[strcspn(str, "\n")] = '\0';
			strncpy(nuevoNomProd, str, MaxLine);
			printf("%s\n", nuevoNomProd);
			cambiarNombreProd(db, idProd, nuevoNomProd);
			permanecer = continuar();
		}else if(opcion == '4'){
			printf("Introduce la nueva descripcion del Producto: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			str[strcspn(str, "\n")] = '\0';
			strncpy(descripcion, str, MaxLine);
			printf("%s\n", descripcion);
			cambiarDescripcionProd(db, idProd, descripcion);
			permanecer = continuar();
		}else if(opcion == '5'){
			printf("Introduce la nueva marca del Producto: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			str[strcspn(str, "\n")] = '\0';
			strncpy(marca, str, MaxLine);
			printf("%s\n", marca);
			cambiarMarcaProd(db, idProd, marca);
			permanecer = continuar();
		}else if(opcion == '6'){
			printf("Introduce la categoria del Producto: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			str[strcspn(str, "\n")] = '\0';
			strncpy(nomCat, str, MaxLine);
			printf("%s\n", nomCat);
			cambiarCategoriaProd(db, idProd, nomCat);
			permanecer = continuar();
		}else if(opcion == '7'){
			visualizarProveedores(db);
			printf("Introduce el nuevo nombre del proveedor del Producto: ");
			fflush(stdout);
			fgets(str, 50, stdin);
			clearLines(str, MaxLine);
			str[strcspn(str, "\n")] = '\0';
			strncpy(nomProv, str, MaxLine);
			printf("%s\n", nomProv);
			cambiarProveedorProd(db, idProd, nomProv);
			permanecer = continuar();
		}else{
			printError(db, 6, "Opcion");
		}
	}
}
void cambiarStock(sqlite3 *db, int idProd, int stock){
	sqlite3_stmt *stmt;
	int result;
	char sql[] = "update PRODUCTO set stock = ? where id_producto = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, stock);
	sqlite3_bind_int(stmt, 2, idProd);
	result = sqlite3_step(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Producto");
	}else{
		printf("Producto actualizado\n");
	}
	sqlite3_finalize(stmt);
}
void getLineaProd(sqlite3 *db, int idProd, double precio, double precioAntiguo){
	int result;
	int cantidad;
	double total;
	int idCar = 0;
	int idCarAntiguo;
	sqlite3_stmt *stmt;
	char sql1[] = "select P.id_carrito, P.total, L.cantidad from LINEA_CARRITO L, PEDIDO P where L.id_producto = ? and P.id_carrito = L.id_carrito";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idProd);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			cantidad = sqlite3_column_int(stmt, 2);
			total = sqlite3_column_double(stmt, 1);
			idCarAntiguo = idCar;
			idCar = sqlite3_column_int(stmt, 0);
			total = round(total * 100.0) / 100.0;
			total = total - precioAntiguo*cantidad;
			total = total + precio*cantidad;
			printf("%.2f", total);
			if(idCarAntiguo != idCar){
				actualizarLineaProd(db, idProd, idCar, precio*cantidad, total);
			}
		}
	} while(result == SQLITE_ROW);
	sqlite3_finalize(stmt);
}
void actualizarLineaProd(sqlite3 *db, int idProd, int idCar, double precio, double total){
	int result;
	sqlite3_stmt *stmt;
	char sql[] = "update LINEA_CARRITO set precio_unitrio = ? where id_producto = ? and id_carrito = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_double(stmt, 1, precio);
	sqlite3_bind_int(stmt, 2, idProd);
	sqlite3_bind_int(stmt, 3, idCar);
	result = sqlite3_step(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Linea-carrito");
	}else{
		printf("Linea Carrito actualizado\n");
		actualizarTotal(db, idCar, total);
	}
}
void cambiarPrecio(sqlite3 *db, int idProd, double precio){
	sqlite3_stmt *stmt;
	int result;
	double precioAntiguo;
	char sql1[] = "select precio from PRODUCTO where id_producto = ?";
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, idProd);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			precioAntiguo = sqlite3_column_double(stmt, 0);
		}
	} while(result == SQLITE_ROW);
	getLineaProd(db, idProd, precio, precioAntiguo);
	sqlite3_finalize(stmt);
	char sql2[] = "update PRODUCTO set precio = ? where id_producto = ?";
	sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
	sqlite3_bind_double(stmt, 1, precio);
	sqlite3_bind_int(stmt, 2, idProd);
	result = sqlite3_step(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Producto");
	}else{
		printf("Producto actualizado\n");
	}
	sqlite3_finalize(stmt);
}
void cambiarNombreProd(sqlite3 *db, int idProd, char nomProd[MaxLine]){
	sqlite3_stmt *stmt;
	int result;
	char sql[] = "update PRODUCTO set nombre = ? where id_producto = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, nomProd, strlen(nomProd), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 2, idProd);
	result = sqlite3_step(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Producto");
	}else{
		printf("Producto actualizado\n");
	}
	sqlite3_finalize(stmt);
}
void cambiarDescripcionProd(sqlite3 *db, int idProd, char descripcion[MaxLine]){
	sqlite3_stmt *stmt;
	int result;
	char sql[] = "update PRODUCTO set descripcion = ? where id_producto = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, descripcion, strlen(descripcion), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 2, idProd);
	result = sqlite3_step(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Producto");
	}else{
		printf("Producto actualizado\n");
	}
	sqlite3_finalize(stmt);
}
void cambiarMarcaProd(sqlite3 *db, int idProd, char marca[MaxLine]){
	sqlite3_stmt *stmt;
	int result;
	char sql[] = "update PRODUCTO set marca = ? where id_producto = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, marca, strlen(marca), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 2, idProd);
	result = sqlite3_step(stmt);
	if(result != SQLITE_DONE){
		printError(db, 3, "Producto");
	}else{
		printf("Producto actualizado\n");
	}
	sqlite3_finalize(stmt);
}
void cambiarCategoriaProd(sqlite3 *db, int idProd, char nomCat[MaxLine]){
	int result;
	int idCat = 0;
	char sql1[] = "select id_categoria from CATEGORIA where nombre = ?";
	char sql2[] = "update PRODUCTO set id_categoria = ? where id_producto = ?";
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, nomCat, strlen(nomCat), SQLITE_STATIC);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			idCat = sqlite3_column_int(stmt, 0);
		}
	} while (result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	if(idCat == 0){
		printError(db, 1, "Categoria");
	}else{
		sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
		sqlite3_bind_int(stmt, 1, idCat);
		sqlite3_bind_int(stmt, 2, idProd);
		result = sqlite3_step(stmt);
		if(result != SQLITE_DONE){
			printError(db, 3, "Producto");
		}else{
			printf("Producto actualizado\n");
		}
	}
}
void cambiarProveedorProd(sqlite3 *db, int idProd, char nomProv[MaxLine]){
	int result;
	int idProv = 0;
	char sql1[] = "select id_proveedor from PROVEEDOR where nombre = ?";
	char sql2[] = "update PRODUCTO set id_proveedor = ? where id_producto = ?";
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql1, strlen(sql1), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, nomProv, strlen(nomProv), SQLITE_STATIC);
	do{
		result = sqlite3_step(stmt);
		if(result == SQLITE_ROW){
			idProv = sqlite3_column_int(stmt, 0);
		}
	} while (result == SQLITE_ROW);
	sqlite3_finalize(stmt);
	if(idProv == 0){
		printError(db, 1, "Proveedor");
	}else{
		sqlite3_prepare_v2(db, sql2, strlen(sql2), &stmt, NULL);
		sqlite3_bind_int(stmt, 1, idProv);
		sqlite3_bind_int(stmt, 2, idProd);
		result = sqlite3_step(stmt);
		if(result != SQLITE_DONE){
			printError(db, 3, "Producto");
		}else{
			printf("Producto actualizado\n");
		}
	}
}



