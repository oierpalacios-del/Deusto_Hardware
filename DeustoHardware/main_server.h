/*
 * main_server.h
 *
 *  Created on: 30 mar 2026
 *      Author: oipac
 */

#ifndef MAIN_SERVER_H_
#define MAIN_SERVER_H_

#include "sqlite3.h"

#define MAX_PETICION 4096
#define MAX_RESPUESTA 16384

int server(sqlite3 *db);
int procesarPeticion(sqlite3 *db, const char *peticion, char *respuesta, int tamRespuesta);

#endif
