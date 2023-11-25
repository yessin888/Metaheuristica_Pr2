//
// Created by Yessin on 19/11/2023.
//

#include <chrono>
#include "EvolutivoDiferencial.h"
#include "FileLoader.h"
#include <set>
#include <algorithm>

EvolutivoDiferencial::EvolutivoDiferencial(): poblacion() {
    FileLoader *loader = FileLoader::GetInstancia();

    log.open("RESULTADOS/EvolutivoDiferencial.txt" ); //abro fichero Log
    log << "Evolutivo Diferencial" << "\n" << "****Datos fichero parámetro**** " << std::endl;
    log << "numParada: " << loader->getNumEvaluaciones() << "\n"
        << "tiempoParada: " << loader->getTiempoParada() << "\n"
        << "Tam Población: " << loader->getTamPoblacion() << "\n"
        << "KBest: " << loader->getKBest() << "\n"
        << "Porcentaje de inicialización aleatoria: " << loader->getPorcentajeInicializacion() << "\n"
        << "tam greedy aleatorizado: " << loader->getTamGreedyAleatorio() << "\n"
        << "Tipo de Selección: " << loader->getTipoSeleccion() << "\n"
        << "Conjunto de datos: " <<  loader->getConjuntoDatos() << "\n\n" << std::endl;

    poblacion = new Poblacion(log); // creo la población inicial
    primeraVez = true;
}

EvolutivoDiferencial::~EvolutivoDiferencial() {
    delete poblacion;
    poblacion = nullptr;
    if (log.is_open()){
        log.close();
    }
}

/*
 * Método de ejecución principal del algoritmo
 */

void EvolutivoDiferencial::executeTSP() {

    FileLoader* loader = FileLoader::GetInstancia();
    int numEvaluaciones = 0;
    auto startTime = std::chrono::high_resolution_clock::now(); // inicialización tiempo inicio
    auto endTime = std::chrono::high_resolution_clock::now();   // inicialización tiempo fin
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); //inicialización tiempo transcurrido
    std::vector<Individuo*> individuosSeleccionados;
    primeraVez = true;
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

            if (primeraVez) {
                log << "\nSelecciono al padre de forma secuencial: " << std::endl;
                for (int i = 0; i < padre->getVIndividuo().size(); ++i) {
                    log << padre->getVIndividuo()[i] << " ";
                }
                log << "" << std::endl;
            }

            // RECOMBINACIÓN TERNARIA
            Individuo* IndividuoGenerado = recombinacionTernaria(padre, aleatorio1, aleatorio2, objetivo); // genero el nuevo individuo
            individuosSeleccionados.push_back(IndividuoGenerado); // almaceno el nuevo individuo generado

            delete padre;
            secuenciaPadre++; // actualizo el índice que indica el padre a tomar
            primeraVez = false;
        }

        primeraVez = numEvaluaciones == 0;

        // REMPLAZAMIENTO
        for (int i = 0; i < poblacion->getIndividuos().size(); ++i) {
            individuosSeleccionados[i]->setCoste();
            if (primeraVez)
                log << "Coste hijo: " << individuosSeleccionados[i]->getCosteAsociado() << ", Coste padre: " << poblacion->getIndividuos()[i]->getCosteAsociado() << std::endl;
            if( individuosSeleccionados[i]->getCosteAsociado() < poblacion->getIndividuos()[i]->getCosteAsociado() ) { // si el hijo tiene menor coste que el padre sustituyo
                if (primeraVez)
                    log << "El hijo sustituye al padre" << std::endl;
                poblacion->getIndividuos()[i] = individuosSeleccionados[i];
            } else{
                if (primeraVez){
                    log << "Se mantiene el padre" << std::endl;
                }
            }
        }

        primeraVez = false;
        std::sort(poblacion->getIndividuos().begin(),poblacion->getIndividuos().end(),Poblacion::comparadorIndividuos);
        log << "coste: " << poblacion->getIndividuos()[0]->getCosteAsociado() << std::endl;
        std::cout << "coste: " << poblacion->getIndividuos()[0]->getCosteAsociado() << std::endl;

        endTime = std::chrono::high_resolution_clock::now();
        elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); // actualizo el número de segundos que han transcurrido desde el inicio
        numEvaluaciones++;
    }

}
/*
 * Método encargado de inicializar los individuos pasados por cabecera según el tipo de selección especificado
 *
 * @throw: excepción si no se especifica el tipo de selección como EDB o EDA
 * param aleatorio1, aleatorio2, objetivo: individuos a inicializar
 * post: los individuos especificados en parámetros se inicializan
 */
