#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <sstream>  // Para stringstream en op18
#include <limits>   // FIX: Para numeric_limits<streamsize>
#include <cstdlib>  // Para system(mkdir)

using namespace std;

/*
 * Fase 3: Implementación en C++ de BibliotecaDB.
 * Entidades: Autor, Libro, Estudiante, Préstamo.
 * Soporte CRUD completo con validaciones.
 * Persistencia en CSV (.txt) con fstream.
 * Consultas: Libros por estudiante (activos), ranking autores por libros.
 * Manejo errores: IDs únicos, FKs válidas, integridad referencial.
 * Relaciones: Simuladas con bucles (no SQL).
 */

struct Autor {
    int id;
    string nombre;
    string nacionalidad;
};

struct Libro {
    int id;
    string titulo;
    string isbn;
    int ano;
    int id_autor;  // FK a Autor
};

struct Estudiante {
    int id;
    string nombre;
    string grado;  // e.g., "2º Bachillerato"
};

struct Prestamo {
    int id;
    int id_libro;  // FK a Libro
    int id_estudiante;  // FK a Estudiante
    string fecha_prestamo;
    string fecha_devolucion;  // "" si activo
};

struct DB {
    vector<Autor> autores;
    vector<Libro> libros;
    vector<Estudiante> estudiantes;
    vector<Prestamo> prestamos;

