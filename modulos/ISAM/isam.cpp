#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

const int MAX_REGISTROS_POR_NODO = 3;  // Máximo de registros por nodo del tercer nivel

// Estructura que representa un registro en el CSV
struct Registro {
    string nombre;
    string apellidos;
    string carrera;
    int edad;
    int dni;

    Registro(string n, string a, string c, int e, int d)
        : nombre(n), apellidos(a), carrera(c), edad(e), dni(d) {}
};

// Nodo del tercer nivel, apunta a los registros
struct NodoRegistro {
    vector<Registro*> registros;  // Punteros a registros con el mismo rango de edad
    NodoRegistro* siguiente;  // Puntero al siguiente nodo de registro en la hoja

    NodoRegistro() : siguiente(nullptr) {}

    bool isFull() const {
        return registros.size() >= MAX_REGISTROS_POR_NODO;
    }

    // Método para eliminar un registro por edad
    bool eliminarRegistroPorEdad(int edad) {
        for (auto it = registros.begin(); it != registros.end(); ++it) {
            if ((*it)->edad == edad) {
                delete *it;  // Liberar memoria
                registros.erase(it);
                return true;
            }
        }
        return false;
    }
};

// Nodo del segundo nivel, apunta a nodos del tercer nivel
struct NodoIntermedio {
    map<int, NodoRegistro*> punteros;  // Rango menor de edad -> nodo del tercer nivel
};

// Nodo del primer nivel, apunta a nodos del segundo nivel
struct NodoRaiz {
    map<int, NodoIntermedio*> punteros;  // Rango mayor de edad -> nodo del segundo nivel
};

// Estructura ISAM con tres niveles
class ISAM {
public:
    NodoRaiz* raiz;

    ISAM() {
        raiz = new NodoRaiz();
    }

    // Insertar un registro en el ISAM
    void insertarRegistro(Registro* registro) {
        int rangoMayor = (registro->edad / 10) * 10;  // Primer nivel: rango 10, 20, 30, etc.
        int rangoMenor = (registro->edad / 5) * 5;    // Segundo nivel: subrango 10-14, 15-19, etc.

        // Crear los niveles si no existen
        if (raiz->punteros.find(rangoMayor) == raiz->punteros.end()) {
            raiz->punteros[rangoMayor] = new NodoIntermedio();
        }
        if (raiz->punteros[rangoMayor]->punteros.find(rangoMenor) == raiz->punteros[rangoMayor]->punteros.end()) {
            raiz->punteros[rangoMayor]->punteros[rangoMenor] = new NodoRegistro();
        }

        // Insertar el registro en el nodo adecuado
        NodoRegistro* nodo = raiz->punteros[rangoMayor]->punteros[rangoMenor];
        if (!nodo->isFull()) {
            nodo->registros.push_back(registro);
        } else {
            // Si el nodo está lleno, se crea un nuevo nodo y se ajustan los punteros
            NodoRegistro* nuevoNodo = new NodoRegistro();
            nuevoNodo->registros.push_back(registro);

            // Conectar el nuevo nodo al siguiente en la lista
            nuevoNodo->siguiente = nodo->siguiente;
            nodo->siguiente = nuevoNodo;
        }
    }

    // Función para leer el CSV e insertar los registros
    void leerCSV(const string& nombreArchivo) {
        ifstream archivo(nombreArchivo);
        string linea;

        if (!archivo.is_open()) {
            cerr << "No se pudo abrir el archivo." << endl;
            return;
        }

        // Ignorar la primera línea (encabezados)
        getline(archivo, linea);

        while (getline(archivo, linea)) {
            stringstream ss(linea);
            string nombre, apellidos, carrera, edadStr, dniStr;
            int edad, dni;

            getline(ss, nombre, ',');
            getline(ss, apellidos, ',');
            getline(ss, carrera, ',');
            getline(ss, edadStr, ',');
            getline(ss, dniStr, ',');

            try {
                edad = stoi(edadStr);
                dni = stoi(dniStr);
            } catch (const invalid_argument& e) {
                cerr << "Error en la conversión de edad o DNI en la línea: " << linea << endl;
                continue;
            }

            // Crear el registro e insertarlo en el ISAM
            Registro* nuevoRegistro = new Registro(nombre, apellidos, carrera, edad, dni);
            insertarRegistro(nuevoRegistro);
        }

        archivo.close();
    }