void EvolutivoDiferencial::seleccionIndividuos(Individuo *&aleatorio1, Individuo *&aleatorio2, Individuo *&objetivo) {

    FileLoader* loader = FileLoader::GetInstancia();

    if(loader->getTipoSeleccion() == "EDA"){


        aleatorio1 = poblacion->getIndividuos()[(rand() % (loader->getTamPoblacion()))];
        aleatorio2 = poblacion->getIndividuos()[(rand() % (loader->getTamPoblacion()))];

        if (primeraVez) {
            log << "Selecciono el individuo aleatorio1 mediante EDA: " << std::endl;
            for (int i = 0; i < aleatorio1->getVIndividuo().size(); ++i) {
                log << aleatorio1->getVIndividuo()[i] << " ";
            }
            log << "Selecciono el individuo aleatorio2 mediante EDA: " << std::endl;
            for (int i = 0; i < aleatorio2->getVIndividuo().size(); ++i) {
                log << aleatorio2->getVIndividuo()[i] << " ";
            }
            log << "" << std::endl;
        }

    }else{
        if (loader->getTipoSeleccion() == "EDB"){

            if (primeraVez) {
                log << "Elijo el individuo aleatorio1 para EDB: " << std::endl;
            }
            int torneoWinner = torneo(); // hago el torneo para el aleatorio1
            aleatorio1 = poblacion->getIndividuos()[torneoWinner];

            if (primeraVez) {
                log << "Elijo el individuo aleatorio2 para EDB" << std::endl;

            }
            int torneoWinner2 = torneo(); // hago el torneo para el aleatorio2
            aleatorio2 = poblacion->getIndividuos()[torneoWinner2];



        } else{
            throw std::invalid_argument("El tipo de selección indicado para el algoritmo es incorrecot");
        }

    }

    if (primeraVez) {
        log << "Elijo el individuo objetivo mediante torneo: " << std::endl;
    }

    int torneoWinner = torneo(); // hago el torneo
    objetivo = poblacion->getIndividuos()[torneoWinner]; // inicilizo 'objetivo' al ganador




}

/*
 * Se aplica la recombinación ternaria mediante los individuos pasados por parámetro para obtener un nuevo individuo
 * @param padre, aleatorio1, aleatorio2, objetivo: Individuos con los que se opera para obtener el nuevo individuo
 * @return: nuevo individuo generado
 */
