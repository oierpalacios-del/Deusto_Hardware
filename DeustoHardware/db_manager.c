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
void rellenar_base_datos(sqlite3 *db){
char *mensaje = 0;
const char *inserts =
		"INSERT INTO PAIS VALUES(1, 'ESPAÑA');"
		"INSERT INTO PAIS VALUES(2, 'FRANCIA');"
		"INSERT INTO PAIS VALUES(3, 'PORTUGAL');"
		"INSERT INTO PROVINCIA VALUES(1, 'ILE DE FRANCE', 2);"
		"INSERT INTO PROVINCIA VALUES(2, 'OPORTO', 3);"
		"INSERT INTO PROVINCIA VALUES(3, 'GALICIA', 1);"
		"INSERT INTO CIUDAD VALUES(1, 'PARIS', '37812', 1);"
		"INSERT INTO CIUDAD VALUES(2, 'OPORTO', '72732', 2);"
		"INSERT INTO CIUDAD VALUES(3, 'LUGO', '89274', 3);"
		"INSERT INTO CATEGORIA VALUES(1, 'PERIFERICOS', '');"
		"INSERT INTO CATEGORIA VALUES(2, 'CONSOLAS', '');"
		"INSERT INTO CATEGORIA VALUES(3, 'PC', '');"
		"INSERT INTO PROVEEDOR VALUES(1, 'MANOLO','974967789', 'manolo.uber@hardware.es', 'Av conchinchina', 1);"
		"INSERT INTO PROVEEDOR VALUES(2, 'JUAN','974967789', 'juan.uber@hardware.es', 'Av conchinchina', 2);"
		"INSERT INTO PROVEEDOR VALUES(3, 'PEPE','974967789', 'pepe.uber@hardware.es', 'Av conchinchina', 3);"
		"INSERT INTO USUARIO VALUES(1, 'MANOLO', 'GOMEZ PEREZ', 'manolo123@gmail.com', 'megust4nloscocos', 'CLIENTE', 1);"
		"INSERT INTO USUARIO VALUES(2, 'JORGE', 'RAMIREZ GARCIA', 'jaimito324@opendeusto.es', 'FOR3VERDEUSTO!', 'CLIENTE', 2);"
		"INSERT INTO USUARIO VALUES(3, 'ANTUAN', 'FERNANDEZ BERMEJO', 'antuan@gmail.com', 'olala', 'CLIENTE', 3);"
		"INSERT INTO PRODUCTO VALUES(1, 'RATON GAMER', '', 49.99, 13, 'LOGITECH', 1, 1);"
		"INSERT INTO PRODUCTO VALUES(2, 'NINTENDO WII', '', 499.99, 15, 'NINTENDO', 2, 2);"
		"INSERT INTO PRODUCTO VALUES(3, 'PC CON LEDS', '', 750.00, 3, 'AMD', 3, 3);";
	int resultado = sqlite3_exec(db, inserts, 0, 0, &mensaje);
	if(resultado != SQLITE_OK){
		printf("Error al insertar datos en la base de datos: %s\n", mensaje);
		sqlite3_free(mensaje);
	}else{
		printf("Inserts completados\n");
	}
}
