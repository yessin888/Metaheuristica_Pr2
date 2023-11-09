//
// Created by yessi on 31/10/2023.
//

#ifndef PR2META_EVOLUTIVOGENERACIONAL_H
#define PR2META_EVOLUTIVOGENERACIONAL_H


#include "Poblacion.h"

class EvolutivoGeneracional {
private:
    Poblacion* poblacion;
    int torneo();
    void seleccion(std::vector<Individuo*> &nuevaPoblacion);
    void cruceOX2(Individuo* padre1, Individuo* padre2, Individuo* hijo);
    void cruceMOC(Individuo* padre1, Individuo* padre2, Individuo* hijo1, Individuo* hijo2);
    void cruce(std::vector<Individuo*> &IndividuosACruzar);
    void mutacion(std::vector<Individuo*> &IndividuosAMutar);

public:
    EvolutivoGeneracional();
    ~EvolutivoGeneracional();
    void executeEvolutivo();
};


#endif //PR2META_EVOLUTIVOGENERACIONAL_H
