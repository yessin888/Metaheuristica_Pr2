//
// Created by Yessin on 19/11/2023.
//

#include <chrono>
#include "TSP.h"
#include "FileLoader.h"
#include <set>
#include <algorithm>

TSP::TSP(): poblacion(new Poblacion()) {

}

TSP::~TSP() {
    delete poblacion;
    poblacion = nullptr;
}

void TSP::executeTSP() {

    FileLoader* loader = FileLoader::GetInstancia();
    int numEvaluaciones = 0;
    auto startTime = std::chrono::high_resolution_clock::now(); // inicialización tiempo inicio
    auto endTime = std::chrono::high_resolution_clock::now();   // inicialización tiempo fin
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); //inicialización tiempo transcurrido
    std::vector<Individuo*> individuosSeleccionados;

    while( numEvaluaciones < loader->getNumEvaluaciones() && elapsed_seconds.count() < 60 ) {

        Individuo* padre = nullptr;
        Individuo* aleatorio1 = nullptr;
        Individuo* aleatorio2 = nullptr;
        Individuo* objetivo = nullptr;
        int secuenciaPadre = 0;

        //SELECCIÓN
        while(individuosSeleccionados.size() < loader->getTamPoblacion()) { // genero todos los individuos resultado de aplicar la recombinación ternaria sobre los individuos seleccionados

            padre = ( new Individuo(*poblacion->getIndividuos()[secuenciaPadre]) ); // asigno el padre de manera secuencial//NOTA -> el padre lo hago por copia pq al generar el nuevo individuo hago un swap sobre él, de modo que al hacer la segunda iteración y tomar valores aleatorios puedo tomar el padre ya modificado
            seleccionIndividuos(aleatorio1, aleatorio2, objetivo); // incializo los aleatorios y el objetivo según el tipo de selección

            // RECOMBINACIÓN TERNARIA
            Individuo* IndividuoGenerado = recombinacionTernaria(padre, aleatorio1, aleatorio2, objetivo); // genero el nuevo individuo
            individuosSeleccionados.push_back(IndividuoGenerado); // almaceno el nuevo individuo generado

            delete padre;
            secuenciaPadre++; // actualizo el índice que indica el padre a tomar
        }

        //std::sort(individuosSeleccionados.begin(),individuosSeleccionados.end(),Poblacion::comparadorIndividuos);
        //std::cout << "->>>> " << individuosSeleccionados[0]->getCosteAsociado() << std::endl;

        // REMPLAZAMIENTO
        for (int i = 0; i < poblacion->getIndividuos().size(); ++i) {
            individuosSeleccionados[i]->setCoste();
            if( individuosSeleccionados[i]->getCosteAsociado() < poblacion->getIndividuos()[i]->getCosteAsociado() ) { // si el hijo tiene menor coste que el padre sustituyo
                poblacion->getIndividuos()[i] = individuosSeleccionados[i];
            }
        }

        std::sort(poblacion->getIndividuos().begin(),poblacion->getIndividuos().end(),Poblacion::comparadorIndividuos);
        std::cout << "coste: " << poblacion->getIndividuos()[0]->getCosteAsociado() << std::endl;

        endTime = std::chrono::high_resolution_clock::now();
        elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); // actualizo el número de segundos que han transcurrido desde el inicio
        numEvaluaciones++;
    }

}

void TSP::seleccionIndividuos(Individuo *&aleatorio1, Individuo *&aleatorio2, Individuo *&objetivo) {

    FileLoader* loader = FileLoader::GetInstancia();

    if(loader->getTipoSeleccion() == "EDA"){

        aleatorio1 = poblacion->getIndividuos()[(rand() % (loader->getTamPoblacion()))];
        aleatorio2 = poblacion->getIndividuos()[(rand() % (loader->getTamPoblacion()))];

    }else{
        if (loader->getTipoSeleccion() == "EDB"){
            int torneoWinner = torneo(); // hago el torneo para el aleatorio1
            aleatorio1 = poblacion->getIndividuos()[torneoWinner];
            int torneoWinner2 = torneo(); // hago el torneo para el aleatorio2
            aleatorio2 = poblacion->getIndividuos()[torneoWinner2];

        } else{
            throw std::invalid_argument("El tipo de selección indicado para el algoritmo es incorrecot");
        }

    }

    int torneoWinner = torneo(); // hago el torneo
    objetivo = poblacion->getIndividuos()[torneoWinner]; // inicilizo 'objetivo' al ganador




}

