//
// Created by carlo on 2023/09/15.
//

#ifndef SEMINARIO_2_FILELOADER_H
#define SEMINARIO_2_FILELOADER_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "math.h"
class FileLoader {

private:
    static FileLoader* cargadorArchivos;
    std::string ruta;
    int tamDatos;
    std::vector<std::vector<double>> ciudades;
    std::vector<std::vector<double>> distancias;

    std::string conjuntoDatos;
    int tamPoblacion;
    int porcentajeInicializacion;
    int numEvaluaciones;
    int kBest; // numero para torneo de selección
    int numeroDeElites;
    int tamGreedyAleatorio;
    std::string tipoCruce;
    int probabilidadCruce;
    int probabilidadMutacion;
    int kWorst;

    std::string tipoSeleccion;
    int tiempoParada;


public:
    FileLoader(const std::string& ruta);
    FileLoader(const FileLoader& obj) = delete;
    static FileLoader* GetInstancia(std::string archivoParametros = "");
    ~FileLoader();
    void ToString();

    const std::string& getRuta() const {
        return ruta;
    }

    const std::vector<std::vector<double>>& getCiudades() const {
        return ciudades;
    }

    const std::vector<std::vector<double>>& getDistancias() const {
        return distancias;
    }

    int getTamDatos() const;

    int getPorcentajeInicializacion() const;

    int getTamPoblacion() const;

    int getNumEvaluaciones() const;

    int getKBest() const;

    int getNumElite() const;

    int getTamGreedyAleatorio() const;

    int getProbabilidadCruce() const;

    std::string getTipoCruce() const;

    int getProbabilidadMutacion() const;

    int getKWorst() const;

    std::string getTipoSeleccion() const;

    int getProbabilidadTernaria() const;

    int getTiempoParada() const;

    std::string getConjuntoDatos() const;

};


#endif //SEMINARIO_2_FILELOADER_H
