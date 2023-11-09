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

    while( numEvaluaciones < loader->getNumEvaluaciones() && elapsed_seconds.count() > 60 ) {
        //todo comprobar que no he metido la ciudad x en v[x], sobre todo en poblacion al generar aleatoriamente y greedy
        //SELECCIÓN
        while(individuosSeleccionados.size() < loader->getTamPoblacion()) { // hago torneo hasta seleccionar todos los individuos
            int torneoWinner = torneo(); // hago el torneo
            individuosSeleccionados.push_back(poblacion->getIndividuos()[torneoWinner]); // almaceno los individuos seleccionados
        }
        //CRUCE
        cruce(individuosSeleccionados);
        //MUTACIÓN
        mutacion(individuosSeleccionados);
        //REMPLAZAMIENTO





        //todo remplazamiento a lo mejor tiene que ser como operador= en composición o algo asi con la memoria

        endTime = std::chrono::high_resolution_clock::now();
        elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); // actualizo el número de segundos que han transcurrido desde el inicio
        numEvaluaciones++;
    }




}

int EvolutivoGeneracional::torneo() {

    FileLoader *loader = FileLoader::GetInstancia();
    std::set<int> individuosSeleccionados;
    int mejorCoste = INT_MAX;  // coste mejor individuo
    int mejorIndividuo = 0; // mejor individuo

    while (individuosSeleccionados.size() < loader->getKBest()) {

        int numero = (rand() % (loader->getTamPoblacion())); // elijo un individuo de la población de manera aleatoria
        individuosSeleccionados.insert(numero); // evito duplicar los individuos con el conjunto

    }

    for( std::set<int>::iterator it = individuosSeleccionados.begin(); it != individuosSeleccionados.end(); ++it ) {

        int i = *it; // índice individuo de la población
        if( poblacion->getIndividuos()[i]->getCosteAsociado() < mejorCoste ) { // me quedo con el individuo con mejor fitness
            mejorCoste = poblacion->getIndividuos()[i]->getCosteAsociado();
            mejorIndividuo = i;
        }

    }

    return mejorIndividuo; // devuelvo el ganador del torneo, el de mejor fiteness

}

void EvolutivoGeneracional::cruce(std::vector<Individuo*> &IndividuosACruzar) {

    FileLoader* loader = FileLoader::GetInstancia();
    Individuo* padre1 = nullptr;
    Individuo* padre2 = nullptr;
    Individuo* hijo1 = nullptr;
    Individuo* hijo2 = nullptr;
    for (int i = 0; i < IndividuosACruzar.size() - 1; i++) {

        //todo considerar poblaciones de tamaño impar
        padre1 = IndividuosACruzar[i];
        padre2 = IndividuosACruzar[i+1];
        int random = (rand() % (10)); // aleatorio para simular 70%

        if( random < (int)(loader->getProbabilidadCruce() / 10) ) {
            hijo1 = new Individuo(*padre1);
            hijo2 = new Individuo(*padre2);
            if( loader->getTipoCruce() == "OX2" ) {
                cruceOX2(padre1,padre2,hijo1);
                cruceOX2(padre2,padre1,hijo2);
                padre1 = hijo1;
                padre1->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
                padre2 = hijo2;
                padre2->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
            }else{
                if( loader->getTipoCruce() == "MOC" ){
                    cruceMOC(padre1,padre2,hijo1,hijo2);
                    padre1 = hijo1;
                    padre1->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
                    padre2 = hijo2;
                    padre2->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
                }else{
                    throw std::invalid_argument("El algoritmo de cruce especificado en el paramFile es incorrecto, recuerda que debe ser \"OX2\" o \"MOC\"");
                }
            }

        }

    }

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

    for (int i = 0; i < padre2->getVIndividuo().size() ; i++) {
        int numero = (rand() % (10)); // aleatorio de [0-9]
        if( numero < 5 ) {
            elementosSeleccionados.push_back( padre2->getVIndividuo()[i] ); // elemento se añade a la lista
        }
    }

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

/*
 * @pre: el hijo debe ser inicializado como el padre1
 *
 * */

void EvolutivoGeneracional::cruceMOC(Individuo *padre1, Individuo *padre2, Individuo *hijo1, Individuo* hijo2) {

    FileLoader* loader = FileLoader::GetInstancia();
    int centinela = 0, centinela2 = 0;
    std::vector<int> parteDerechaPadre2;
    std::vector<int> parteDerechaPadre1;
    int i = 0;
    bool fin = true;

    int posicionSeparacion = (rand() % (padre1->getVIndividuo().size())); // establezco la posición de separación

    for (int j = posicionSeparacion; j < padre1->getVIndividuo().size(); j++) {
        parteDerechaPadre1.push_back(padre1->getVIndividuo()[j]); // relleno los elementos correspondiente a laparte derecha
        parteDerechaPadre2.push_back(padre2->getVIndividuo()[j]);
    }

    while( i < hijo1->getVIndividuo().size() && fin ) {

        for(int j = 0; j < parteDerechaPadre2.size(); i++) { // recorro los elementos seleccionados
            if(parteDerechaPadre2[j] == hijo1->getVIndividuo()[i] ) { // compruebo si el elemento del hijo pertenece a la parte derecha del padre2
                hijo1->getVIndividuo()[i] = parteDerechaPadre2[centinela]; // en caso de ser así sustituyo dicho elemento por su correspondiente
                centinela++; // actualizo para indicar que ahora en vez de sustituir el elemento que aparece por 'centinela' vez en la derecha del padre2, sea ahora el que aparece por 'centinela+1' vez
            }
            if(centinela == parteDerechaPadre2.size()) { // si se han sustituido ya todos los elementos de 'parteDerechaPadre2' acabo
                fin = false;
                break;
            }
        }

        for(int j = 0; j < parteDerechaPadre1.size(); i++) { // recorro los elementos seleccionados
            if(parteDerechaPadre1[j] == hijo2->getVIndividuo()[i] ) { // compruebo si el elemento del hijo pertenece a la parte derecha del padre1
                hijo2->getVIndividuo()[i] = parteDerechaPadre1[centinela2]; // en caso de ser así sustituyo dicho elemento por su correspondiente
                centinela2++; // actualizo para indicar que ahora en vez de sustituir el elemento que aparece por 'centinela' vez en la derecha del padre1, sea ahora el que aparece por 'centinela+1' vez
            }
            if(centinela2 == parteDerechaPadre1.size()) { // si se han sustituido ya todos los elementos de 'parteDerechaPadre1' acabo
                fin = false;
                break;
            }
        }


        i++;

    }

}

void EvolutivoGeneracional::mutacion(std::vector<Individuo *> &IndividuosAMutar) {

    FileLoader* loader = FileLoader::GetInstancia();

    for (int i = 0; i < IndividuosAMutar.size(); i++) {
        int random = (rand() % (10)); // simular probabilidad
        if( random < (int)(loader->getProbabilidadMutacion() / 10) ) {
            int posicionRandom1 = (rand() % (loader->getTamDatos())); // primera posición del individuo para el 2-opt
            int posicionRandom2 = (rand() % (loader->getTamDatos())); // segunda posición del individuo para el 2-opt
            std::swap(IndividuosAMutar[i]->getVIndividuo()[posicionRandom1],IndividuosAMutar[i]->getVIndividuo()[posicionRandom2]); // aplico 2-opt
            IndividuosAMutar[i]->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
        }

    }

}
