//
// Created by yessi on 31/10/2023.
//

#include <set>
#include <algorithm>
#include "Poblacion.h"
#include "FileLoader.h"

Poblacion::Poblacion(std::ofstream& log) {
    this->log = &log;
    inicializarPoblacion();
    elite.resize(FileLoader::GetInstancia()->getNumElite() );
    calcularElite();
}

/*
 * Método que inicializa los individuos de la población tanto de manera aleatoria como mediante un greddy según una probabilidad
 */

void Poblacion::inicializarPoblacion() {

    FileLoader* loader = FileLoader::GetInstancia();
    int rango = (loader->getTamPoblacion() * loader->getPorcentajeInicializacion()) / 100; // establezco el número de elementos que tengo que generar aleatoriamente

    *log << "Inicializo los individuos de la población: " << rango << " número de individuos generados aleatoriamente, " << (loader->getTamPoblacion() - rango) << " generados mediante greedy" << std::endl;

    for(int i = 0; i < rango; i++) { // genero un procentaje de manera aleatoria
        std::vector<int> individuoAleatorio;
        generaInidividuoAleatorio(individuoAleatorio);
        this->individuos.push_back(new Individuo(individuoAleatorio)); // creo un individuo aleatorio y lo inserto al vector de individuos

        if ( i == 0 ) {
            *log << "Ejemplo de primer individuo generado aleaotoriamente: " << std::endl;
            for (int j = 0; j < individuoAleatorio.size(); ++j) {
                *log << individuoAleatorio[j] << " ";
            }
            *log << "" << std::endl;
        }

    }
    for (int i = rango; i < loader->getTamPoblacion() ; i++) { // genero el resto de individuos con un greedy aleatorizado
        std::vector<int> individuoAleatorio;
        generaInidividuoGreedy(individuoAleatorio);
        this->individuos.push_back(new Individuo(individuoAleatorio)); // creo un individuo aleatorio y lo inserto al vector de individuos

        if ( i == rango ) {
            *log << "Ejemplo de primer individuo generado mediante greedy: " << std::endl;
            for (int j = 0; j < individuoAleatorio.size(); ++j) {
                *log << individuoAleatorio[j] << " ";
            }
            *log << "" << std::endl;
        }

    }


}

Poblacion::~Poblacion() {

    for (auto individuo : individuos) {
        delete individuo; // borro cada individuo creado
        individuo = nullptr;
    }

    for (auto elite : elite) {
        delete elite; // borro cada elite creado
        elite = nullptr;
    }

}

void Poblacion::generaInidividuoAleatorio(std::vector<int> &v) {
    FileLoader* loader = FileLoader::GetInstancia();
    std::set<int> conjuntoAux; // conjunto para evitar el duplicado de ciudades
    bool success;
    int limiteIntentos = 0; // atributo para limitar los intentos de generar una ciudad válida aleatoriamente, evitando así que una ciudad no se genere hasta el intento número 10000 por ejemplo
    while (v.size() < loader->getTamDatos()) {

        int numero = ( rand() % (loader->getTamDatos()) ); // genero un número random entre 0-numeroDeCiudades
        if(numero != v.size()) { // evito meter la ciudad x en el índice x
            if (limiteIntentos >= (loader->getTamDatos() / 4) ) { // para evitar que el número que falta tarde en salir mucho, pongo un límite a partir del cual se inserta el número manualmente
                for (int i = 0; i < loader->getTamDatos(); i++) {
                    if ( conjuntoAux.insert(i).second ) { // si la ciudad todavía no se ha seleccionado
                        v.push_back(i);
                        limiteIntentos = 0;
                    }
                }
            }
            success = conjuntoAux.insert(numero).second;
            if (success) { //uso el conjunto como filtro para evitar ducplicados
                v.push_back(numero); // si no se ducplica lo meto al vector
                limiteIntentos = 0;
            } else{
                limiteIntentos++;
            }

        }

    }

}

