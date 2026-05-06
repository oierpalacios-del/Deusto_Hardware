/*
 * gestionarPedidos.h
 *
 *  Created on: 6 may 2026
 *      Author: oipac
 */

#ifndef GESTIONARPEDIDOS_H_
#define GESTIONARPEDIDOS_H_
#include "sqlite3.h"
#define MaxLine 50
#include <stdbool.h>
bool crearCarrito(sqlite3 *db, int *idCarrito);
bool anyadirProductoCarrito(sqlite3 *db, int *idCarrito, double *total);
bool crearPedido(sqlite3 *db, int *idCarrito, double *total);
void eliminarCarrito(sqlite3 *db, int *idCarrito);
void eliminarLineaCarrito(sqlite3 *db, int *idCarrito);
void anyadirPedidos(sqlite3 *db);
void eliminarPedidos(sqlite3 *db);
void cambiarCantidad(sqlite3 *db, int idCarrito);
void eliminarProductoCarrito(sqlite3 *db, int idCarrito);
void modificarPedidos(sqlite3 *db);
void getProductoCarrito(sqlite3 *db, int *idProd, double *precio, double *precioAntiguo);


#endif /* GESTIONARPEDIDOS_H_ */