    // Utilidades CSV: Split y escape para comas/comillas
    static vector<string> splitCSV(const string& s) {
        vector<string> out;
        string cur;
        bool inq = false;
        for (char c : s) {
            if (c == '"') { inq = !inq; cur.push_back(c); }
            else if (c == ',' && !inq) { out.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        out.push_back(cur);
        // Quitar comillas externas
        for (auto& x : out) {
            if (x.size() >= 2 && x.front() == '"' && x.back() == '"') {
                x = x.substr(1, x.size() - 2);
            }
        }
        return out;
    }

    static string esc(const string& s) {
        if (s.find(',') != string::npos || s.find('"') != string::npos) {
            string r = "\"";
            for (char c : s) { if (c == '"') r += "\"\""; else r.push_back(c); }
            r += "\"";
            return r;
        }
        return s;
    }

    // Carga desde CSV (salta header)
    void loadAutores(const string& path) {
        autores.clear();
        ifstream f(path);
        if (!f.good()) return;
        string line;
        getline(f, line);  // Header
        while (getline(f, line)) {
            if (line.empty()) continue;
            auto v = splitCSV(line);
            if (v.size() < 3) continue;
            Autor a{stoi(v[0]), v[1], v[2]};
            autores.push_back(a);
        }
    }

    void loadLibros(const string& path) {
        libros.clear();
        ifstream f(path);
        if (!f.good()) return;
        string line;
        getline(f, line);
        while (getline(f, line)) {
            if (line.empty()) continue;
            auto v = splitCSV(line);
            if (v.size() < 5) continue;
            Libro l{stoi(v[0]), v[1], v[2], stoi(v[3]), stoi(v[4])};
            libros.push_back(l);
        }
    }

    void loadEstudiantes(const string& path) {
        estudiantes.clear();
        ifstream f(path);
        if (!f.good()) return;
        string line;
        getline(f, line);
        while (getline(f, line)) {
            if (line.empty()) continue;
            auto v = splitCSV(line);
            if (v.size() < 3) continue;
            Estudiante e{stoi(v[0]), v[1], v[2]};
            estudiantes.push_back(e);
        }
    }

    void loadPrestamos(const string& path) {
        prestamos.clear();
        ifstream f(path);
        if (!f.good()) return;
        string line;
        getline(f, line);
        while (getline(f, line)) {
            if (line.empty()) continue;
            auto v = splitCSV(line);
            if (v.size() < 5) continue;
            Prestamo p{stoi(v[0]), stoi(v[1]), stoi(v[2]), v[3], v[4]};
            prestamos.push_back(p);
        }
    }

    // Guarda en CSV (con header)
    void saveAutores(const string& path) {
        ofstream f(path);
        if (!f) return;
        f << "id,nombre,nacionalidad\n";
        for (auto& a : autores) {
            f << a.id << "," << esc(a.nombre) << "," << esc(a.nacionalidad) << "\n";
        }
    }

    void saveLibros(const string& path) {
        ofstream f(path);
        if (!f) return;
        f << "id,titulo,isbn,ano_publicacion,id_autor\n";
        for (auto& l : libros) {
            f << l.id << "," << esc(l.titulo) << "," << esc(l.isbn) << "," << l.ano << "," << l.id_autor << "\n";
        }
    }

    void saveEstudiantes(const string& path) {
        ofstream f(path);
        if (!f) return;
        f << "id,nombre,grado\n";
        for (auto& e : estudiantes) {
            f << e.id << "," << esc(e.nombre) << "," << esc(e.grado) << "\n";
        }
    }

    void savePrestamos(const string& path) {
        ofstream f(path);
        if (!f) return;
        f << "id,id_libro,id_estudiante,fecha_prestamo,fecha_devolucion\n";
        for (auto& p : prestamos) {
            f << p.id << "," << p.id_libro << "," << p.id_estudiante << "," << esc(p.fecha_prestamo) << "," << esc(p.fecha_devolucion) << "\n";
        }
    }

    // Helpers: Existencia y disponible
    bool idAutorExiste(int id) {
        return any_of(autores.begin(), autores.end(), [id](auto& a){ return a.id == id; });
    }
    bool idLibroExiste(int id) {
        return any_of(libros.begin(), libros.end(), [id](auto& l){ return l.id == id; });
    }
    bool idEstudianteExiste(int id) {
        return any_of(estudiantes.begin(), estudiantes.end(), [id](auto& e){ return e.id == id; });
    }
    bool idPrestamoExiste(int id) {
        return any_of(prestamos.begin(), prestamos.end(), [id](auto& p){ return p.id == id; });
    }

    bool libroDisponible(int id_libro) {
        for (auto& p : prestamos) {
            if (p.id_libro == id_libro && p.fecha_devolucion.empty()) return false;
        }
        return true;
    }

    // CRUD Autor
    bool addAutor(Autor a) {
        if (idAutorExiste(a.id)) return false;
        autores.push_back(a);
        return true;
    }
    bool updateAutor(int id, string nombre, string nac) {
        for (auto& a : autores) {
            if (a.id == id) {
                a.nombre = nombre;
                a.nacionalidad = nac;
                return true;
            }
        }
        return false;
    }
    bool deleteAutor(int id) {
        // No borrar si referenciado por libros
        for (auto& l : libros) if (l.id_autor == id) return false;
        auto it = remove_if(autores.begin(), autores.end(), [id](auto& a){ return a.id == id; });
        if (it == autores.end()) return false;
        autores.erase(it, autores.end());
        return true;
    }

    // CRUD Libro
    bool addLibro(Libro l) {
        if (idLibroExiste(l.id)) return false;
        if (!idAutorExiste(l.id_autor)) return false;
        libros.push_back(l);
        return true;
    }
    bool updateLibro(int id, string titulo, string isbn, int ano, int id_autor) {
        for (auto& l : libros) {
            if (l.id == id) {
                if (!idAutorExiste(id_autor)) return false;
                l.titulo = titulo;
                l.isbn = isbn;
                l.ano = ano;
                l.id_autor = id_autor;
                return true;
            }
        }
        return false;
    }
    bool deleteLibro(int id) {
        if (!libroDisponible(id)) return false;
        auto it = remove_if(libros.begin(), libros.end(), [id](auto& l){ return l.id == id; });
        if (it == libros.end()) return false;
        libros.erase(it, libros.end());
        return true;
    }

    // CRUD Estudiante
    bool addEstudiante(Estudiante e) {
        if (idEstudianteExiste(e.id)) return false;
        estudiantes.push_back(e);
        return true;
    }
    bool updateEstudiante(int id, string nombre, string grado) {
        for (auto& e : estudiantes) {
            if (e.id == id) {
                e.nombre = nombre;
                e.grado = grado;
                return true;
            }
        }
        return false;
    }
    bool deleteEstudiante(int id) {
        // No borrar si tiene préstamos
        for (auto& p : prestamos) if (p.id_estudiante == id) return false;
        auto it = remove_if(estudiantes.begin(), estudiantes.end(), [id](auto& e){ return e.id == id; });
        if (it == estudiantes.end()) return false;
        estudiantes.erase(it, estudiantes.end());
        return true;
    }

    // CRUD Préstamo
    bool addPrestamo(Prestamo p) {
        if (idPrestamoExiste(p.id)) return false;
        if (!idLibroExiste(p.id_libro) || !idEstudianteExiste(p.id_estudiante)) return false;
        if (!libroDisponible(p.id_libro)) return false;
        prestamos.push_back(p);
        return true;
    }
    bool devolverPrestamo(int id_prestamo, string fecha_devolucion) {
        for (auto& p : prestamos) {
            if (p.id == id_prestamo) {
                if (!p.fecha_devolucion.empty()) return false;
                p.fecha_devolucion = fecha_devolucion;
                return true;
            }
        }
        return false;
    }
    bool deletePrestamo(int id) {
        // Solo históricos (no activos)
        for (auto& p : prestamos) {
            if (p.id == id && p.fecha_devolucion.empty()) return false;
        }
        auto it = remove_if(prestamos.begin(), prestamos.end(), [id](auto& p){ return p.id == id; });
        if (it == prestamos.end()) return false;
        prestamos.erase(it, prestamos.end());
        return true;
    }

    // Consultas
    void listarLibrosPrestadosPorEstudiante(int id_est) {
        cout << "Libros prestados (activos) por estudiante " << id_est << ":\n";
        for (auto& p : prestamos) {
            if (p.id_estudiante == id_est && p.fecha_devolucion.empty()) {
                auto it = find_if(libros.begin(), libros.end(), [p](auto& l){ return l.id == p.id_libro; });
                if (it != libros.end()) {
                    cout << " - [" << it->id << "] " << it->titulo << " (ISBN " << it->isbn << ") prestado el " << p.fecha_prestamo << "\n";
                }
            }
        }
    }

    void rankingAutoresPorCantidadLibros(int topN = 10) {
        unordered_map<int, int> cnt;
        for (auto& l : libros) cnt[l.id_autor]++;
        vector<pair<int, int>> v(cnt.begin(), cnt.end());
        sort(v.begin(), v.end(), [](auto& a, auto& b){ return a.second > b.second; });
        cout << "Autores con mas libros:\n";
        int shown = 0;
        for (auto& pr : v) {
            auto it = find_if(autores.begin(), autores.end(), [pr](auto& a){ return a.id == pr.first; });
            string nombre = (it != autores.end() ? it->nombre : "Autor#" + to_string(pr.first));
            cout << " - " << nombre << " : " << pr.second << "\n";
            if (++shown >= topN) break;
        }
    }

    // Listados para Read
    void listarLibros() {
        cout << "Libros:\n";
        for (auto& l : libros) {
            cout << "[" << l.id << "] " << l.titulo << " | ISBN " << l.isbn << " | " << l.ano << " | autor " << l.id_autor << "\n";
        }
    }
    void listarAutores() {
        cout << "Autores:\n";
        for (auto& a : autores) {
            cout << "[" << a.id << "] " << a.nombre << " (" << a.nacionalidad << ")\n";
        }
    }
    void listarEstudiantes() {
        cout << "Estudiantes:\n";
        for (auto& e : estudiantes) {
            cout << "[" << e.id << "] " << e.nombre << " - " << e.grado << "\n";
        }
    }
    void listarPrestamos() {
        cout << "Préstamos:\n";
        for (auto& p : prestamos) {
            cout << "[" << p.id << "] Libro " << p.id_libro << " a Est " << p.id_estudiante << " (" << p.fecha_prestamo;
            if (p.fecha_devolucion.empty()) cout << " - ACTIVO)\n";
            else cout << " - Devuelto " << p.fecha_devolucion << ")\n";
        }
    }
};

string DATA_DIR = "./data";

void initDataDir() {
    system("mkdir -p data");  // Crea dir si no existe
}

void cargarTodo(DB& db) {
    initDataDir();
    db.loadAutores(DATA_DIR + "/autores.csv");
    db.loadLibros(DATA_DIR + "/libros.csv");
    db.loadEstudiantes(DATA_DIR + "/estudiantes.csv");
    db.loadPrestamos(DATA_DIR + "/prestamos.csv");
}

void guardarTodo(DB& db) {
    initDataDir();
    db.saveAutores(DATA_DIR + "/autores.csv");
    db.saveLibros(DATA_DIR + "/libros.csv");
    db.saveEstudiantes(DATA_DIR + "/estudiantes.csv");
    db.savePrestamos(DATA_DIR + "/prestamos.csv");
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    DB db;
    cargarTodo(db);

    int opcion;
    while (true) {
        cout << "\n--- Menú BibliotecaDB ---\n";
        cout << "1. Agregar Libro\n2. Listar Libros\n3. Actualizar Libro\n4. Borrar Libro\n";
        cout << "5. Agregar Autor\n6. Listar Autores\n7. Actualizar Autor\n8. Borrar Autor\n";
        cout << "9. Agregar Estudiante\n10. Listar Estudiantes\n11. Actualizar Estudiante\n12. Borrar Estudiante\n";
        cout << "13. Agregar Préstamo\n14. Listar Préstamos\n15. Devolver Préstamo\n16. Borrar Préstamo (histórico)\n";
        cout << "17. Listar libros prestados por estudiante\n18. Autores con más libros\n";
        cout << "0. Salir y guardar\nElección: ";
        cin >> opcion;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // FIX: Limpia buffer para getline

        switch (opcion) {
            case 1: {  // Agregar Libro
                Libro l;
                cout << "ID Título ISBN Año ID_Autor: ";
                cin >> l.id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // FIX: Limpia después de cin>>
                getline(cin, l.titulo);
                getline(cin, l.isbn);
                cin >> l.ano >> l.id_autor;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << (db.addLibro(l) ? "OK\n" : "Error: ID duplicado o autor inexistente\n");
                break;
            }
            case 2: db.listarLibros(); break;  // Listar
            case 3: {  // Update Libro
                int id, ano, id_autor;
                string titulo, isbn;
                cout << "ID nuevo_título nuevo_isbn nuevo_año nuevo_id_autor: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, titulo);
                getline(cin, isbn);
                cin >> ano >> id_autor;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << (db.updateLibro(id, titulo, isbn, ano, id_autor) ? "OK\n" : "Error\n");
                break;
            }
            case 4: {  // Delete Libro
                int id;
                cout << "ID a borrar: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << (db.deleteLibro(id) ? "OK\n" : "Error: Préstamo activo\n");
                break;
            }
            case 5: {  // Agregar Autor
                Autor a;
                cout << "ID Nombre Nacionalidad: ";
                cin >> a.id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, a.nombre);
                getline(cin, a.nacionalidad);
                cout << (db.addAutor(a) ? "OK\n" : "Error: ID duplicado\n");
                break;
            }
            case 6: db.listarAutores(); break;
            case 7: {  // Update Autor
                int id;
                string nombre, nac;
                cout << "ID nuevo_nombre nueva_nacionalidad: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, nombre);
                getline(cin, nac);
                cout << (db.updateAutor(id, nombre, nac) ? "OK\n" : "Error\n");
                break;
            }
            case 8: {  // Delete Autor
                int id;
                cout << "ID a borrar: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << (db.deleteAutor(id) ? "OK\n" : "Error: Referenciado por libros\n");
                break;
            }
            case 9: {  // Agregar Estudiante
                Estudiante e;
                cout << "ID Nombre Grado: ";
                cin >> e.id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, e.nombre);
                getline(cin, e.grado);
                cout << (db.addEstudiante(e) ? "OK\n" : "Error: ID duplicado\n");
                break;
            }
            case 10: db.listarEstudiantes(); break;
            case 11: {  // Update Estudiante
                int id;
                string nombre, grado;
                cout << "ID nuevo_nombre nuevo_grado: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, nombre);
                getline(cin, grado);
                cout << (db.updateEstudiante(id, nombre, grado) ? "OK\n" : "Error\n");
                break;
            }
            case 12: {  // Delete Estudiante
                int id;
                cout << "ID a borrar: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << (db.deleteEstudiante(id) ? "OK\n" : "Error: Tiene préstamos\n");
                break;
            }
            case 13: {  // Agregar Préstamo
                Prestamo p;
                cout << "ID ID_Libro ID_Est Fecha (AAAA-MM-DD): ";
                cin >> p.id >> p.id_libro >> p.id_estudiante;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, p.fecha_prestamo);
                p.fecha_devolucion = "";
                cout << (db.addPrestamo(p) ? "OK\n" : "Error: IDs inválidos o libro no disponible\n");
                break;
            }
            case 14: db.listarPrestamos(); break;
            case 15: {  // Devolver
                int id;
                string fecha;
                cout << "ID_Préstamo Fecha_Devolución: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, fecha);
                cout << (db.devolverPrestamo(id, fecha) ? "OK\n" : "Error: Ya devuelto o inválido\n");
                break;
            }
            case 16: {  // Delete Préstamo histórico
                int id;
                cout << "ID a borrar: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << (db.deletePrestamo(id) ? "OK\n" : "Error: Activo o inexistente\n");
                break;
            }
            case 17: {  // Consulta 1
                int id_est;
                cout << "ID_Estudiante: ";
                cin >> id_est;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                db.listarLibrosPrestadosPorEstudiante(id_est);
                break;
            }
            case 18: {  // Consulta 2
                int topN = 10;
                cout << "Top N (default 10): ";
                string input;
                getline(cin, input);  // FIX: Usa getline directo después de ignore previo
                if (!input.empty()) {
                    stringstream ss(input);
                    ss >> topN;
                }
                db.rankingAutoresPorCantidadLibros(topN);
                break;
            }
            case 0: {
                guardarTodo(db);
                cout << "Datos guardados en ./data/. ¡Adiós!\n";
                return 0;
            }
            default: cout << "Opción inválida.\n";
        }
    }
    return 0;
}