Individuo* EvolutivoDiferencial::recombinacionTernaria(Individuo *padre, Individuo *aleatorio1, Individuo *aleatorio2, Individuo* objetivo) {

    FileLoader* loader = FileLoader::GetInstancia();

    Individuo* nuevoIndividuo = ( new Individuo(*aleatorio1) ); // inicializo el nuevo individuo
    int posicionSwap = (rand() % (loader->getTamPoblacion() - 1)); // como el swap lo haré entre 'posiciónSwap' y 'posicionSwap' + 1, limito el rango de aleatorio a 'n' - 1, para no salirme de rango
    int elemento1 = posicionSwap, elemento2 = posicionSwap + 1; // elementos correspondientes al swap del padre
    int i = 0, centinela = 0, indiceAleatorio1 = 0, indiceAleatorio2 = 0, elementoAleatorio2 = 0; // indices y elementos correspondientes al vector aleatorio 1 o 2

    if (primeraVez) {
        log << "Hago el swap del padre entre las posiciones " << elemento1 << " y " << elemento2 << std::endl;
    }

    std::swap(padre->getVIndividuo()[posicionSwap],padre->getVIndividuo()[posicionSwap+1]); // swap de las posiciones del padre

    if (primeraVez) {
        log << "Padre tras el swap en dichas posiciones: " << std::endl;
        for (int j = 0; j < padre->getVIndividuo().size(); ++j) {
            log << padre->getVIndividuo()[j] << " ";
        }
        log << "" << std::endl;
    }

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

    if (primeraVez) {
        log << "\nEl primer elemento afectado en el swap del padre se encuentra en la posición(llamemosla posAleatorio1) " << indiceAleatorio1 << " del individuo aleatorio1" << std::endl;
        log << "El segundo elemento afectado en el swap del padre se encuentra en la posición(llamemosla posAleatoriox2) " << indiceAleatorio2 << " del individuo aleatorio2" << std::endl;
    }

    if (primeraVez) {
        log << "\nNuevo individuo ANTES de los swap:" << std::endl;
        for (int k = 0; k < nuevoIndividuo->getVIndividuo().size(); ++k) {
            log << nuevoIndividuo->getVIndividuo()[k] << " ";
        }
        log << "\n" << std::endl;
    }

    int indiceAux = 0;
    for (int j = 0; j < aleatorio1->getVIndividuo().size(); j++) {
        if ( aleatorio1->getVIndividuo()[j] == padre->getVIndividuo()[indiceAleatorio1] ) { // busco la posición en el 'aleatorio1' correspondiente al elemento del padre
            indiceAux = j;
            std::swap(nuevoIndividuo->getVIndividuo()[j], nuevoIndividuo->getVIndividuo()[indiceAleatorio1]);
            break;
        }
    }

    for (int j = 0; j < nuevoIndividuo->getVIndividuo().size(); j++) {
        if ( nuevoIndividuo->getVIndividuo()[j] == elementoAleatorio2 ) { // busco la posición en el 'nuevoIndividuo' correspondiente al elemento 'elementoAleatorio2'

            if (primeraVez) {
                log << "Hago un swap entre el elemento en posAleatorio1(" << nuevoIndividuo->getVIndividuo()[indiceAleatorio1] << ") y el nuevo individuo(inicialmente igual a aleatorio1) " << nuevoIndividuo->getVIndividuo()[indiceAux] << ": " << std::endl;
                log << "Hago un swap entre el elemento en posAleatorio2(" << nuevoIndividuo->getVIndividuo()[indiceAleatorio2] << ") y el nuevo individuo(inicialmente igual a aleatorio1) " << nuevoIndividuo->getVIndividuo()[j] << ": " << std::endl;
            }
            std::swap(nuevoIndividuo->getVIndividuo()[j], nuevoIndividuo->getVIndividuo()[indiceAleatorio2]);

            if (primeraVez) {
                log << "\nNuevo individuo tras los swap:" << std::endl;
                for (int k = 0; k < nuevoIndividuo->getVIndividuo().size(); ++k) {
                    log << nuevoIndividuo->getVIndividuo()[k] << " ";
                }
                log << "\n" << std::endl;
            }

            break;
        }
    }

    Individuo* hijo1 = new Individuo(*nuevoIndividuo);
    this->cruceOX2(nuevoIndividuo, objetivo,hijo1); // aplico el OX2

    if (primeraVez) {

        log << "Aplico una mutación OX2 sobre el nuevo individuo y obtengo: " << std::endl;
        for (int k = 0; k < hijo1->getVIndividuo().size(); ++k) {
            log << hijo1->getVIndividuo()[k] << " ";
        }
        log << "\n" << std::endl;
    }


    delete nuevoIndividuo;


    return hijo1;
}

/*
 * Método que el individuo con menor coste entre una serie de individuos elegidos aleatoriamente
 * @return: un entero que representa el índice en la población del individuo ganador
 */
int EvolutivoDiferencial::torneo() {

    FileLoader *loader = FileLoader::GetInstancia();
    std::set<int> individuosSeleccionados; // conjunto para evitar repetidos
    double mejorCoste = INT_MAX;  // coste mejor individuo
    int mejorIndividuo = 0; // mejor individuo

    while (individuosSeleccionados.size() < loader->getKBest()) { // genero tantos como k

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
    /*if (primeraVez) {
        log << "El ganador del torneo es: " << mejorIndividuo << " con coste " << mejorCoste << "\n\n" << std::endl;
        primeraVez = false;
    }*/
    return mejorIndividuo; // devuelvo el ganador del torneo, el de mejor fiteness


}

/*
 * Genera un hijo a partir de dos individuos mediante el operador OX2
 * @pre hijo inicializado como padre1
 * @param padre1, padre2: inidividuos que representan a los padres
 * @param hijo: individuo resultante de la descendencia
 * @post: hijo se ve modificado por el operador
 */
void EvolutivoDiferencial::cruceOX2(Individuo *padre1, Individuo *padre2, Individuo *hijo) {

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

