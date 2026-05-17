#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "main_server.h"

#define MAX_CAMPOS 20
#define TAM_CAMPO 256

static void quitar_salto_linea(char *cadena) {
    if (cadena != NULL) {
        cadena[strcspn(cadena, "\r\n")] = '\0';
    }
}

static void responder(char *respuesta, int tamRespuesta, const char *estado, const char *mensaje) {
    snprintf(respuesta, tamRespuesta, "%s|%s", estado, mensaje);
}

static void responder_bd_error(sqlite3 *db, char *respuesta, int tamRespuesta) {
    snprintf(respuesta, tamRespuesta, "ERR|Error de base de datos: %s", sqlite3_errmsg(db));
}

static int separar_campos(const char *peticion, char campos[MAX_CAMPOS][TAM_CAMPO]) {
    char copia[MAX_PETICION];
    char *token;
    int numCampos = 0;

    if (peticion == NULL || strlen(peticion) == 0) {
        return 0;
    }

    strncpy(copia, peticion, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    quitar_salto_linea(copia);

    token = strtok(copia, "|");

    while (token != NULL && numCampos < MAX_CAMPOS) {
        strncpy(campos[numCampos], token, TAM_CAMPO - 1);
        campos[numCampos][TAM_CAMPO - 1] = '\0';

        numCampos++;
        token = strtok(NULL, "|");
    }

    return numCampos;
}

static int append_text(char *destino, int tamDestino, const char *texto) {
    int usado = (int)strlen(destino);
    int libre = tamDestino - usado - 1;

    if (libre <= 0) {
        return 0;
    }

    strncat(destino, texto, libre);
    return 1;
}

static int existe_id(sqlite3 *db, const char *tabla, const char *columnaId, int id) {
    sqlite3_stmt *stmt = NULL;
    char sql[200];
    int existe = 0;

    snprintf(sql, sizeof(sql), "SELECT 1 FROM %s WHERE %s = ?", tabla, columnaId);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        existe = 1;
    }

    sqlite3_finalize(stmt);
    return existe;
}

static int obtener_id_ciudad(sqlite3 *db, const char *nombreCiudad) {
    sqlite3_stmt *stmt = NULL;
    int idCiudad = 1;
    const char *sql = "SELECT id_ciudad FROM CIUDAD WHERE UPPER(nombre) = UPPER(?) LIMIT 1";

    if (nombreCiudad == NULL || strlen(nombreCiudad) == 0) {
        return idCiudad;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return idCiudad;
    }

    sqlite3_bind_text(stmt, 1, nombreCiudad, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        idCiudad = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return idCiudad;
}

static void comando_login(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                          char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT id_usuario, rol FROM USUARIO WHERE email = ? AND contrasena = ?";

    if (numCampos != 3) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 01|email|contrasena");
        return;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_text(stmt, 1, campos[1], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, campos[2], -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int idUsuario = sqlite3_column_int(stmt, 0);
        const unsigned char *rol = sqlite3_column_text(stmt, 1);

        snprintf(respuesta, tamRespuesta, "OK|Login correcto|%s|%d",
                 rol ? (const char *)rol : "CLIENTE",
                 idUsuario);
    } else {
        responder(respuesta, tamRespuesta, "ERR", "Usuario o contrasena incorrectos");
    }

    sqlite3_finalize(stmt);
}

static void comando_registrar_admin(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                    char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;

    const char *sql =
        "INSERT INTO USUARIO (nombre, apellidos, email, contrasena, rol, id_ciudad) "
        "VALUES (?, ?, ?, ?, 'ADMIN', 1)";

    if (numCampos != 5) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 02|nombre|apellidos|email|contrasena");
        return;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_text(stmt, 1, campos[1], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, campos[2], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, campos[3], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, campos[4], -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        responder(respuesta, tamRespuesta, "OK", "Administrador registrado correctamente");
    } else {
        responder(respuesta, tamRespuesta, "ERR", "No se pudo registrar el administrador");
    }

    sqlite3_finalize(stmt);
}

static void comando_registrar_cliente(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                      char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;
    int idCiudad;

    const char *sql =
        "INSERT INTO USUARIO (nombre, apellidos, email, contrasena, rol, id_ciudad) "
        "VALUES (?, ?, ?, ?, 'CLIENTE', ?)";

    if (numCampos != 8) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 03|nombre|apellidos|email|contrasena|telefono|direccion|ciudad");
        return;
    }

    idCiudad = obtener_id_ciudad(db, campos[7]);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_text(stmt, 1, campos[1], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, campos[2], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, campos[3], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, campos[4], -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, idCiudad);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        responder(respuesta, tamRespuesta, "OK", "Cliente registrado correctamente");
    } else {
        responder(respuesta, tamRespuesta, "ERR", "No se pudo registrar el cliente");
    }

    sqlite3_finalize(stmt);
}