    // Función para buscar registros en un rango de edades
    vector<Registro*> buscarRegistrosPorRango(int rangoInferior, int rangoSuperior) {
        vector<Registro*> resultados; // Vector para almacenar registros encontrados
        for (const auto& primerNivel : raiz->punteros) {
            if (primerNivel.first >= rangoInferior && primerNivel.first <= rangoSuperior) {
                for (const auto& segundoNivel : primerNivel.second->punteros) {
                    NodoRegistro* nodo = segundoNivel.second;
                    while (nodo) {
                        for (const auto& registro : nodo->registros) {
                            if (registro->edad >= rangoInferior && registro->edad <= rangoSuperior) {
                                resultados.push_back(registro); // Agregar registro encontrado al vector
                            }
                        }
                        nodo = nodo->siguiente;  // Avanzar al siguiente nodo en la hoja
                    }
                }
            }
        }
        return resultados; // Retornar los registros encontrados
    }

    // Función para insertar un nuevo registro en el ISAM y en el CSV
    void insertarNuevoRegistro(const string& nombre, const string& apellidos, const string& carrera, int edad, int dni) {
        Registro* nuevoRegistro = new Registro(nombre, apellidos, carrera, edad, dni);
        insertarRegistro(nuevoRegistro);
        actualizarCSV(nuevoRegistro);
    }

    // Función para actualizar el CSV
    void actualizarCSV(Registro* registro) {
        ofstream archivo("estudiantes10.csv", ios::app); // Abrir en modo append

        if (!archivo.is_open()) {
            cerr << "No se pudo abrir el archivo para escribir." << endl;
            return;
        }

        archivo << registro->nombre << "," 
                << registro->apellidos << "," 
                << registro->carrera << "," 
                << registro->edad << "," 
                << registro->dni << endl;

        archivo.close();
    }

    // Función para eliminar un registro del ISAM y del CSV según la edad
    void eliminarRegistroPorEdad(int edad) {
        for (const auto& primerNivel : raiz->punteros) {
            for (const auto& segundoNivel : primerNivel.second->punteros) {
                if (segundoNivel.second->eliminarRegistroPorEdad(edad)) {
                    cout << "Registro con edad " << edad << " eliminado del ISAM." << endl;
                    actualizarCSVEliminacion(edad);
                    return;
                }
            }
        }
        cout << "Registro con edad " << edad << " no encontrado." << endl;
    }

    // Función para actualizar el CSV eliminando el registro
    void actualizarCSVEliminacion(int edad) {
        ifstream archivo("estudiantes10.csv");
        ofstream tempArchivo("temp.csv");
        string linea;

        if (!archivo.is_open() || !tempArchivo.is_open()) {
            cerr << "No se pudo abrir el archivo." << endl;
            return;
        }

        // Ignorar la primera línea (encabezados)
        getline(archivo, linea);
        tempArchivo << linea << endl; // Escribir encabezados en el nuevo archivo

        while (getline(archivo, linea)) {
            stringstream ss(linea);
            string nombre, apellidos, carrera, edadStr, dniStr;

            getline(ss, nombre, ',');
            getline(ss, apellidos, ',');
            getline(ss, carrera, ',');
            getline(ss, edadStr, ',');
            getline(ss, dniStr, ',');

            int edadRegistro = stoi(edadStr);
            if (edadRegistro != edad) {
                tempArchivo << linea << endl; // Escribir solo los registros que no se eliminan
            }
        }

        archivo.close();
        tempArchivo.close();

        // Reemplazar el archivo original por el temporal
        remove("estudiantes10.csv");
        rename("temp.csv", "estudiantes10.csv");
    }

