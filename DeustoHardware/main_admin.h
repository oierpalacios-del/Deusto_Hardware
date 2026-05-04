/*
 * main_admin.h
 *
 *  Created on: 30 mar 2026
 *      Author: oipac
 */

#ifndef MAIN_ADMIN_H_
#define MAIN_ADMIN_H_
#include <stdbool.h>
#include "sqlite3.h"
#define MaxLine 50
void inicio(sqlite3 *db);
void menu(sqlite3 *db);
void importarFichero(sqlite3 *db);
void registrarAdmin(sqlite3 *db);
void gestionarProductos(sqlite3 *db);
void gestionarPedidos(sqlite3 *db);
bool comprobarUsuario(sqlite3 *db, char username[MaxLine], char contrasenya[MaxLine]);
bool registrarAdminDB(sqlite3 *db, char username[MaxLine], char apellido[MaxLine], int telefono, char email[MaxLine], char ciudad[MaxLine], char contrasenya[MaxLine]);
void modificarProductos(sqlite3 *db);
void modificarPedidos(sqlite3 *db);
void anyadirPedidos(sqlite3 *db);
bool crearCarrito(sqlite3 *db, int *idCarrito);
bool anyadirProductoCarrito(sqlite3 *db, int *idCarrito, double *total);
bool crearPedido(sqlite3 *db, int *idCarrito, double *total);
void eliminarCarrito(sqlite3 *db, int *idCarrito);
void eliminarLineaCarrito(sqlite3 *db, int *idCarrito);
void anyadirProductos(sqlite3 *db);
void visualizarPedidos(sqlite3 *db);
void visualizarProductos(sqlite3 *db);
void eliminarProductos(sqlite3 *db);
void eliminarPedidos(sqlite3 *db);
void clearLines(char *str, int maxLine);

#endif /* MAIN_ADMIN_H_ */
