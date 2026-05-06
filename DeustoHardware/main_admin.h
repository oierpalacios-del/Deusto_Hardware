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
void clearLines(char *str, int maxLine);
void menu(sqlite3 *db);
void importarFichero(sqlite3 *db);
void exportarFichero(sqlite3 *db);
void gestionarPedidos(sqlite3 *db);
void gestionarProductos(sqlite3 *db);
void visualizarPedidos(sqlite3 *db);
void visualizarProveedores(sqlite3 *db);
void visualizarProductos(sqlite3 *db);
void visualizarCarrito(sqlite3 *db, int id);
int getUsuarioId(sqlite3 *db);
void getTotal(sqlite3 *db, int idCar, double *precio, double *precioAntiguo);
void actualizarTotal(sqlite3 *db, int idCar, double total);
bool continuar();
bool comprobarUsuario(sqlite3 *db, char username[MaxLine], char contrasenya[MaxLine]);
void iniciarSesion(sqlite3 *db);
bool registrarAdminDB(sqlite3 *db, char username[MaxLine], char apellido[MaxLine], int telefono, char email[MaxLine], char ciudad[MaxLine], char contrasenya[MaxLine]);
void registrarAdmin(sqlite3 *db);
void inicio(sqlite3 *db);

#endif /* MAIN_ADMIN_H_ */