static void comando_ver_catalogo(sqlite3 *db, char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;
    int primero = 1;

    const char *sql =
        "SELECT P.id_producto, P.nombre, P.marca, P.precio, P.stock, COALESCE(C.nombre, '') "
        "FROM PRODUCTO P "
        "LEFT JOIN CATEGORIA C ON P.id_categoria = C.id_categoria "
        "ORDER BY P.id_producto";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    strcpy(respuesta, "OK|");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char producto[512];

        snprintf(producto, sizeof(producto), "%s%d;%s;%s;%.2f;%d;%s",
                 primero ? "" : "#",
                 sqlite3_column_int(stmt, 0),
                 sqlite3_column_text(stmt, 1) ? (const char *)sqlite3_column_text(stmt, 1) : "",
                 sqlite3_column_text(stmt, 2) ? (const char *)sqlite3_column_text(stmt, 2) : "",
                 sqlite3_column_double(stmt, 3),
                 sqlite3_column_int(stmt, 4),
                 sqlite3_column_text(stmt, 5) ? (const char *)sqlite3_column_text(stmt, 5) : "");

        append_text(respuesta, tamRespuesta, producto);
        primero = 0;
    }

    sqlite3_finalize(stmt);

    if (primero) {
        responder(respuesta, tamRespuesta, "ERR", "No hay productos disponibles");
    }
}

static int obtener_precio_stock(sqlite3 *db, int idProducto, double *precio, int *stock) {
    sqlite3_stmt *stmt = NULL;
    int encontrado = 0;
    const char *sql = "SELECT precio, stock FROM PRODUCTO WHERE id_producto = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_int(stmt, 1, idProducto);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *precio = sqlite3_column_double(stmt, 0);
        *stock = sqlite3_column_int(stmt, 1);
        encontrado = 1;
    }

    sqlite3_finalize(stmt);
    return encontrado;
}