void Poblacion::generaInidividuoGreedy(std::vector<int> &v) {

    FileLoader* loader = FileLoader::GetInstancia();
    std::vector<bool> marcajeCiudades(loader->getTamDatos());

    for (int i = 0; i < loader->getTamDatos(); i++) {
        marcajeCiudades[i] = false;
    }

    int centinela = 0;
    int ciudadActual = (rand() % (loader->getTamDatos()) ); //genero la primera ciudad de manera aleatoria
    v.push_back(ciudadActual); // la meto en la posición 0
    marcajeCiudades[ciudadActual] = true;

    while (v.size() < loader->getTamDatos()) {

        std::vector<std::pair<double, int>> ciudadesContiguas;

        for (int i = 0; i < loader->getDistancias()[ciudadActual].size(); i++) { // compruebo las distancias desde la ciudad actual a las otras
            ciudadesContiguas.push_back({loader->getDistancias()[ciudadActual][i], i});
        }
        std::sort(ciudadesContiguas.begin(),ciudadesContiguas.end()); // ordeno las ciudades de menor a mayor distancia

        do{
            int randomGreedy = (rand() % (loader->getTamGreedyAleatorio())); // me quedo con el n mejor del rango del greedy aleatorio, basicamente halo la aleatorización sobre el greedy
            int ciudadElegida = ciudadesContiguas[randomGreedy].second; // la 'randomGreedy' ciudad más cercana
            if(  !marcajeCiudades[ciudadElegida] && ciudadElegida != v.size()) { // compruebo que esa ciudad no esté ya seleccionada y que no se mete x en v[x]
                ciudadActual = ciudadElegida;
                v.push_back(ciudadActual); // inserto la ciudad al vector solución
                marcajeCiudades[ciudadElegida] = true; // marco la ciudad como ya elegida
                centinela = -1;
            }else{
                centinela++;
            }

        }while( centinela != -1 && centinela < (loader->getTamGreedyAleatorio()*5) ) ; // genero un número 'n' de manera aleatoria, si la 'n' ciudad más cercana está ya elegida vuelvo a generar otro 'n',
                                                                                        // el bucle se repite hasta que encuentre una ciudad sin seleccionar o hasta que de un número de vueltas determinado.

        if (centinela != -1) { // si no he podido encontrar una ciudad no seleccionada previamente en el do-while, meto la primera ciudad sin seleccionar que encuentre
            for (int i = 0; i < marcajeCiudades.size(); i++) {
                if ( !marcajeCiudades[i] && i != v.size()) { // busco y me quedo con la primera ciudad no elegida
                    ciudadActual = i;
                    marcajeCiudades[i] = true;
                    v.push_back(i);
                }else{
                    if (i == loader->getTamDatos() - 1) { // casuistica especial, solo falta por meter el 129 y la única posición libre es la 129
                        ciudadActual = i;
                        marcajeCiudades[i] = true;
                        v.push_back(i);
                        std::swap(v[loader->getTamDatos() - 1],v[loader->getTamDatos() - 2]); // en este caso meto la 129 en v[129] y hago un swap para que no este en la misma posición
                    }
                }
            }
        }

        centinela = 0;

    }

}

void Poblacion::calcularElite() {

    *log << "\nCalculo los élites de la población" << std::endl;
    FileLoader* loader = FileLoader::GetInstancia();
    auto aux = individuos; // vector auxiliar para no trabajar directamente sobre los individuos
    std::sort(aux.begin(),aux.end(), comparadorIndividuos); // ordeno de menor a mayor por costes
    for (int i = 0; i < loader->getNumElite(); i++) {

        this->elite[i] = ( new Individuo(*aux[i]) ); // elijo tantos elementos como elites tenga el algoritmo
        *log << "Elite " << i + 1 << " -> " << elite[i]->getCosteAsociado() << ", vector: " << std::endl;
        for (int j = 0; j < elite[i]->getVIndividuo().size(); ++j) {
            *log << elite[i]->getVIndividuo()[j] << " ";
        }
        std::cout << "Elite -> "<< i + 1 << " -> "  << elite[i]->getCosteAsociado() << std::endl;
    }


}

std::vector<Individuo *> &Poblacion::getIndividuos()  {
    return individuos;
}

/*
 * Función que me dice cual de los dos individuos es más pequeño
 * @param uno, dos: dos individuos
 * @return: boleano indicando si el individuo 'uno' es menor que el 'dos'
 */
bool Poblacion::comparadorIndividuos(Individuo *uno, Individuo *otro) {
    if( uno->getCosteAsociado() < otro->getCosteAsociado() ) return true;
    return false;
}

const std::vector<Individuo *> &Poblacion::getElite() const {
    return elite;
}

/*
 * Modifica el vector de individuos por el pasado por parámetro
 * @param individuos: vector por el que es sustituido
 * @post: además del cambio de individuos se calcula los élites y el coste de aquellos individuos que han sido modificados
 *
 */
void Poblacion::setIndividuos(const std::vector<Individuo *> &individuos) {
    this->individuos = individuos;

    for (int i = 0; i < this->individuos.size(); i++) {
        if( !this->individuos[i]->isEvaluado() ) { // si no está evaluado calculo su coste
            this->individuos[i]->setCoste();
        }
    }

    calcularElite(); // calculo los nuevos élites


}

