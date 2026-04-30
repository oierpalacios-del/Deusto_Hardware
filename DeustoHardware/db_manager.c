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
    		"id_pais INTEGER,"
    		"FOREIGN KEY (id_pais) REFERENCES PAIS (id_pais)"
    		");"

    		"CREATE TABLE IF NOT EXISTS CIUDAD ( "
    		"id_ciudad INTEGER PRIMARY KEY AUTOINCREMENT, "
    		"nombre TEXT NOT NULL, "
    		"codigo_postal TEXT, "
    		"id_provincia INTEGER,"
    		"FOREIGN KEY (id_provincia) REFERENCES PROVINCIA (id_provincia)"
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
    		"id_ciudad INTEGER,"
    		"FOREIGN KEY (id_ciudad) REFERENCES CIUDAD (id_ciudad)"
    		");"

        "CREATE TABLE IF NOT EXISTS USUARIO ("
        "id_usuario INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nombre TEXT NOT NULL, "
        "apellidos TEXT, "
        "email TEXT UNIQUE, "
        "contrasena TEXT NOT NULL, "
        "rol TEXT, "
        "id_ciudad INTEGER,"
        "FOREIGN KEY (id_ciudad) REFERENCES CIUDAD (id_ciudad)"
        ");"

        "CREATE TABLE IF NOT EXISTS PRODUCTO ("
        "id_producto INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nombre TEXT NOT NULL, "
        "descripcion TEXT, "
        "precio REAL, "
        "stock INTEGER, "
        "marca TEXT, "
        "id_categoria INTEGER, "
        "id_proveedor INTEGER,"
        "FOREIGN KEY (id_categoria) REFERENCES CATEGORIA(id_categoria),"
        "FOREIGN KEY (id_proveedor) REFERENCES PROVEEDOR(id_proveedor)"
        ");"

        "CREATE TABLE IF NOT EXISTS CARRITO ("
        "id_carrito INTEGER PRIMARY KEY AUTOINCREMENT,"
        "fecha_creacion DATE NOT NULL,"
        "estado_compra TEXT,"
        "id_usuario INTEGER,"
        "FOREIGN KEY (id_usuario) REFERENCES USUARIO(id_usuario));"

        "CREATE TABLE IF NOT EXISTS LINEA_CARRITO ("
        "id_carrito INTEGER,"
        "id_producto INTEGER,"
        "cantidad INTEGER,"
        "precio_unitrio REAL,"
		"PRIMARY KEY (id_carrito, id_producto),"
        "FOREIGN KEY (id_carrito) REFERENCES CARRITO(id_carrito),"
        "FOREIGN KEY (id_producto) REFERENCES PRODUCTO(id_producto));"

    	"CREATE TABLE IF NOT EXISTS PEDIDO ("
    	"id_pedido INTEGER PRIMARY KEY AUTOINCREMENT,"
    	"fecha_envio DATE,"
    	"total REAL,"
    	"id_carrito INTEGER,"
    	"FOREIGN KEY (id_carrito) REFERENCES CARRITO(id_carrito));"

    	"CREATE TABLE IF NOT EXISTS RESEÑA ("
    	"id_reseña INTEGER PRIMARY KEY AUTOINCREMENT,"
    	"id_usuario INTEGER,"
    	"puntuacion REAL,"
    	"comentario TEXT,"
    	"fecha DATE,"
    	"FOREIGN KEY (id_usuario) REFERENCES USUARIO(id_usuario));";

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
		"INSERT INTO PAIS (nombre) VALUES('ESPAÑA');"
		"INSERT INTO PAIS (nombre) VALUES('FRANCIA');"
		"INSERT INTO PAIS (nombre) VALUES('PORTUGAL');"
		"INSERT INTO PROVINCIA (nombre, id_pais) VALUES('ILE DE FRANCE', 2);"
		"INSERT INTO PROVINCIA (nombre, id_pais) VALUES('OPORTO', 3);"
		"INSERT INTO PROVINCIA (nombre, id_pais) VALUES('GALICIA', 1);"
		"INSERT INTO CIUDAD (nombre, codigo_postal, id_provincia) VALUES('PARIS', '37812', 1);"
		"INSERT INTO CIUDAD (nombre, codigo_postal, id_provincia) VALUES('OPORTO', '72732', 2);"
		"INSERT INTO CIUDAD (nombre, codigo_postal, id_provincia) VALUES('LUGO', '89274', 3);"
		"INSERT INTO CATEGORIA (nombre, descripcion) VALUES('PERIFERICOS', '');"
		"INSERT INTO CATEGORIA (nombre, descripcion) VALUES('CONSOLAS', '');"
		"INSERT INTO CATEGORIA (nombre, descripcion) VALUES('PC', '');"
		"INSERT INTO PROVEEDOR (nombre, telefono, email, direccion, id_ciudad) VALUES('MANOLO', '974967799', 'manolo.uber@hardware.es', 'Av conchinchina', 1);"
		"INSERT INTO PROVEEDOR (nombre, telefono, email, direccion, id_ciudad) VALUES('JUAN', '974967779', 'juan.uber@hardware.es', 'Av conchinchina', 2);"
		"INSERT INTO PROVEEDOR (nombre, telefono, email, direccion, id_ciudad) VALUES('PEPE', '974967789', 'pepe.uber@hardware.es', 'Av conchinchina', 3);"
		"INSERT INTO USUARIO (nombre, apellidos, email, contrasena, rol, id_ciudad) VALUES('MANOLO', 'GOMEZ PEREZ', 'manolo123@gmail.com', 'megust4nloscocos', 'CLIENTE', 1);"
		"INSERT INTO USUARIO (nombre, apellidos, email, contrasena, rol, id_ciudad) VALUES('JORGE', 'RAMIREZ GARCIA', 'jaimito324@opendeusto.es', 'FOR3VERDEUSTO!', 'CLIENTE', 2);"
		"INSERT INTO USUARIO (nombre, apellidos, email, contrasena, rol, id_ciudad) VALUES('ANTUAN', 'FERNANDEZ BERMEJO', 'antuan@gmail.com', 'olala', 'CLIENTE', 3);"
		"INSERT INTO PRODUCTO (nombre, descripcion, precio, stock, marca, id_categoria, id_proveedor) VALUES('RATON GAMER', '', 49.99, 13, 'LOGITECH', 1, 1);"
		"INSERT INTO PRODUCTO (nombre, descripcion, precio, stock, marca, id_categoria, id_proveedor) VALUES('NINTENDO WII', '', 499.99, 15, 'NINTENDO', 2, 2);"
		"INSERT INTO PRODUCTO (nombre, descripcion, precio, stock, marca, id_categoria, id_proveedor) VALUES('PC CON LEDS', '', 750.00, 3, 'AMD', 3, 3);";
	int resultado = sqlite3_exec(db, inserts, 0, 0, &mensaje);
	if(resultado != SQLITE_OK){
		printf("Error al insertar datos en la base de datos: %s\n", mensaje);
		sqlite3_free(mensaje);
	}else{
		printf("Inserts completados\n");
	}
}
