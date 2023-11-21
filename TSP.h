//
// Created by Yessin on 19/11/2023.
//

#ifndef PR2META_TSP_H
#define PR2META_TSP_H


#include "Poblacion.h"

class TSP {
private:
    Poblacion* poblacion;
    int torneo();
    void seleccionIndividuos(Individuo *&aleatorio1, Individuo *&aleatorio2, Individuo *&objetivo);
    Individuo* recombinacionTernaria(Individuo* padre, Individuo *aleatorio1, Individuo *aleatorio2, Individuo* objetivo);
    //void recombinacionTernatria(std::vector<Individuo *>& individuosARecombinar);
    //void GeneracionPadresTernaria(Individuo *padre1, Individuo *padre2, Individuo *padre3);
    //void DiferenciaIndividuos(Individuo *padreMutado, Individuo *padre2, Individuo *padre3);
    //int sumatoria(Individuo *pIndividuo, Individuo *pIndividuo1);
    void cruceOX2(Individuo* padre1, Individuo* padre2, Individuo* hijo);



public:
    TSP();
    ~TSP();
    void executeTSP();


    void cruceMOC(Individuo *padre1, Individuo *padre2, Individuo *hijo1, Individuo *hijo2);
};


#endif //PR2META_TSP_H