Individuo* TSP::recombinacionTernaria(Individuo *padre, Individuo *aleatorio1, Individuo *aleatorio2, Individuo* objetivo) {

    FileLoader* loader = FileLoader::GetInstancia();

    Individuo* nuevoIndividuo = ( new Individuo(*aleatorio1) );
    int posicionSwap = (rand() % (loader->getTamPoblacion() - 1)); // como el swap lo haré entre 'posiciónSwap' y 'posicionSwap' + 1, limito el rango de aleatorio a 'n' - 1, para no salirme de rango
    int elemento1 = posicionSwap, elemento2 = posicionSwap + 1; // elementos correspondientes al swap del padre
    int i = 0, centinela = 0, indiceAleatorio1 = 0, indiceAleatorio2 = 0, elementoAleatorio2 = 0; // indices y elementos correspondientes al vector aleatorio 1 o 2

    std::swap(padre->getVIndividuo()[posicionSwap],padre->getVIndividuo()[posicionSwap+1]); // swap de las posiciones del padre

    while ( centinela <= 1 && i < aleatorio1->getVIndividuo().size()) {

        if (aleatorio1->getVIndividuo()[i] == padre->getVIndividuo()[elemento1]) {
            indiceAleatorio1 = i; // busco la posición correspondiente al 'elemento1' en el vector 'aleatorio1'
            centinela++;
        }
        if (aleatorio2->getVIndividuo()[i] == padre->getVIndividuo()[elemento2]) {
            indiceAleatorio2 = i; // busco la posición correspondiente al 'elemento2' en el vector 'aleatorio2'
            elementoAleatorio2 = aleatorio2->getVIndividuo()[i];
            centinela++;
        }

        i++;

    }

    for (int j = 0; j < aleatorio1->getVIndividuo().size(); j++) {
        if ( aleatorio1->getVIndividuo()[j] == padre->getVIndividuo()[indiceAleatorio1] ) { // busco la posición en el 'aleatorio1' correspondiente al elemento del padre
            int indiceAux = j;
            std::swap(nuevoIndividuo->getVIndividuo()[j], nuevoIndividuo->getVIndividuo()[indiceAleatorio1]);
            break;
        }
    }

    for (int j = 0; j < nuevoIndividuo->getVIndividuo().size(); j++) {
        if ( nuevoIndividuo->getVIndividuo()[j] == elementoAleatorio2 ) { // busco la posición en el 'nuevoIndividuo' correspondiente al elemento 'elementoAleatorio2'
            std::swap(nuevoIndividuo->getVIndividuo()[j], nuevoIndividuo->getVIndividuo()[indiceAleatorio2]);
            break;
        }
    }

    //Una vez que he obtenido ya el nuevo individuo le aplico el operador OX2 para obtener el hijo definitivo
    //nuevoIndividuo->setCoste();
   // std::cout << "coste antes de mutar: " << nuevoIndividuo->getCosteAsociado() << std::endl;

    Individuo* hijo1 = new Individuo(*nuevoIndividuo);
    this->cruceOX2(nuevoIndividuo, objetivo,hijo1); // aplico el OX2
    delete nuevoIndividuo;


    return hijo1;
}

int TSP::torneo() {

    FileLoader *loader = FileLoader::GetInstancia();
    std::set<int> individuosSeleccionados;
    double mejorCoste = INT_MAX;  // coste mejor individuo
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

void TSP::cruceOX2(Individuo *padre1, Individuo *padre2, Individuo *hijo) {

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

        for( int j = 0; j < elementosSeleccionados.size(); j++) { // recorro los elementos seleccionados
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


void TSP::cruceMOC(Individuo *padre1, Individuo *padre2, Individuo *hijo1, Individuo* hijo2) {

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

        for(int j = 0; j < parteDerechaPadre2.size(); j++) { // recorro los elementos seleccionados
            if(parteDerechaPadre2[j] == hijo1->getVIndividuo()[i] ) { // compruebo si el elemento del hijo pertenece a la parte derecha del padre2
                hijo1->getVIndividuo()[i] = parteDerechaPadre2[centinela]; // en caso de ser así sustituyo dicho elemento por su correspondiente
                centinela++; // actualizo para indicar que ahora en vez de sustituir el elemento que aparece por 'centinela' vez en la derecha del padre2, sea ahora el que aparece por 'centinela+1' vez
            }
            if(centinela == parteDerechaPadre2.size()) { // si se han sustituido ya todos los elementos de 'parteDerechaPadre2' acabo
                fin = false;
                break;
            }
        }

        for(int j = 0; j < parteDerechaPadre1.size(); j++) { // recorro los elementos seleccionados
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