static void comando_confirmar_compra(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                     char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;
    char carrito[TAM_CAMPO];
    char *linea;
    int idUsuario;
    int idCarrito = 0;
    int idPedido = 0;
    double total = 0.0;
    char mensaje[256];

    if (numCampos != 3) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 06|id_usuario|id_producto,cantidad#id_producto,cantidad");
        return;
    }

    idUsuario = atoi(campos[1]);

    if (!existe_id(db, "USUARIO", "id_usuario", idUsuario)) {
        responder(respuesta, tamRespuesta, "ERR", "El usuario no existe");
        return;
    }

    strncpy(carrito, campos[2], sizeof(carrito) - 1);
    carrito[sizeof(carrito) - 1] = '\0';

    linea = strtok(carrito, "#");

    while (linea != NULL) {
        int idProducto = 0;
        int cantidad = 0;
        int stock = 0;
        double precio = 0.0;

        if (sscanf(linea, "%d,%d", &idProducto, &cantidad) != 2 || cantidad <= 0) {
            responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto en el carrito");
            return;
        }

        if (!obtener_precio_stock(db, idProducto, &precio, &stock)) {
            snprintf(mensaje, sizeof(mensaje), "El producto %d no existe", idProducto);
            responder(respuesta, tamRespuesta, "ERR", mensaje);
            return;
        }

        if (stock < cantidad) {
            snprintf(mensaje, sizeof(mensaje), "Stock insuficiente para el producto %d", idProducto);
            responder(respuesta, tamRespuesta, "ERR", mensaje);
            return;
        }

        total += precio * cantidad;
        linea = strtok(NULL, "#");
    }

    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    if (sqlite3_prepare_v2(db,
        "INSERT INTO CARRITO (fecha_creacion, estado_compra, id_usuario) "
        "VALUES (DATE('now'), 'CONFIRMADO', ?)",
        -1, &stmt, NULL) != SQLITE_OK) {

        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_finalize(stmt);
    stmt = NULL;

    idCarrito = (int)sqlite3_last_insert_rowid(db);

    strncpy(carrito, campos[2], sizeof(carrito) - 1);
    carrito[sizeof(carrito) - 1] = '\0';

    linea = strtok(carrito, "#");

    while (linea != NULL) {
        int idProducto = 0;
        int cantidad = 0;
        int stock = 0;
        double precio = 0.0;

        sscanf(linea, "%d,%d", &idProducto, &cantidad);
        obtener_precio_stock(db, idProducto, &precio, &stock);

        if (sqlite3_prepare_v2(db,
            "INSERT INTO LINEA_CARRITO (id_carrito, id_producto, cantidad, precio_unitrio) "
            "VALUES (?, ?, ?, ?)",
            -1, &stmt, NULL) != SQLITE_OK) {

            sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
            responder_bd_error(db, respuesta, tamRespuesta);
            return;
        }

        sqlite3_bind_int(stmt, 1, idCarrito);
        sqlite3_bind_int(stmt, 2, idProducto);
        sqlite3_bind_int(stmt, 3, cantidad);
        sqlite3_bind_double(stmt, 4, precio);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
            responder_bd_error(db, respuesta, tamRespuesta);
            return;
        }

        sqlite3_finalize(stmt);
        stmt = NULL;

        if (sqlite3_prepare_v2(db,
            "UPDATE PRODUCTO SET stock = stock - ? WHERE id_producto = ?",
            -1, &stmt, NULL) != SQLITE_OK) {

            sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
            responder_bd_error(db, respuesta, tamRespuesta);
            return;
        }

        sqlite3_bind_int(stmt, 1, cantidad);
        sqlite3_bind_int(stmt, 2, idProducto);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
            responder_bd_error(db, respuesta, tamRespuesta);
            return;
        }

        sqlite3_finalize(stmt);
        stmt = NULL;

        linea = strtok(NULL, "#");
    }

    if (sqlite3_prepare_v2(db,
        "INSERT INTO PEDIDO (fecha_envio, total, id_carrito) "
        "VALUES (DATE('now'), ?, ?)",
        -1, &stmt, NULL) != SQLITE_OK) {

        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_double(stmt, 1, total);
    sqlite3_bind_int(stmt, 2, idCarrito);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_finalize(stmt);

    idPedido = (int)sqlite3_last_insert_rowid(db);

    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);

    snprintf(respuesta, tamRespuesta, "OK|Compra realizada correctamente|%d|%.2f", idPedido, total);
}

static void comando_modificar_pedido(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                     char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;

    const char *sql =
        "UPDATE CARRITO SET estado_compra = ? "
        "WHERE id_carrito = (SELECT id_carrito FROM PEDIDO WHERE id_pedido = ?)";

    if (numCampos != 3) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 07|id_pedido|nuevo_estado");
        return;
    }

    if (!existe_id(db, "PEDIDO", "id_pedido", atoi(campos[1]))) {
        responder(respuesta, tamRespuesta, "ERR", "El pedido no existe");
        return;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_text(stmt, 1, campos[2], -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, atoi(campos[1]));

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        responder(respuesta, tamRespuesta, "OK", "Pedido modificado correctamente");
    } else {
        responder_bd_error(db, respuesta, tamRespuesta);
    }

    sqlite3_finalize(stmt);
}

