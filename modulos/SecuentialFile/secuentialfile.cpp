#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
using namespace std;

struct IndexNode {
    int edad;
    int position;
    IndexNode* next;

    IndexNode(int e, int pos) : edad(e), position(pos), next(nullptr) {}
};

struct IndexList {
    IndexNode* head;

    IndexList() : head(nullptr) {}
};

IndexList globalIndexList; // Índice global en RAM

vector<string> dividir(const string& linea, char delimitador) {
    vector<string> tokens;
    string token;
    stringstream ss(linea);

    while (getline(ss, token, delimitador)) {
        tokens.push_back(token);
    }

    return tokens;
}

void insertarEnIndice(IndexList& indice, int edad, int position) {
    IndexNode* nuevoIndice = new IndexNode(edad, position);

    if (indice.head == nullptr || indice.head->edad > edad) {
        nuevoIndice->next = indice.head;
        indice.head = nuevoIndice;
        return;
    }

    IndexNode* current = indice.head;
    while (current->next != nullptr && current->next->edad < edad) {
        current = current->next;
    }

    nuevoIndice->next = current->next;
    current->next = nuevoIndice;
}

void eliminarDeIndice(IndexList& indice, int edad) {
    if (indice.head == nullptr) return;

    if (indice.head->edad == edad) {
        IndexNode* temp = indice.head;
        indice.head = indice.head->next;
        delete temp;
        return;
    }

    IndexNode* current = indice.head;
    while (current->next != nullptr && current->next->edad != edad) {
        current = current->next;
    }

    if (current->next == nullptr) return;

    IndexNode* temp = current->next;
    current->next = current->next->next;
    delete temp;
}

IndexNode* buscarEnIndice(IndexList& indice, int edad) {
    IndexNode* current = indice.head;
    while (current != nullptr) {
        if (current->edad == edad) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Nueva función para buscar por rango de edades
vector<string> buscarPorRango(IndexList& indice, int edadMin, int edadMax) {
    vector<string> resultados;
    IndexNode* current = indice.head;

    while (current != nullptr) {
        if (current->edad >= edadMin && current->edad <= edadMax) {
            resultados.push_back("Edad: " + to_string(current->edad) + ", Posición: " + to_string(current->position));
        }
        current = current->next;
    }

    return resultados;
}

void imprimirIndice(IndexNode* cabeza) {
    IndexNode* current = cabeza;
    while (current != nullptr) {
        cout << "Edad: " << current->edad
             << ", Posición: " << current->position
             << ", Siguiente posición: " << (current->next ? current->next->edad : -1) << endl;
        current = current->next;
    }
}

void liberarIndice(IndexNode* cabeza) {
    IndexNode* current = cabeza;
    while (current != nullptr) {
        IndexNode* temp = current;
        current = current->next;
        delete temp;
    }
}

IndexList leerCSV(const string& nombre_archivo) {
    ifstream archivo(nombre_archivo);
    IndexList indice;
    string linea;
    int posicion = 0;

    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombre_archivo << endl;
        return indice; // Retorna índice vacío si no se puede abrir el archivo
    }

    getline(archivo, linea); // Leer cabecera

    while (getline(archivo, linea)) {
        vector<string> columnas = dividir(linea, ',');
        if (columnas.size() < 4) {
            cerr << "Error en el formato de la línea: " << linea << endl;
            continue; // Si la línea no tiene suficientes columnas, continuar con la siguiente
        }
        int edad = stoi(columnas[3]); // La columna 3 es la edad
        insertarEnIndice(indice, edad, posicion);
        posicion++;
    }

    archivo.close();
    return indice;
}

void agregarRegistro(const string& nombre_archivo, const string& registro) {
    ofstream archivo(nombre_archivo, ios::app);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo para agregar un registro." << endl;
        return;
    }
    archivo << registro << endl;

    vector<string> columnas = dividir(registro, ',');
    if (columnas.size() < 4) {
        cerr << "Error en el formato del registro: " << registro << endl;
        return; // Si el registro no tiene suficientes columnas, retornar
    }

    int edad = stoi(columnas[3]); // La columna 3 es la edad
    int position = globalIndexList.head == nullptr ? 0 : (globalIndexList.head->position + 1);

    insertarEnIndice(globalIndexList, edad, position);
    archivo.close();
}

