//
// Created by yessi on 31/10/2023.
//

#include <chrono>
#include <set>
#include "EvolutivoGeneracional.h"
#include "FileLoader.h"

EvolutivoGeneracional::EvolutivoGeneracional(): poblacion(new Poblacion()) {
    // gracias a los constructores y las relaciones entre clases, mediante este constructor consigo inicializar los individuos de la
    //  población, evaluarlos y quedarme con los élite


}

EvolutivoGeneracional::~EvolutivoGeneracional() {
    delete poblacion;
    poblacion = nullptr;
}

void EvolutivoGeneracional::executeEvolutivo() {

    FileLoader* loader = FileLoader::GetInstancia();
    int numEvaluaciones = 0;
    auto startTime = std::chrono::high_resolution_clock::now(); // inicialización tiempo inicio
    auto endTime = std::chrono::high_resolution_clock::now();   // inicialización tiempo fin
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); //inicialización tiempo transcurrido
    std::vector<Individuo*> individuosSeleccionados;

    while( numEvaluaciones < loader->getNumEvaluaciones() && elapsed_seconds.count() > 60) {

        while(individuosSeleccionados.size() < loader->getTamPoblacion()) { // hago torneo hasta seleccionar todos los individuos
            int torneoWinner = torneo(); // hago el torneo
            individuosSeleccionados.push_back(poblacion->getIndividuos()[torneoWinner]); // almaceno los individuos seleccionados
        }

        //todo remplazamiento a lo mejor tiene que ser como operador= en composición o algo asi con la memoria

        endTime = std::chrono::high_resolution_clock::now();
        elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); // actualizo el número de segundos que han transcurrido desde el inicio

    }




}

int EvolutivoGeneracional::torneo() {

    FileLoader *loader = FileLoader::GetInstancia();
    std::set<int> individuosSeleccionados;
    int mejorCoste = INT_MAX;
    int mejorIndividuo = 0;

    while (individuosSeleccionados.size() < loader->getKBest()) {

        int numero = (rand() % (loader->getTamPoblacion())); // elijo un individuo de la población de manera aleatoria
        individuosSeleccionados.insert(numero).second; // evito duplicar los individuos con el conjunto

    }

    for( std::set<int>::iterator it = individuosSeleccionados.begin(); it != individuosSeleccionados.end(); ++it ) {

        int i = *it;

        if( poblacion->getIndividuos()[i]->getCosteAsociado() < mejorCoste ) { // me quedo con el individuo con mejor fitness
            mejorCoste = poblacion->getIndividuos()[i]->getCosteAsociado();
            mejorIndividuo = i;
        }

    }

    return mejorIndividuo; // devuelvo el ganador del torneo, el de mejor fiteness

}

/*
 * Genera un hijo a partir del padre1 y las posiciones seleccionadas en el padre2
 * @pre hijo inicializado como padre1
 * 
 */

void EvolutivoGeneracional::cruceOX2(Individuo *padre1, Individuo *padre2, Individuo *hijo) {

    int centinela = 0;
    std::vector<int> elementosSeleccionados;
    int i = 0;
    bool fin = true;
    //TODO selecciono posiciones random de padre2 e inserto en 'elementosSeleccionados' padre2[i]

    while( i < hijo->getVIndividuo().size() && fin ) {

        for( int j = 0; j < elementosSeleccionados.size(); i++) { // recorro los elementos seleccionados
            if( elementosSeleccionados[j] == hijo->getVIndividuo()[i] ) { // compruebo si es uno de los elementos seleccionados de manera aleatoria
                hijo->getVIndividuo()[i] = elementosSeleccionados[centinela]; // pongo en la respectiva posición del hijo el elemento correspondiente del segundo padre( el elemento pertenece a 'elementosSeleccionados')
                centinela++; // actualizo para indicar que ahora en vez de sustituir el elemento que aparece por 'centinela' vez en el padre, sea ahora el que aparece por 'centinela+1' vez
            }
            if(centinela == elementosSeleccionados.size()) { // si se han sustituido ya todos los elementos de 'elementosSeleccionados' acabo
                fin = false;
                break;
            }
        }
        i++;

    }

}