static void comando_eliminar_pedido(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                    char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;
    int idPedido;
    int idCarrito = 0;

    if (numCampos != 2) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 08|id_pedido");
        return;
    }

    idPedido = atoi(campos[1]);

    if (!existe_id(db, "PEDIDO", "id_pedido", idPedido)) {
        responder(respuesta, tamRespuesta, "ERR", "El pedido no existe");
        return;
    }

    if (sqlite3_prepare_v2(db,
        "SELECT id_carrito FROM PEDIDO WHERE id_pedido = ?",
        -1, &stmt, NULL) == SQLITE_OK) {

        sqlite3_bind_int(stmt, 1, idPedido);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            idCarrito = sqlite3_column_int(stmt, 0);
        }
    }

    if (stmt != NULL) {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    if (sqlite3_prepare_v2(db,
        "DELETE FROM PEDIDO WHERE id_pedido = ?",
        -1, &stmt, NULL) != SQLITE_OK) {

        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_int(stmt, 1, idPedido);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_finalize(stmt);
    stmt = NULL;

    if (idCarrito > 0) {
        if (sqlite3_prepare_v2(db,
            "DELETE FROM LINEA_CARRITO WHERE id_carrito = ?",
            -1, &stmt, NULL) == SQLITE_OK) {

            sqlite3_bind_int(stmt, 1, idCarrito);
            sqlite3_step(stmt);
        }

        if (stmt != NULL) {
            sqlite3_finalize(stmt);
            stmt = NULL;
        }

        if (sqlite3_prepare_v2(db,
            "DELETE FROM CARRITO WHERE id_carrito = ?",
            -1, &stmt, NULL) == SQLITE_OK) {

            sqlite3_bind_int(stmt, 1, idCarrito);
            sqlite3_step(stmt);
        }

        if (stmt != NULL) {
            sqlite3_finalize(stmt);
            stmt = NULL;
        }
    }

    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);

    responder(respuesta, tamRespuesta, "OK", "Pedido eliminado correctamente");
}

static void comando_ver_pedidos_usuario(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                        char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;
    int primero = 1;

    const char *sql =
        "SELECT P.id_pedido, P.fecha_envio, COALESCE(C.estado_compra, ''), P.total "
        "FROM PEDIDO P "
        "INNER JOIN CARRITO C ON P.id_carrito = C.id_carrito "
        "WHERE C.id_usuario = ? "
        "ORDER BY P.id_pedido";

    if (numCampos != 2) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 09|id_usuario");
        return;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_int(stmt, 1, atoi(campos[1]));

    strcpy(respuesta, "OK|");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char pedido[256];

        snprintf(pedido, sizeof(pedido), "%s%d;%s;%s;%.2f",
                 primero ? "" : "#",
                 sqlite3_column_int(stmt, 0),
                 sqlite3_column_text(stmt, 1) ? (const char *)sqlite3_column_text(stmt, 1) : "",
                 sqlite3_column_text(stmt, 2) ? (const char *)sqlite3_column_text(stmt, 2) : "",
                 sqlite3_column_double(stmt, 3));

        append_text(respuesta, tamRespuesta, pedido);
        primero = 0;
    }

    sqlite3_finalize(stmt);

    if (primero) {
        responder(respuesta, tamRespuesta, "ERR", "El usuario no tiene pedidos");
    }
}

static void comando_anyadir_producto(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                     char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;

    const char *sql =
        "INSERT INTO PRODUCTO (nombre, descripcion, precio, stock, marca, id_categoria, id_proveedor) "
        "VALUES (?, ?, ?, ?, ?, ?, 1)";

    if (numCampos != 7) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 10|nombre|descripcion|precio|stock|marca|id_categoria");
        return;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_text(stmt, 1, campos[1], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, campos[2], -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, atof(campos[3]));
    sqlite3_bind_int(stmt, 4, atoi(campos[4]));
    sqlite3_bind_text(stmt, 5, campos[5], -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, atoi(campos[6]));

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        responder(respuesta, tamRespuesta, "OK", "Producto anadido correctamente");
    } else {
        responder_bd_error(db, respuesta, tamRespuesta);
    }

    sqlite3_finalize(stmt);
}

static void comando_modificar_producto(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                       char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;
    int idProducto;

    const char *sql =
        "UPDATE PRODUCTO "
        "SET nombre = ?, descripcion = ?, precio = ?, stock = ?, marca = ?, id_categoria = ? "
        "WHERE id_producto = ?";

    if (numCampos != 8) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 11|id_producto|nombre|descripcion|precio|stock|marca|id_categoria");
        return;
    }

    idProducto = atoi(campos[1]);

    if (!existe_id(db, "PRODUCTO", "id_producto", idProducto)) {
        responder(respuesta, tamRespuesta, "ERR", "El producto no existe");
        return;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_text(stmt, 1, campos[2], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, campos[3], -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, atof(campos[4]));
    sqlite3_bind_int(stmt, 4, atoi(campos[5]));
    sqlite3_bind_text(stmt, 5, campos[6], -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, atoi(campos[7]));
    sqlite3_bind_int(stmt, 7, idProducto);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        responder(respuesta, tamRespuesta, "OK", "Producto modificado correctamente");
    } else {
        responder_bd_error(db, respuesta, tamRespuesta);
    }

    sqlite3_finalize(stmt);
}

