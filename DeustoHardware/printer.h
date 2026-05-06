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
void printPedido(int idPed, char username[MaxLine], char fechaCreacion[MaxLine], char estado[MaxLine], double total);
void printError(sqlite3 *db, int tipoError, char tabla[MaxLine]);


#endif /* PRINTER_H_ */