void guardarCSV(const string& nombre_archivo) {
    ofstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo para guardar." << endl;
        return;
    }

    // Escribir la cabecera
    archivo << "Nombre,Apellido,Carrera,Edad,Codigo\n";

    // Escribir cada registro del índice
    IndexNode* current = globalIndexList.head;
    while (current != nullptr) {
        // Aquí asumo que los datos son ficticios para el ejemplo.
        // Tendrías que reemplazar esto con el acceso a los datos reales.
        archivo << "Estudiante" << current->position << ",Apellido" << current->position
                << ",Carrera" << current->position << "," << current->edad << ",Codigo" << current->position << endl;
        current = current->next;
    }

    archivo.close();
}

void eliminarRegistro(const string& nombre_archivo, int edad) {
    eliminarDeIndice(globalIndexList, edad);
    guardarCSV(nombre_archivo);
}

extern "C" {
    void initIndice(const char* nombre_archivo = "estudiantes10.csv") { // Usar el archivo por defecto
        globalIndexList = leerCSV(nombre_archivo);
    }

    void agregarRegistroDesdePython(const char* nombre_archivo, const char* registro) {
        agregarRegistro(nombre_archivo, registro);
    }

    void eliminarRegistroDesdePython(const char* nombre_archivo, int edad) {
        eliminarRegistro(nombre_archivo, edad);
    }

    IndexNode* buscarRegistro(int edad) {
        return buscarEnIndice(globalIndexList, edad);
    }

    void buscarRegistroDesdePython(int edad, char** resultado) {
        IndexNode* resultadoNode = buscarRegistro(edad);
        if (resultadoNode) {
            string res = "Encontrado - Edad: " + to_string(resultadoNode->edad) + ", Posición: " + to_string(resultadoNode->position);
            *resultado = new char[res.size() + 1];
            strcpy(*resultado, res.c_str());
        } else {
            string res = "No se encontró registro con edad " + to_string(edad);
            *resultado = new char[res.size() + 1];
            strcpy(*resultado, res.c_str());
        }
    }

    void buscarPorRangoDesdePython(int edadMin, int edadMax, char*** resultados, int* size) {
        vector<string> res = buscarPorRango(globalIndexList, edadMin, edadMax);
        *size = res.size();

        // Asignar memoria para el array de resultados
        *resultados = (char**)malloc(*size * sizeof(char*));

        for (int i = 0; i < *size; ++i) {
            (*resultados)[i] = (char*)malloc((res[i].size() + 1) * sizeof(char));
            strcpy((*resultados)[i], res[i].c_str());
        }
    }

    void imprimirIndiceDesdePython() {
        imprimirIndice(globalIndexList.head);
    }

    void liberar() {
        liberarIndice(globalIndexList.head);
    }
}

int main() {
    // Inicializar el índice con un archivo CSV
    initIndice("estudiantes10.csv");
    
    // Agregar un registro al archivo y al índice
    const char* nuevoRegistro = "Juan,Perez,CS,23,73413194";
    //agregarRegistroDesdePython("estudiantes10.csv", nuevoRegistro);
    //cout << "Se ha agregado un nuevo registro." << endl;

    // Imprimir el índice completo
    cout << "Imprimiendo índice:" << endl;
    imprimirIndiceDesdePython();
    
    // Buscar un registro por edad
    int edadABuscar = 23;
    char* resultado;
    cout << "Buscando registro con edad: " << edadABuscar << endl;
    buscarRegistroDesdePython(edadABuscar, &resultado);
    cout << resultado << endl;
    delete[] resultado; // Liberar memoria asignada

    // Eliminar un registro por edad
    eliminarRegistroDesdePython("estudiantes10.csv", 23);
    cout << "Se ha eliminado el registro con edad: " << edadABuscar << endl;

    // Buscar por rango de edad
    int edadMin = 20, edadMax = 25;
    char** resultados;
    int size;
    cout << "Buscando registros en el rango [" << edadMin << ", " << edadMax << "]" << endl;
    buscarPorRangoDesdePython(edadMin, edadMax, &resultados, &size);
    
    for (int i = 0; i < size; ++i) {
        cout << resultados[i] << endl;
        free(resultados[i]); // Liberar memoria de cada resultado
    }
    free(resultados); // Liberar memoria del array de resultados

    // Liberar el índice
    liberar();
    
    return 0;
}
