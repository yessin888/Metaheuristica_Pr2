//
// Created by yessi on 31/10/2023.
//

#include <chrono>
#include <set>
#include <algorithm>
#include "EvolutivoGeneracional.h"
#include "FileLoader.h"

EvolutivoGeneracional::EvolutivoGeneracional() {


    FileLoader *loader = FileLoader::GetInstancia();

    log.open("RESULTADOS/EvolutivoGeneracional.txt" ); //abro fichero Log
    log << "Evolutivo Generacional" << "\n" << "****Datos fichero parámetro**** " << std::endl;
    log << "numParada: " << loader->getNumEvaluaciones() << "\n"
        << "tiempoParada: " << loader->getTiempoParada() << "\n"
        << "Tam Población: " << loader->getTamPoblacion() << "\n"
        << "Tipo de Cruce: " << loader->getTipoCruce() << "\n"
        << "KBest: " << loader->getKBest() << "\n"
        << "KWorst: " << loader->getKWorst() << "\n"
        << "Porcentaje de inicialización aleatoria: " << loader->getPorcentajeInicializacion() << "\n"
        << "número de élites: " << loader->getNumElite() << "\n"
        << "tam greedy aleatorizado: " << loader->getTamGreedyAleatorio() << "\n"
        << "probabiliadad de mutación: " << loader->getProbabilidadMutacion() << "\n"
        << "probabilidad de cruce: " << loader->getProbabilidadCruce() << "\n"
        << "Conjunto de datos: " <<  loader->getConjuntoDatos() << "\n\n" << std::endl;

    poblacion = new Poblacion(log); // creo la población inicial
    primeraVez = true;

    // gracias a los constructores y las relaciones entre clases, mediante este constructor consigo inicializar los individuos de la
    //  población, evaluarlos y quedarme con los élite


}

EvolutivoGeneracional::~EvolutivoGeneracional() {
    delete poblacion;
    poblacion = nullptr;
    if (log.is_open()) {
        log.close();
    }
}

/*
 * Método de ejecuón principal
 */
void EvolutivoGeneracional::executeEvolutivo() {

    FileLoader* loader = FileLoader::GetInstancia();
    int numEvaluaciones = 0;
    auto startTime = std::chrono::high_resolution_clock::now(); // inicialización tiempo inicio
    auto endTime = std::chrono::high_resolution_clock::now();   // inicialización tiempo fin
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); //inicialización tiempo transcurrido
    std::vector<Individuo*> individuosSeleccionados; // individuos resultantes tras hacer la selección
    std::vector<Individuo*> elites; // vector auxiliar para recoger aquellos individuos élites que no se mantiene en la nueva población

//todo generaciones contador
    while( numEvaluaciones < loader->getNumEvaluaciones() && elapsed_seconds.count() < loader->getTiempoParada() ) {
        //SELECCIÓN
        while(individuosSeleccionados.size() < loader->getTamPoblacion()) { // hago torneo hasta seleccionar todos los individuos
            int torneoWinner = torneo(); // hago el torneo
            individuosSeleccionados.push_back(poblacion->getIndividuos()[torneoWinner]); // almaceno los individuos seleccionados
            primeraVez = false; // variable para la gestión del log
        }
        primeraVez = numEvaluaciones == 0; // variable para la gestión del log
        //CRUCE
        cruce(individuosSeleccionados);

        primeraVez = numEvaluaciones == 0; // variable para la gestión del log
        //MUTACIÓN
        mutacion(individuosSeleccionados);

        primeraVez = numEvaluaciones == 0; // variable para la gestión del log

        //REMPLAZAMIENTO
        if( !this->comprobarElitismo(individuosSeleccionados,elites) && elites.size() != 0 ) { // compruebo si los elites se mantienen en la nueva población
            // en 'elites' se insertan los élites que faltan en la población
            int centinela = 0;
            if (primeraVez) {
                log << "Como se mantiene el elitismo, inserto los élites que faltan" << std::endl;
            }
            do{
                int perdedor = torneoPerdedor(individuosSeleccionados); // esto hace el torneo y cambia los perdedores por elites
                if (primeraVez) {
                    log << "Inserto el élite en la posición del perdedor" << std::endl;
                    primeraVez = numEvaluaciones == 0;
                }
                if (insertarElite(perdedor,individuosSeleccionados,centinela) ) {
                    centinela++; // si se ha logrado insertar un élite actualizo el contador
                }
            } while ( centinela != poblacion->getElite().size() ); // voy haciendo torneos hasta insertar todos los élites

        }

        poblacion->setIndividuos(individuosSeleccionados); // cambio los individuos de la población, calculo los costes de los individuos modificados y calculo los nuevos élites
        individuosSeleccionados.clear(); // reinicio el vector
        elites.clear(); // reinicio el vector
        log << "" << std::endl;

        endTime = std::chrono::high_resolution_clock::now(); // actualizo el contador
        elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime); // actualizo el número de segundos que han transcurrido desde el inicio
        numEvaluaciones++; // actualizo el contador

        primeraVez = false;
    }

    std::cout << "costes de élites" << std::endl;


    for (int i = 0; i < poblacion->getElite().size(); ++i) {
        std::cout << poblacion->getElite()[i]->getCosteAsociado() << std::endl;
    }

    std::cout << "tiempo" << elapsed_seconds.count() << std::endl;
    log << "Número de evaluaciones ejecutada: " << numEvaluaciones << ", tiempo transcurrido: " << elapsed_seconds.count() << " segundos" << std::endl;




}

