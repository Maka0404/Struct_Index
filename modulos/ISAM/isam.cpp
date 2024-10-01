#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

const int MAX_REGISTROS_POR_NODO = 3; 

struct Registro {
    string nombre;
    string apellidos;
    string carrera;
    int edad;
    int dni;

    Registro(string n, string a, string c, int e, int d)
        : nombre(n), apellidos(a), carrera(c), edad(e), dni(d) {}
};

struct NodoRegistro {
    vector<Registro*> registros;
    NodoRegistro* siguiente;

    NodoRegistro() : siguiente(nullptr) {}

    bool isFull() const {
        return registros.size() >= MAX_REGISTROS_POR_NODO;
    }

    bool eliminarRegistroPorEdad(int edad) {
        for (auto it = registros.begin(); it != registros.end(); ++it) {
            if ((*it)->edad == edad) {
                delete *it;
                registros.erase(it);
                return true;
            }
        }
        return false;
    }
};

struct NodoIntermedio {
    map<int, NodoRegistro*> punteros;
};

struct NodoRaiz {
    map<int, NodoIntermedio*> punteros;
};

class ISAM {
public:
    NodoRaiz* raiz;

    ISAM() {
        raiz = new NodoRaiz();
    }

    void insertarRegistro(Registro* registro) {
        int rangoMayor = (registro->edad / 10) * 10;
        int rangoMenor = (registro->edad / 5) * 5;

        if (raiz->punteros.find(rangoMayor) == raiz->punteros.end()) {
            raiz->punteros[rangoMayor] = new NodoIntermedio();
        }
        if (raiz->punteros[rangoMayor]->punteros.find(rangoMenor) == raiz->punteros[rangoMayor]->punteros.end()) {
            raiz->punteros[rangoMayor]->punteros[rangoMenor] = new NodoRegistro();
        }

        NodoRegistro* nodo = raiz->punteros[rangoMayor]->punteros[rangoMenor];
        if (!nodo->isFull()) {
            nodo->registros.push_back(registro);
        } else {
            NodoRegistro* nuevoNodo = new NodoRegistro();
            nuevoNodo->registros.push_back(registro);
            nuevoNodo->siguiente = nodo->siguiente;
            nodo->siguiente = nuevoNodo;
        }
    }

    void leerCSV(const string& nombreArchivo) {
        ifstream archivo(nombreArchivo);
        string linea;

        if (!archivo.is_open()) {
            cerr << "No se pudo abrir el archivo." << endl;
            return;
        }

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

            Registro* nuevoRegistro = new Registro(nombre, apellidos, carrera, edad, dni);
            insertarRegistro(nuevoRegistro);
        }

        archivo.close();
    }

    vector<Registro*> buscarRegistrosPorRango(int rangoInferior, int rangoSuperior) {
        vector<Registro*> resultados;
        for (const auto& primerNivel : raiz->punteros) {
            if (primerNivel.first >= rangoInferior && primerNivel.first <= rangoSuperior) {
                for (const auto& segundoNivel : primerNivel.second->punteros) {
                    NodoRegistro* nodo = segundoNivel.second;
                    while (nodo) {
                        for (const auto& registro : nodo->registros) {
                            if (registro->edad >= rangoInferior && registro->edad <= rangoSuperior) {
                                resultados.push_back(registro);
                            }
                        }
                        nodo = nodo->siguiente;
                    }
                }
            }
        }
        return resultados;
    }

    void insertarNuevoRegistro(const string& nombre, const string& apellidos, const string& carrera, int edad, int dni) {
        Registro* nuevoRegistro = new Registro(nombre, apellidos, carrera, edad, dni);
        insertarRegistro(nuevoRegistro);
        actualizarCSV(nuevoRegistro);
    }

    void actualizarCSV(Registro* registro) {
        ofstream archivo("estudiantes10.csv", ios::app);

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

    void actualizarCSVEliminacion(int edad) {
        ifstream archivo("estudiantes10.csv");
        ofstream tempArchivo("temp.csv");
        string linea;

        if (!archivo.is_open() || !tempArchivo.is_open()) {
            cerr << "No se pudo abrir el archivo." << endl;
            return;
        }

        getline(archivo, linea);
        tempArchivo << linea << endl;

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
                tempArchivo << linea << endl;
            }
        }

        archivo.close();
        tempArchivo.close();

        remove("estudiantes10.csv");
        rename("temp.csv", "estudiantes10.csv");
    }

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
                    nodo = nodo->siguiente;
                }
            }
        }
    }
};

extern "C" {
    ISAM* isam_create() {
        return new ISAM();
    }

    void isam_destroy(ISAM* isam) {
        delete isam;
    }

    void isam_leer_csv(ISAM* isam, const char* nombreArchivo) {
        isam->leerCSV(nombreArchivo);
    }

    Registro** isam_buscar_por_rango(ISAM* isam, int rangoInferior, int rangoSuperior, int* count) {
        vector<Registro*> resultados = isam->buscarRegistrosPorRango(rangoInferior, rangoSuperior);
        *count = resultados.size();
        Registro** registros = new Registro*[*count];

        for (int i = 0; i < *count; ++i) {
            registros[i] = resultados[i];
        }
        return registros;
    }

    void isam_eliminar_por_edad(ISAM* isam, int edad) {
        isam->eliminarRegistroPorEdad(edad);
    }

    void isam_imprimir(ISAM* isam) {
        isam->imprimirISAM();
    }
}

int main() {
    ISAM isam;
    isam.leerCSV("estudiantes10.csv");
    isam.imprimirISAM();

    int rangoInferior = 18, rangoSuperior = 25;
    cout << "Buscando registros en el rango de edades [" << rangoInferior << ", " << rangoSuperior << "]:" << endl;
    vector<Registro*> registrosEncontrados = isam.buscarRegistrosPorRango(rangoInferior, rangoSuperior);

    ofstream archivoSalida("registros_encontrados.csv");
    if (archivoSalida.is_open()) {
        archivoSalida << "Nombre,Apellidos,Carrera,Edad,DNI" << endl;
        for (const auto& registro : registrosEncontrados) {
            archivoSalida << registro->nombre << ","
                          << registro->apellidos << ","
                          << registro->carrera << ","
                          << registro->edad << ","
                          << registro->dni << endl;
        }
        archivoSalida.close();
        cout << "Registros encontrados han sido serializados a registros_encontrados.csv." << endl;
    }

    int edadAEliminar = 20;
    isam.eliminarRegistroPorEdad(edadAEliminar);
    isam.imprimirISAM();

    return 0;
}
