/*
 * printer.h
 *
 *  Created on: 5 may 2026
 *      Author: oipac
 */

#ifndef PRINTER_H_
#define PRINTER_H_
#define MaxLine 50
#include "sqlite3.h"
void printProducto(int idProd, char nomProd[MaxLine], char marca[MaxLine], double precio, int stock, char nomCat[MaxLine]);
void printProductoCompleto(int idProd, char nomProd[MaxLine], char descripcion[MaxLine],char marca[MaxLine], double precio, int stock, char nomCat[MaxLine], char nomProv[MaxLine]);
void printPedido(int idPed, char username[MaxLine], char fechaCreacion[MaxLine], char estado[MaxLine], double total);
void printError(sqlite3 *db, int tipoError, char tabla[MaxLine]);
void printProveedor(int idProv, char nombre[MaxLine], char telefono[MaxLine], char email[MaxLine], char direccion[MaxLine], int idCiudad);
void printSucces(int tipoExito, char tabla[MaxLine]);

#endif /* PRINTER_H_ */