/*
 * Método que el individuo con menor coste entre una serie de individuos elegidos aleatoriamente
 * @return: un entero que representa el índice en la población del individuo ganador
 */
int EvolutivoGeneracional::torneo() {

    FileLoader *loader = FileLoader::GetInstancia();
    std::set<int> individuosSeleccionados; // conjunto para evitar repetidos
    double mejorCoste = INT_MAX;  // coste mejor individuo
    int mejorIndividuo = 0; // mejor individuo

    while (individuosSeleccionados.size() < loader->getKBest()) { // genero tantos como k

        int numero = (rand() % (loader->getTamPoblacion())); // elijo un individuo de la población de manera aleatoria
        individuosSeleccionados.insert(numero); // evito duplicar los individuos con el conjunto

    }

    if (primeraVez){
        log << "\n\nEjemplo de torneo:\nIndividuos seleccionados para el torneo " << std::endl;
    }
    for( std::set<int>::iterator it = individuosSeleccionados.begin(); it != individuosSeleccionados.end(); ++it ) {
        int i = *it; // índice individuo de la población
        if (primeraVez) {
            log << "Individuo numero " << i << " de la población, con coste: " << poblacion->getIndividuos()[i]->getCosteAsociado() << std::endl;
        }
        if( poblacion->getIndividuos()[i]->getCosteAsociado() < mejorCoste ) { // me quedo con el individuo con mejor fitness
            mejorCoste = poblacion->getIndividuos()[i]->getCosteAsociado();
            mejorIndividuo = i;
        }

    }
    if (primeraVez) {
        log << "El ganador del torneo es: " << mejorIndividuo << " con coste " << mejorCoste << "\n\n" << std::endl;
        primeraVez = false;
    }
    return mejorIndividuo; // devuelvo el ganador del torneo, el de mejor fiteness

}

/*
 * Método que se encarga de aplicar el cruce pertinente sobre los individuos
 * @param individuosACruzar: vector de individuos
 * @post: el vector individuosACruzar se ve modificado por los cruces
 */
