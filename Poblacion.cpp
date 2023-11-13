//
// Created by yessi on 31/10/2023.
//

#include <set>
#include <algorithm>
#include "Poblacion.h"
#include "FileLoader.h"

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
    std::vector<bool> marcajeCiudades(loader->getTamDatos());

    for (int i = 0; i < loader->getTamDatos(); i++) {
        marcajeCiudades.push_back(false);
    }

    int centinela = 0;
    int ciudadActual = (rand() % (loader->getTamDatos()) ); //genero la primera ciudad de manera aleatoria
    v.push_back(ciudadActual); // la meto en la posición 0
    marcajeCiudades[ciudadActual] = true;

    while (v.size() < loader->getTamDatos()) {

        std::vector<std::pair<double, int>> ciudadesContiguas;

        for (int i = 0; i < loader->getDistancias()[ciudadActual].size(); i++) { // compruebo las distancias desde la ciudad actual a las otras
            ciudadesContiguas.push_back({loader->getDistancias()[ciudadActual][i], i}); // guardo en el heap la ciudad destino y la distancia de la ciudad origen a la de destino
        }
        std::sort(ciudadesContiguas.begin(),ciudadesContiguas.end()); // ordeno las ciudades de manor a mayor distancia

        do{

            int randomGreedy = (rand() % (loader->getTamGreedyAleatorio())); // me quedo con el n mejor del rango del greedy aleatorio, basicamente halo la aleatorización sobre el greedy
            int ciudadElegida = ciudadesContiguas[randomGreedy].second; // la 'randomGreedy' ciudad más cercana
            if(  !marcajeCiudades[ciudadElegida] ) { // compruebo que esa ciudad no esté ya seleccionada
                ciudadActual = ciudadesContiguas[randomGreedy].second;
                v.push_back(ciudadActual); // inserto la ciudad al vector solución
                marcajeCiudades[ciudadElegida] = true;
                centinela = -1;
            }else{
                centinela++;
            }

        }while( centinela != -1 && centinela < (loader->getTamGreedyAleatorio()*5) ) ; // genero un número 'n' de manera aleatoria, si la 'n' ciudad más cercana está ya elegida vuelvo a generar otro 'n',
                                                                                        // el bucle se repite hasta que encuentre una ciudad sin seleccionar o hasta que de un número de vueltas determinado.
        //todo condición para dejar de probar los random, considerar otros valores

        if (centinela != -1) { // si no he podido encontrar una ciudad no seleccionada previamente en el do-while, meto la primera ciudad sin seleccionar que encuentre
            for (int i = 0; i < marcajeCiudades.size(); i++) {
                if ( !marcajeCiudades[i] ) {
                    ciudadActual = i;
                    marcajeCiudades[i] = true;
                    v.push_back(i);
                }
            }
        }

        centinela = 0;

    }

}

void Poblacion::calcularElite() {

    FileLoader* loader = FileLoader::GetInstancia();
    //ordenar(this->individuos);
    std::sort(individuos.begin(),individuos.end(), comparadorIndividuos); // ordeno de menor a mayor por costes
    for (int i = 0; i < loader->getNumElite(); i++) {
        this->elite[i] = ( new Individuo(*individuos[i]) ); // elijo tantos elementos como elites tenga el algoritmo
    }

}

const std::vector<Individuo *> &Poblacion::getIndividuos() const {
    return individuos;
}

bool Poblacion::comparadorIndividuos(Individuo *uno, Individuo *otro) {
    if( uno->getCosteAsociado() < otro->getCosteAsociado() ) return true;
    return false;
}

const std::vector<Individuo *> &Poblacion::getElite() const {
    return elite;
}



