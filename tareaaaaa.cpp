#include <iostream>
#include <string>
#include <vector>
#include <limits>

class Libro {
public:
    std::string titulo;
    std::string autor;
    int  anioPublicacion;
    bool esta_disponible;

    
    Libro(std::string _titulo, std::string _autor, int _anio)
        : titulo(_titulo), autor(_autor), anioPublicacion(_anio), esta_disponible(true) {}

    void mostrarDetallesCompletos() const {
        std::cout<< "_____________________"<< std::endl;
        std::cout<<"Titulo: "<< titulo << std::endl;
        std::cout<<"Autor: "<< autor << std::endl;
        std::cout<<"Anio: "<< anioPublicacion << std::endl;
        std::cout<<"Disponibilidad: "<< (esta_disponible ? "Disponible" : "No disponible");
              
    }
};

class Biblioteca {
private:
    std::vector<Libro> coleccion;

public:
    // 1
    void agregarLibro(const Libro& nuevoLibro) {
        coleccion.push_back(nuevoLibro);
    }

    // 2
    void mostrarInventario() {
        for (const auto& libro : coleccion) {
            libro.mostrarDetallesCompletos();
        }
    }

    // 3
    Libro* buscarLibro(const std::string& tituloBuscado) {
        for (auto& libro : coleccion) {
            if (libro.titulo == tituloBuscado) {
                return &libro;
            }
        }
        return nullptr;
    }

    // 4
    void prestarLibro(const std::string& tituloPrestamo) {
        Libro* libro = buscarLibro(tituloPrestamo);
        if (libro) {
            if (libro->esta_disponible) {
                libro->esta_disponible = false;
                std::cout << "El libro: " << libro->titulo << " ha sido prestado."<<std::endl;
            } else {
                std::cout << "El libro: " << libro->titulo << " ya ha sido prestado a otro usuario."<<std::endl;
            }
        } else {
            std::cout << "El libro '" << tituloPrestamo << "' no existe dentro de la biblioteca."<<std::endl;
        }
    }

    // 5
    void devolverLibro(const std::string& tituloDevolucion) {
        Libro* libro = buscarLibro(tituloDevolucion);
        if (libro) {
            if (!libro->esta_disponible) {
                libro->esta_disponible = true;
                std::cout << "El libro: " << libro->titulo << " ha sido devuelto."<<std::endl;
            } else {
                std::cout << "El libro: " << libro->titulo
                          << " esta disponible (no ha sido prestado)."<<std::endl;
            }
        } else {
            std::cout << "El libro '" << tituloDevolucion
                      << "' no existe dentro de la biblioteca."<<std::endl;
        }
    }
};

//*parte 3
int main() {
    Biblioteca miBiblioteca;
    int opcion = 0;

    // Agregar algunos libros de ejemplo para empezar
    Libro libro1("El Hobbit", "J.R.R. Tolkien", 1937);
    miBiblioteca.agregarLibro(libro1);
    Libro libro2("1984", "George Orwell", 1949); // corrige si quieres
    miBiblioteca.agregarLibro(libro2);
    Libro libro3("La ladrona de libros", "Markus Zusak", 2005);
    miBiblioteca.agregarLibro(libro3);
    Libro libro4("Little Women", "Louisa May Alcott", 1869);
    miBiblioteca.agregarLibro(libro4);

    // Demo rápida
    miBiblioteca.mostrarInventario();

    while (opcion != 5) {
        std::cout << "\n--- BIBLIOTECA DIGITAL ---" << std::endl;
        std::cout << "1. Anadir libro" << std::endl;
        std::cout << "2. Mostrar inventario" << std::endl;
        std::cout << "3. Prestar libro" << std::endl;
        std::cout << "4. Devolver libro" << std::endl;
        std::cout << "5. Salir" << std::endl;
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (opcion == 1) {
            std::string titulo, autor;
            int anio;

            std::cout << "Titulo: ";
            std::getline(std::cin, titulo);
            std::cout << "Autor: ";
            std::getline(std::cin, autor);
            std::cout << "Anio: ";
            while (!(std::cin >> anio)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Anio invalido. Intente de nuevo: ";
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            miBiblioteca.agregarLibro(Libro(titulo, autor, anio));
            std::cout << "Libro agregado correctamente."<<std::endl;
        }
        else if (opcion == 2) {
            miBiblioteca.mostrarInventario();
        }
        else if (opcion == 3) {
            std::string titulo;
            std::cout << "Titulo del libro a prestar: ";
            std::getline(std::cin, titulo);
            miBiblioteca.prestarLibro(titulo);
        }
        else if (opcion == 4) {
            std::string titulo;
            std::cout << "Titulo del libro a devolver: ";
            std::getline(std::cin, titulo);
            miBiblioteca.devolverLibro(titulo);
        }
        else if (opcion == 5) {
            std::cout << "Saliendo del sistema..."<<std::endl;
        }
        else {
            std::cout << "Opcion invalida. Intente de nuevo."<<std::endl;
        }
    } while (opcion != 5);

        
            
    return 0;
};
