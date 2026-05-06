/*
 * gestionarProductos.h
 *
 *  Created on: 6 may 2026
 *      Author: oipac
 */

#ifndef GESTIONARPRODUCTOS_H_
#define GESTIONARPRODUCTOS_H_
#include "sqlite3.h"
#define MaxLine 50
void eliminarLineaCarritoProducto(sqlite3 *db, int idProd);
void anyadirProductos(sqlite3 *db);
void eliminarProductos(sqlite3 *db);
int getProducto(sqlite3 *db, char nomProd[MaxLine]);
void getAndPrintProducto(sqlite3 *db, int idProd);
void modificarProductos(sqlite3 *db);
void cambiarStock(sqlite3 *db, int idProd, int Stock);
void actualizarTotalProd(sqlite3 *db, int idProd, double precio, double precioAntiguo);
void cambiarPrecio(sqlite3 *db, int idProd, double precio);
void cambiarNombreProd(sqlite3 *db, int idProd, char nomProd[MaxLine]);
void cambiarDescripcionProd(sqlite3 *db, int idProd, char descripcion[MaxLine]);
void cambiarMarcaProd(sqlite3 *db, int idProd, char marca[MaxLine]);
void cambiarCategoriaProd(sqlite3 *db, int idProd, char nomCat[MaxLine]);
void cambiarProveedorProd(sqlite3 *db, int idProd, char nomProv[MaxLine]);


#endif /* GESTIONARPRODUCTOS_H_ */