    // Función para imprimir el ISAM (pretty print)
    void imprimirISAM() const {
        for (const auto& primerNivel : raiz->punteros) {
            cout << "Rango Mayor (Primer Nivel): " << primerNivel.first << endl;
            for (const auto& segundoNivel : primerNivel.second->punteros) {
                cout << "  Rango Menor (Segundo Nivel): " << segundoNivel.first << endl;
                NodoRegistro* nodo = segundoNivel.second;
                while (nodo) {
                    for (const auto& registro : nodo->registros) {
                        cout << "    Registro: " << registro->nombre << " " << registro->apellidos 
                             << ", Edad: " << registro->edad 
                             << ", DNI: " << registro->dni << endl;
                    }
                    nodo = nodo->siguiente;  // Avanzar al siguiente nodo
                }
            }
        }
    }
};


extern "C" {
    ISAM* isam_create() {
        return new ISAM();  // Crear un nuevo objeto ISAM
    }

    void isam_destroy(ISAM* isam) {
        delete isam;  // Liberar memoria del objeto ISAM
    }

    void isam_leer_csv(ISAM* isam, const char* nombreArchivo) {
        isam->leerCSV(nombreArchivo);  // Llamar a la función para leer CSV
    }

    // Función para buscar registros en un rango de edades
    Registro** isam_buscar_por_rango(ISAM* isam, int rangoInferior, int rangoSuperior, int* count) {
        vector<Registro*> resultados = isam->buscarRegistrosPorRango(rangoInferior, rangoSuperior);
        *count = resultados.size();  // Devolver el número de registros encontrados
        Registro** registros = new Registro*[*count];  // Crear un arreglo de punteros a registros

        for (int i = 0; i < *count; ++i) {
            registros[i] = resultados[i];  // Copiar punteros de registros al arreglo
        }
        return registros;  // Retornar el arreglo de registros encontrados
    }

    void isam_eliminar_por_edad(ISAM* isam, int edad) {
        isam->eliminarRegistroPorEdad(edad);  // Llamar a la función para eliminar registro
    }

    void isam_imprimir(ISAM* isam) {
        isam->imprimirISAM();  // Llamar a la función para imprimir el ISAM
    }
}

int main() {
    ISAM isam;

    // Leer el archivo CSV
    isam.leerCSV("estudiantes10.csv");

    // Imprimir el contenido del ISAM
    isam.imprimirISAM();

    // Buscar registros en un rango de edades
    int rangoInferior = 18, rangoSuperior = 25;
    cout << "Buscando registros en el rango de edades [" << rangoInferior << ", " << rangoSuperior << "]:" << endl;
    vector<Registro*> registrosEncontrados = isam.buscarRegistrosPorRango(rangoInferior, rangoSuperior);

    // Serializar los registros encontrados
    ofstream archivoSalida("registros_encontrados.csv");
    if (archivoSalida.is_open()) {
        archivoSalida << "Nombre,Apellidos,Carrera,Edad,DNI" << endl; // Encabezados
        for (const auto& registro : registrosEncontrados) {
            archivoSalida << registro->nombre << ","
                          << registro->apellidos << ","
                          << registro->carrera << ","
                          << registro->edad << ","
                          << registro->dni << endl;
        }
        archivoSalida.close();
        cout << "Registros encontrados han sido serializados a 'registros_encontrados.csv'." << endl;
    } else {
        cerr << "No se pudo abrir el archivo de salida." << endl;
    }

    // Ejemplo de inserción de un nuevo registro
    isam.insertarNuevoRegistro("Juan", "Pérez", "Ingeniería", 20, 12345678);

    // Ejemplo de eliminación de un registro por edad
    isam.eliminarRegistroPorEdad(20);

    return 0;
}
