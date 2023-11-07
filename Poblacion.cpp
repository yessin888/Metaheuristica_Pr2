//
// Created by yessi on 31/10/2023.
//

#include <set>
#include "Poblacion.h"
#include "FileLoader.h"
#include "CompareGreedy.h"

Poblacion::Poblacion() {
    inicializarPoblacion();
    elite.resize(FileLoader::GetInstancia()->getNumElite() );
    calcularElite();
}

Poblacion::Poblacion(int size) {

    individuos.resize(size);

}

void Poblacion::inicializarPoblacion() {

    FileLoader* loader = FileLoader::GetInstancia();
    int rango = (loader->getTamPoblacion() * loader->getPorcentajeInicializacion()) / 100; // establezco el número de elementos que tengo que generar aleatoriamente

    for(int i = 0; i < rango; i++) { // genero un procentaje de manera aleatoria
        std::vector<int> individuoAleatorio;
        generaInidividuoAleatorio(individuoAleatorio);
        this->individuos.push_back(new Individuo(individuoAleatorio)); // creo un individuo aleatorio y lo inserto al vector de individuos
    }
    for (int i = rango; i < loader->getTamPoblacion() ; i++) { // genero el resto de individuos con un greedy aleatorizado
        std::vector<int> individuoAleatorio;
        generaInidividuoGreedy(individuoAleatorio);
        this->individuos.push_back(new Individuo(individuoAleatorio)); // creo un individuo aleatorio y lo inserto al vector de individuos
    }

    //todo calcular elite y guardarlos por copia


}

Poblacion::~Poblacion() {

    for (auto individuo : individuos) {
        delete individuo; // borro cada individuo creado
        individuo = nullptr;
    }

}

void Poblacion::generaInidividuoAleatorio(std::vector<int> &v) {
    FileLoader* loader = FileLoader::GetInstancia();
    std::set<int> conjuntoAux;
    bool success;

    while (v.size() < loader->getTamDatos()) {

        int numero = ( rand() % (loader->getTamDatos()) ); // genero un número random entre 0-numeroDeCiudades
        if(numero != v.size()) { // evito meter la ciudad x en el índice x

            success = conjuntoAux.insert(numero).second;
            if (success) { //uso el conjunto como filtro para evitar ducplicados
                v.push_back(numero); // si no se ducplica lo meto al vector
            }

        }

    }

}

void Poblacion::generaInidividuoGreedy(std::vector<int> &v) {

    FileLoader* loader = FileLoader::GetInstancia();
    int numero = ( rand() % (loader->getTamDatos()) ); //genero la primera ciudad de manera aleatoria
    v.push_back(numero); // la meto en la posición 0

    while (v.size() < loader->getTamDatos()) {

        // Usar un min-heap para mantener un seguimiento de los n valores más pequeños
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, CompareGreedy> distanciasHeap;

        for (int i = 0; i <
                        loader->getDistancias()[numero].size(); i++) { // compruebo las distancias desde la ciudad actual a las otras
            distanciasHeap.push({i,
                                 loader->getDistancias()[numero][i]}); // guardo en el heap la ciudad destino y la distancia de la ciudad origen a la de destino
        }

        int randomGreedy = (rand() %
                            (loader->getTamGreedyAleatorio())); // me quedo con el n mejor del rango del greedy aleatorio, basicamente halo la aleatorización sobre el greedy

        for (int i = 0; i < distanciasHeap.size(); ++i) {
            if (i == randomGreedy) { // me quedo con la ciudad pertinente
                numero = distanciasHeap.top().first;
                distanciasHeap.pop();
                break;
            }
            distanciasHeap.pop();
        }
        v.push_back(numero); // inserto la ciudad al vector solución
        distanciasHeap = std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, CompareGreedy>(); // vuelvo a inicializar el heap, basicamente vacio su contenido

    }

}

const std::vector<Individuo *> &Poblacion::getIndividuos() const {
    return individuos;
}

void Poblacion::calcularElite() {

}
