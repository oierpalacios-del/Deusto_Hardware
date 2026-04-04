#include <stdio.h>
#include "db_manager.h"
#include "sqlite3.h"

void inicializar_base_datos(sqlite3 *db) {
    char *mensaje_error = 0;

    const char *sql_tablas =
        "CREATE TABLE IF NOT EXISTS PAIS ("
        "id_pais INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nombre TEXT NOT NULL"
        ");"

    		"CREATE TABLE IF NOT EXISTS PROVINCIA ("
    		"id_provincia INTEGER PRIMARY KEY AUTOINCREMENT, "
    		"nombre TEXT NOT NULL, "
    		"id_pais INTEGER"
    		");"

    		"CREATE TABLE IF NOT EXISTS CIUDAD ( "
    		"id_ciudad INTEGER PRIMARY KEY AUTOINCREMENT, "
    		"nombre TEXT NOT NULL, "
    		"codigo_postal TEXT, "
    		"id_provincia INTEGER "
    		");"

    		"CREATE TABLE IF NOT EXISTS CATEGORIA( "
    		"id_categoria INTEGER PRIMARY KEY AUTOINCREMENT, "
    		"nombre TEXT NOT NULL, "
    		"descripcion TEXT "
    		");"

    		"CREATE TABLE IF NOT EXISTS PROVEEDOR( "
    		"id_proveedor INTEGER PRIMARY KEY AUTOINCREMENT, "
    		"nombre TEXT NOT NULL, "
    		"telefono TEXT, "
    		"email TEXT UNIQUE, "
    		"direccion TEXT, "
    		"id_ciudad INTEGER "
    		");"

        "CREATE TABLE IF NOT EXISTS USUARIO ("
        "id_usuario INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nombre TEXT NOT NULL, "
        "apellidos TEXT, "
        "email TEXT UNIQUE, "
        "contrasena TEXT NOT NULL, "
        "rol TEXT, "
        "id_ciudad INTEGER "
        ");"

        "CREATE TABLE IF NOT EXISTS PRODUCTO ("
        "id_producto INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nombre TEXT NOT NULL, "
        "descripcion TEXT, "
        "precio REAL, "
        "stock INTEGER, "
        "marca TEXT, "
        "id_categoria INTEGER, "
        "id_proveedor INTEGER "
        ");";

    int resultado = sqlite3_exec(db, sql_tablas, 0, 0, &mensaje_error);

    if (resultado != SQLITE_OK) {
        printf("ERROR al inicializar las tablas: %s\n", mensaje_error);
        sqlite3_free(mensaje_error);
    } else {
        printf("OK: Base de datos estructurada y lista para operar.\n");
    }
}
