//
// Created by Yessin on 19/11/2023.
//

#ifndef PR2META_EVOLUTIVODIFERENCIAL_H
#define PR2META_EVOLUTIVODIFERENCIAL_H


#include "Poblacion.h"

class EvolutivoDiferencial {
private:
    Poblacion* poblacion;
    std::ofstream log;
    bool primeraVez;

    int torneo();
    void seleccionIndividuos(Individuo *&aleatorio1, Individuo *&aleatorio2, Individuo *&objetivo);
    Individuo* recombinacionTernaria(Individuo* padre, Individuo *aleatorio1, Individuo *aleatorio2, Individuo* objetivo);
    void cruceOX2(Individuo* padre1, Individuo* padre2, Individuo* hijo);



public:
    EvolutivoDiferencial();
    ~EvolutivoDiferencial();
    void executeTSP();


    void cruceMOC(Individuo *padre1, Individuo *padre2, Individuo *hijo1, Individuo *hijo2);
};


#endif //PR2META_EVOLUTIVODIFERENCIAL_H