void EvolutivoGeneracional::cruce(std::vector<Individuo*> &IndividuosACruzar) {

    FileLoader* loader = FileLoader::GetInstancia();
    Individuo* padre1 = nullptr;
    Individuo* padre2 = nullptr;
    Individuo* hijo1 = nullptr;
    Individuo* hijo2 = nullptr;
    for ( int i = 0; i < IndividuosACruzar.size() - 1; i+=2 ) { // itero de dos en dos

        //todo considerar poblaciones de tamaño impar
        padre1 = IndividuosACruzar[i]; // inicializo el padre1
        padre2 = IndividuosACruzar[i+1]; // inicializo el padre2
        int random = (rand() % (10)); // aleatorio para simular 70%

        if( random < (int)(loader->getProbabilidadCruce() / 10) ) { // simulo probabilidad
            hijo1 = new Individuo(*padre1); // inicializo el hijo1 igual que el padre1
            hijo2 = new Individuo(*padre2); // inicializo el hijo2 igual que el padre2
            if( loader->getTipoCruce() == "OX2" ) {

                if (primeraVez) {
                    log << "Aplico el cruce OX2 sobre" << "\n" << "padre1: ";
                    for (int j = 0; j < padre1->getVIndividuo().size(); ++j) {
                        log << padre1->getVIndividuo()[j] << " ";
                    }
                    log << "\npadre2: ";
                    for (int j = 0; j < padre2->getVIndividuo().size(); ++j) {
                        log  << padre2->getVIndividuo()[j] << " ";
                    }
                    log << "" << std::endl;
                }

                cruceOX2(padre1,padre2,hijo1); // hago el cruce OX2 entre padre1-padre2 para obtener el hijo1

                if (primeraVez) {
                    log << "\nEl hijo obtenido usando OX2 para el padre1-padre2" << "\n" << "hijo: ";
                    for (int j = 0; j < hijo1->getVIndividuo().size(); ++j) {
                        log << hijo1->getVIndividuo()[j] << " ";
                    }
                    log << "\n" << "Aplico OX2 para padre2-padre1 y obtengo un segundo hijo, además marco que el coste de los individuos padres han cambiado " << std::endl;
                    primeraVez = false;
                }

                cruceOX2(padre2,padre1,hijo2); // hago el cruce OX2 entre padre2-padre1 para obtener el hijo2
                IndividuosACruzar[i] = hijo1; // introduzco el hijo1 en el vector de individuso
                IndividuosACruzar[i]->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
                IndividuosACruzar[i+1] = hijo2; // introduzco el hijo2 en el vector de individuso
                IndividuosACruzar[i+1]->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
            }else{
                if( loader->getTipoCruce() == "MOC" ){
                    if (primeraVez) {
                        log << "Aplico el cruce MOC" << "\n" << "padre1: ";
                        for (int j = 0; j < padre1->getVIndividuo().size(); ++j) {
                            log << padre1->getVIndividuo()[j] << " ";
                        }
                        log << "\npadre2: ";
                        for (int j = 0; j < padre2->getVIndividuo().size(); ++j) {
                            log << padre2->getVIndividuo()[j] << " ";
                        }
                        log << "" << std::endl;
                    }

                    cruceMOC(padre1,padre2,hijo1,hijo2); // aplico MOC para obtener ambos hijos

                    if (primeraVez) {
                        log << "\nLos hijos obtenidos usando MOC" << "\n" << "hijo1: ";
                        for (int j = 0; j < hijo1->getVIndividuo().size(); ++j) {
                            log << hijo1->getVIndividuo()[j] << " ";
                        }
                        log << "\nhijo2: ";
                        for (int j = 0; j < hijo2->getVIndividuo().size(); ++j) {
                            log << hijo2->getVIndividuo()[j] << " ";
                        }
                        log << "Además marco que el coste de los individuos padres han cambiado \n" << std::endl;
                        primeraVez = false;
                    }

                    IndividuosACruzar[i] = hijo1; // introduzco el hijo1 en el vector de individuos
                    IndividuosACruzar[i]->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
                    IndividuosACruzar[i+1] = hijo2; // introduzco el hijo2 en el vector de individuos
                    IndividuosACruzar[i+1]->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente
                }else{
                    throw std::invalid_argument("El algoritmo de cruce especificado en el paramFile es incorrecto, recuerda que debe ser \"OX2\" o \"MOC\"");
                }
            }

        }

    }

}

/*
 * Genera un hijo a partir de dos individuos mediante el operador OX2
 * @pre hijo inicializado como padre1
 * @param padre1, padre2: inidividuos que representan a los padres
 * @param hijo: individuo resultante de la descendencia
 * @post: hijo se ve modificado por el operador
 */
