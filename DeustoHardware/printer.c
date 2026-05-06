/*
 * printer.c
 *
 *  Created on: 5 may 2026
 *      Author: oipac
 */

#include <stdio.h>
#include "sqlite3.h"
#include "string.h"
#define MaxLine 50
void printProducto(int idProd, char nomProd[MaxLine], char marca[MaxLine], double precio, int stock, char nomCat[MaxLine]){
	printf("Producto: Id:%i, Nombre: %s, Marca: %s, Precio Unidad: %.2f, Stock: %i, Categoria: %s\n", idProd, nomProd, marca, precio, stock, nomCat);
}
void printPedido(int idPed, char username[MaxLine], char fechaCreacion[MaxLine], char estado[MaxLine], double total){
	printf("Pedido: Id: %i, Usuario comprador: %s, Fecha de Compra: %s, Estado del Pedido: %s, Total: %.2f\n", idPed, username, fechaCreacion, estado, total);
}
void printError(sqlite3 *db, int tipoError, char tabla[MaxLine]){
	if(strcmp(sqlite3_errmsg(db), "not an error") != 0){
		printf(sqlite3_errmsg(db));
		printf("\n");
	}
	switch (tipoError) {
		case 0:
			printf("Error 0: %s no insertado\n", tabla);
			break;
		case 1:
			printf("Error 1: %s no encontrado\n", tabla);
			break;
		case 2:
			printf("Error 2: %s no fue eliminado\n", tabla);
			break;
		case 3:
			printf("Error 3: %s no se pudo actualizar\n", tabla);
			break;
		case 4:
			printf("Error 4: El usuario o la contraseña está mas escrito\n");
			break;
		case 5:
			printf("Error 5: El numero de telefono tiene caracteres incompatibles o es demasiado grande o pequeño\n");
			break;
		case 6:
			printf("Error 6: La %s no es válida\n", tabla);
			break;
		default:
			break;
	}
}