static void comando_eliminar_producto(sqlite3 *db, char campos[MAX_CAMPOS][TAM_CAMPO], int numCampos,
                                      char *respuesta, int tamRespuesta) {
    sqlite3_stmt *stmt = NULL;
    int idProducto;

    if (numCampos != 2) {
        responder(respuesta, tamRespuesta, "ERR", "Formato incorrecto. Uso: 12|id_producto");
        return;
    }

    idProducto = atoi(campos[1]);

    if (!existe_id(db, "PRODUCTO", "id_producto", idProducto)) {
        responder(respuesta, tamRespuesta, "ERR", "El producto no existe");
        return;
    }

    if (sqlite3_prepare_v2(db,
        "DELETE FROM PRODUCTO WHERE id_producto = ?",
        -1, &stmt, NULL) != SQLITE_OK) {

        responder_bd_error(db, respuesta, tamRespuesta);
        return;
    }

    sqlite3_bind_int(stmt, 1, idProducto);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        responder(respuesta, tamRespuesta, "OK", "Producto eliminado correctamente");
    } else {
        responder_bd_error(db, respuesta, tamRespuesta);
    }

    sqlite3_finalize(stmt);
}

int procesarPeticion(sqlite3 *db, const char *peticion, char *respuesta, int tamRespuesta) {
    char campos[MAX_CAMPOS][TAM_CAMPO];
    int numCampos;

    if (db == NULL) {
        responder(respuesta, tamRespuesta, "ERR", "Base de datos no abierta");
        return 0;
    }

    numCampos = separar_campos(peticion, campos);

    if (numCampos == 0) {
        responder(respuesta, tamRespuesta, "ERR", "Peticion vacia");
        return 0;
    }

    if (strcmp(campos[0], "00") == 0) {
        responder(respuesta, tamRespuesta, "OK", "Conexion cerrada");

    } else if (strcmp(campos[0], "01") == 0) {
        comando_login(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "02") == 0) {
        comando_registrar_admin(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "03") == 0) {
        comando_registrar_cliente(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "04") == 0) {
        responder(respuesta, tamRespuesta, "ERR", "Importar catalogo no implementado");

    } else if (strcmp(campos[0], "05") == 0) {
        comando_ver_catalogo(db, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "06") == 0) {
        comando_confirmar_compra(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "07") == 0) {
        comando_modificar_pedido(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "08") == 0) {
        comando_eliminar_pedido(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "09") == 0) {
        comando_ver_pedidos_usuario(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "10") == 0) {
        comando_anyadir_producto(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "11") == 0) {
        comando_modificar_producto(db, campos, numCampos, respuesta, tamRespuesta);

    } else if (strcmp(campos[0], "12") == 0) {
        comando_eliminar_producto(db, campos, numCampos, respuesta, tamRespuesta);

    } else {
        responder(respuesta, tamRespuesta, "ERR", "Comando no reconocido");
    }

    return strncmp(respuesta, "OK|", 3) == 0;
}

int server(sqlite3 *db) {
    char peticion[MAX_PETICION];
    char respuesta[MAX_RESPUESTA];

    printf("\n=== SERVIDOR ===\n");
    printf("Escribe una peticion del cliente.\n");
    printf("Ejemplos:\n");
    printf("01|email|contrasena\n");
    printf("05\n");
    printf("06|1|1,1#2,1\n");
    printf("00 para salir\n\n");

    while (1) {
        printf("Cliente -> Servidor: ");

        if (fgets(peticion, sizeof(peticion), stdin) == NULL) {
            break;
        }

        quitar_salto_linea(peticion);

        procesarPeticion(db, peticion, respuesta, sizeof(respuesta));

        printf("Servidor -> Cliente: %s\n\n", respuesta);

        if (strcmp(peticion, "00") == 0) {
            break;
        }
    }

    return 0;
}
