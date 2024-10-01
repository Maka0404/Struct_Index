# Proyecto de Gestión de Registros de Estudiantes

Este proyecto está diseñado para gestionar registros de estudiantes almacenados en un archivo CSV. Se utiliza una estructura de índice en memoria (ISAM) para permitir búsquedas rápidas y eficientes por edad, así como la capacidad de agregar, eliminar y buscar registros.

## Contenido

- [Estructuras de Datos](#estructuras-de-datos)
- [Funciones Principales](#funciones-principales)
- [Uso](#uso)
- [Requisitos](#requisitos)
- [Ejemplo de Archivo CSV](#ejemplo-de-archivo-csv)
- [Contribuciones](#contribuciones)
- [Licencia](#licencia)

## Estructuras de Datos

### Índice en Memoria

El índice está representado por una lista enlazada que contiene nodos con información sobre la edad y la posición de cada registro en el archivo CSV. La estructura de nodos se define de la siguiente manera:

```cpp
struct IndexNode {
    int edad;
    int position;
    IndexNode* next;
};
```
ISAM (Indexed Sequential Access Method)
La estructura ISAM tiene tres niveles:

Nodo Raíz: Contiene punteros a nodos intermedios basados en rangos de edad.
Nodo Intermedio: Contiene punteros a nodos de registros basados en rangos más finos de edad.
Nodo Registro: Contiene los registros que corresponden a un rango de edad específico.
Funciones Principales
Leer CSV: Carga los registros desde un archivo CSV y construye el índice en memoria.
Agregar Registro: Permite agregar un nuevo registro al archivo CSV y al índice.
Eliminar Registro: Permite eliminar un registro basado en la edad.
Buscar Registro: Busca un registro específico por edad.
Buscar por Rango: Permite buscar registros dentro de un rango de edades.
Uso
Para utilizar el programa, primero inicializa el índice con un archivo CSV:

initIndice("estudiantes10.csv");

Luego, puedes agregar, eliminar o buscar registros:

const char* nuevoRegistro = "Juan,Perez,CS,23,73413194";
agregarRegistroDesdePython("estudiantes10.csv", nuevoRegistro);
eliminarRegistroDesdePython("estudiantes10.csv", 23);
buscarRegistroDesdePython(23, &resultado);



