//
// Created by carlo on 2023/09/15.
//

#include <sstream>
#include "FileLoader.h"

FileLoader* FileLoader::cargadorArchivos = nullptr;

FileLoader::FileLoader(const std::string& ruta):ruta(ruta.substr(0, ruta.find('.'))) {


    std::string linea;
    //std::ifstream archivo(ruta);


    std::ifstream archivo;
    archivo.open(ruta);

    if (!archivo) {
        throw std::invalid_argument("Ups, no se ha podido abrir el archivo xD!");
    }

    // Leemos el archivo y pasamos al stream(mystring) su contenido
    std::string mystring, parametro, valor;
    if (archivo.is_open()) {

        while (getline(archivo, mystring) && !mystring.empty()) {
            std::istringstream linea(mystring);
            getline(linea, parametro, '=');
            getline(linea, valor);

            if(parametro == "conjuntoDatos") {
                conjuntoDatos = valor;
            }
            if(parametro == "porcentajeInicializacion") {
                porcentajeInicializacion = std::stoi(valor);
            }
            if(parametro == "tamPoblacion") {
                tamPoblacion = std::stoi(valor);
            }
            if(parametro == "numEvaluaciones") {
                numEvaluaciones = std::stoi(valor);
            }
            if(parametro == "kBest") {
                kBest = std::stoi(valor);
            }
            if(parametro == "numDeElites") {
                numeroDeElites = std::stoi(valor);
            }
            if(parametro == "tamGreedyAleatorio") {
                tamGreedyAleatorio = std::stoi(valor);
            }
            if(parametro == "tipoCruce") {
                tipoCruce = valor;
            }
            if(parametro == "probabilidadCruce") {
                probabilidadCruce = std::stoi(valor);
            }
            if(parametro == "probabilidadMutacion") {
                probabilidadMutacion = std::stoi(valor);
            }
            if(parametro == "kWorst") {
                kWorst = std::stoi(valor);
            }

        }

    }

    // Cierro archivo
    if (archivo.is_open()) {
        archivo.close();
    }

    archivo.open(this->conjuntoDatos);

    if (!archivo.is_open()) {
        throw std::invalid_argument("Ups, no se ha podido abrir el archivo de los datos xD");
    }

    while (std::getline(archivo, linea)) {
        if (linea.find("DIMENSION") == 0) {
            tamDatos = std::stoi(linea.substr(linea.find(":") + 1));
            ciudades.resize(tamDatos, std::vector<double>(2));
        }

        if (linea.find("NODE_COORD_SECTION") == 0) {
            int i = 0;
            while (std::getline(archivo, linea) && linea != "EOF") {
                double x, y;
                sscanf(linea.c_str(), "%*d %lf %lf", &x, &y);
                ciudades[i][0] = x;
                ciudades[i][1] = y;
                i++;
            }
        }
    }
    if(archivo.is_open()) {
        archivo.close();
    }


    distancias.resize(ciudades.size(), std::vector<double>(ciudades.size(), 0.0));

    for (int i = 0; i < ciudades.size(); i++) {
        for (int j = i; j < ciudades.size(); j++) {
            if (i == j) {
                distancias[i][j] = std::numeric_limits<double>::infinity();
            } else {
                double dx = ciudades[i][0] - ciudades[j][0];
                double dy = ciudades[i][1] - ciudades[j][1];
                distancias[i][j] = distancias[j][i] = sqrt(dx * dx + dy * dy);
            }
        }
    }


}

FileLoader::~FileLoader() {

}

FileLoader* FileLoader::GetInstancia(std::string archivoParametros) {
    // Creamos instancia del Singleton en caso de no existir y devolvemos la existente en caso de haber sido previamente creada
    if (!cargadorArchivos) {
        cargadorArchivos = new FileLoader(archivoParametros);
        return cargadorArchivos;
    }
    else {
        return cargadorArchivos;
    }
}



void FileLoader::ToString() {
    std::cout << "tam datos:" << tamDatos << std::endl;
    std::cout << "Datos de ciudades:" << std::endl;
    for (const auto& fila : ciudades) {
        std::cout << std::to_string(fila[0]) << " " << std::to_string(fila[1]) << std::endl;
    }
    std::cout << "Datos de distancias:" << std::endl;
    for (const auto& fila : distancias) {
        for (const double valor : fila) {
            std::cout << std::to_string(valor) << " ";
        }
        std::cout << std::endl;
    }
}

int FileLoader::getTamDatos() const {
    return tamDatos;
}

int FileLoader::getPorcentajeInicializacion() const {
    return porcentajeInicializacion;
}

int FileLoader::getTamPoblacion() const {
    return tamPoblacion;
}

int FileLoader::getNumEvaluaciones() const {
    return numEvaluaciones;
}

int FileLoader::getKBest() const {
    return kBest;
}

int FileLoader::getNumElite() const {
    return numeroDeElites;
}

int FileLoader::getTamGreedyAleatorio() const {
    return tamGreedyAleatorio;
}

int FileLoader::getProbabilidadCruce() const {
    return probabilidadCruce;
}

std::string FileLoader::getTipoCruce() const {
    return tipoCruce;
}

int FileLoader::getProbabilidadMutacion() const {
    return probabilidadMutacion;
}

int FileLoader::getKWorst() const {
    return kWorst;
}