void EvolutivoGeneracional::cruceOX2(Individuo *padre1, Individuo *padre2, Individuo *hijo) {

    int centinela = 0;
    std::vector<int> elementosSeleccionados;
    int i = 0;
    bool fin = true;

    if (primeraVez) {
        log << "elementos seleccionadas para el cruceOX2: " << std::endl;
    }

    for (int i = 0; i < padre2->getVIndividuo().size() ; i++) {
        int numero = (rand() % (10)); // aleatorio de [0-9]
        if( numero < 5 ) { // simulo probabilidad
            elementosSeleccionados.push_back( padre2->getVIndividuo()[i] ); // elemento se añade a la lista
            if (primeraVez) {
                log << padre2->getVIndividuo()[i] << " ";
            }
        }
    }
    if (primeraVez) {
        log << "\n" <<std::endl;
    }


    while( i < hijo->getVIndividuo().size() && fin ) {

        for( int j = 0; j < elementosSeleccionados.size(); j++) { // recorro los elementos seleccionados
            if( elementosSeleccionados[j] == hijo->getVIndividuo()[i] ) { // compruebo si es uno de los elementos seleccionados de manera aleatoria
                if (primeraVez) {
                    log << hijo->getVIndividuo()[i] << " es uno de los elementos seleccionados, por tanto, lo sustituyo por " << elementosSeleccionados[centinela] << std::endl;
                }
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
 * genero dos hijos mutando dos individuos mediante el operador MOC
 * @pre: el hijoi debe ser inicializado como el padrei, para i=1 e i=2
 * @param padre1, padre2: inidividuos que representan a los padres
 * @param hijo1, hijo2: individuos resultantes de la descendencia
 * @post: hijos se ven modificados por el operador
*/

void EvolutivoGeneracional::cruceMOC(Individuo *padre1, Individuo *padre2, Individuo *hijo1, Individuo* hijo2) {

    int centinela = 0, centinela2 = 0;
    std::vector<int> parteDerechaPadre2;
    std::vector<int> parteDerechaPadre1;
    int i = 0;
    bool fin = true;

    int posicionSeparacion = (rand() % (padre1->getVIndividuo().size())); // establezco la posición de separación

    if ( primeraVez ) {
        log << "Posición de separación establecida en el MOC: " << posicionSeparacion << "\n" << std::endl;
    }

    for (int j = posicionSeparacion; j < padre1->getVIndividuo().size(); j++) {
        parteDerechaPadre1.push_back(padre1->getVIndividuo()[j]); // relleno los elementos correspondiente a la parte derecha del padre1
        parteDerechaPadre2.push_back(padre2->getVIndividuo()[j]); // relleno los elementos correspondiente a la parte derecha del padre2
    }

    while( i < hijo1->getVIndividuo().size() && fin ) {

        for(int j = 0; j < parteDerechaPadre2.size(); j++) { // recorro los elementos seleccionados
            if(parteDerechaPadre2[j] == hijo1->getVIndividuo()[i] ) { // compruebo si el elemento del hijo pertenece a la parte derecha del padre2
                hijo1->getVIndividuo()[i] = parteDerechaPadre2[centinela]; // en caso de ser así sustituyo dicho elemento por su correspondiente
                if (primeraVez) {
                    log << hijo1->getVIndividuo()[i] << " es uno de los elementos de la parte derecha del padre2, por tanto, lo sustituyo por " << parteDerechaPadre2[centinela] << std::endl;
                }
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
                if (primeraVez) {
                    log << hijo2->getVIndividuo()[i] << " es uno de los elementos de la parte derecha del padre1, por tanto, lo sustituyo por " << parteDerechaPadre1[centinela2] << std::endl;
                }
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

/*
 * Muto el vector de individuos mediante un swap de dos elementos según una probabilidad
 * @param individuosAMutar: vector de individuos
 */

void EvolutivoGeneracional::mutacion(std::vector<Individuo *> &IndividuosAMutar) {

    FileLoader* loader = FileLoader::GetInstancia();

    for (int i = 0; i < IndividuosAMutar.size(); i++) {
        int random = (rand() % (10)); // simular probabilidad
        if( random < (int)(loader->getProbabilidadMutacion() / 10) ) { // simulo probabilidad

            if ( primeraVez ) {
                log << "\n\nEjemplo de operador mutación: \n" << "Lo aplico sobre: " << std::endl;
                for (int j = 0; j < IndividuosAMutar[i]->getVIndividuo().size(); ++j) {
                    log << IndividuosAMutar[i]->getVIndividuo()[j] << " ";
                }
            }

            int posicionRandom1 = (rand() % (loader->getTamDatos())); // primera posición del individuo para el 2-opt
            int posicionRandom2 = (rand() % (loader->getTamDatos())); // segunda posición del individuo para el 2-opt
            std::swap(IndividuosAMutar[i]->getVIndividuo()[posicionRandom1],IndividuosAMutar[i]->getVIndividuo()[posicionRandom2]); // aplico 2-opt
            IndividuosAMutar[i]->setEvaluado(false); // indico que el individuo se ha modificado y que por tanto es necesario racalcular el coste posteriormente

            if (primeraVez) {

                log << "\nPosiciones del swap: " << posicionRandom1 << "-" << posicionRandom2 << ", swap entre " <<  IndividuosAMutar[i]->getVIndividuo()[posicionRandom1] << "-" << IndividuosAMutar[i]->getVIndividuo()[posicionRandom2] << "\nNuevo individuo tras mutar: " << std::endl;
                for (int j = 0; j < IndividuosAMutar[i]->getVIndividuo().size(); ++j) {
                    log << IndividuosAMutar[i]->getVIndividuo()[j] << " ";
                }
                log << "\n" << std::endl;
                primeraVez = false;
            }
        }

    }

}

/*
 * Compruebo que la nueva población mantenga los elites de la anterior
 * @return: booleano indicando si se mantienen los elites o no
 * @post: en caso de no mantener los elites inserto en el parámetro vector 'elites' los individuos que falten, así
 *  si la población tiene 2 élites y solo mantiene uno, en 'elites' meto el otro que falta
 * @param individuosNuevos[IN]: vetor que representa la nueva población a evaluar
 * @param elites[IN/OUT]: veector en el que almaceno los individuos élites que no se encuentran en la nueva población
 */

bool EvolutivoGeneracional::comprobarElitismo(std::vector<Individuo *> &individuosNuevos, std::vector<Individuo *> &elites) {

    if( primeraVez ) {
        log << "Compruebo si se mantiene el elitismo en la descendencia" << std::endl;
    }

    int elementosElites = 0; // contador de elementos élites que se mantienen en la descendencia
    for (int i = 0; i < poblacion->getElite().size(); i++) {
        bool encontrado = false;
        for (int j = 0; j < individuosNuevos.size(); ++j) {
            if( poblacion->getElite()[i] == individuosNuevos[j] ) { // compruebo si el élite se encuentra en la población
               encontrado = true;
               elementosElites++; // actualizo contador
                if (primeraVez) {
                    log << "Se mantiene el élite " << i << " en la descendencia" << std::endl;
                    for (int k = 0; k < elites[i]->getVIndividuo().size(); ++k) {
                        log << elites[i]->getVIndividuo()[k] << " ";
                    }
                    log << "" << std::endl;
                }
            }
        }
        if( !encontrado ) { // en caso de que el élite no siga en la descendencia
            elites.push_back(poblacion->getElite()[i]); // almaceno el élite en el vector 'elite'
            if (primeraVez) {
                log << "No se mantiene el élite " << i << " en la descendencia" << std::endl;
                for (int k = 0; k < elites[i]->getVIndividuo().size(); ++k) {
                    log << elites[i]->getVIndividuo()[k] << " ";
                }
                log << "" << std::endl;
            }
        }
    }

    return ( elementosElites == poblacion->getElite().size() ); // si el número de elites es igual al tamaño de elites de la población -> true
}

/*
 * Método que devuelve el perdedor de entre una serie de individuos seleccionados de manera aleatoria
 * @param nuevaPoblacion: vector de individuos sobre el que se trabaja
 * @return: se devuelve el índice en el vector 'nuevaPoblacion' correspondiente al perdedor del torneo
 */
int EvolutivoGeneracional::torneoPerdedor(std::vector<Individuo*> &nuevaPoblacion) {

    FileLoader *loader = FileLoader::GetInstancia();
    std::set<int> individuosSeleccionados; // conjunto para evitar duplicados
    double peorCoste = -INT_MAX;  // coste peeor individuo
    int peorIndividuo = 0; // peor individuo

        while ( individuosSeleccionados.size() < loader->getKWorst() ) {

            int numero = (rand() %(loader->getTamPoblacion())); // elijo un individuo de la población de manera aleatoria
            individuosSeleccionados.insert(numero); // evito duplicar los individuos con el conjunto

        }

        if (primeraVez){
            log << "\n\nEjemplo de torneo:\nIndividuos seleccionados para el torneo de perdedores" << std::endl;
        }

        for (std::set<int>::iterator it = individuosSeleccionados.begin(); it != individuosSeleccionados.end(); ++it) {

            int i = *it; // índice individuo de la población

            if (primeraVez) {
                log << "Individuo numero " << i << " de la población, con coste: " << poblacion->getIndividuos()[i]->getCosteAsociado() << std::endl;
            }

            if (nuevaPoblacion[i]->getCosteAsociado() > peorCoste) { // me quedo con el individuo con peor fitness
                peorCoste = nuevaPoblacion[i]->getCosteAsociado();
                peorIndividuo = i;
            }

        }

    if (primeraVez) {
        log << "El ganador del torneo de perdedores es: " << peorIndividuo << " con coste " << peorCoste << "\n\n" << std::endl;
        primeraVez = false;
    }

    return peorIndividuo;


}

/*
 * Inserto el élite en el vector dado en los parámetros
 * @param perdedor: posición en la que insertar el élite
 * @nuevoIndividuos: vector de individuos en el cual hay que insertar el élite
 * @param eliteAInsertar: índice que representa el élite de la población a insertar
 */
bool EvolutivoGeneracional::insertarElite(int perdedor, std::vector<Individuo *>& nuevoIndividuos, int eliteAInsertar) {

    for (int i = 0; i < poblacion->getElite().size(); i++) {
        if ( nuevoIndividuos[perdedor] == poblacion->getElite()[i]) { // evito insertar el nuevo élite sobre un élite insertado previamente
            return false;
        }
    }
    // si no es una posición de un élite inserto el élite correspondiente
    nuevoIndividuos[perdedor] = poblacion->getElite()[eliteAInsertar];
    return true;
}

