//
// Created by yessi on 31/10/2023.
//

#ifndef PR2META_POBLACION_H
#define PR2META_POBLACION_H

#include "Individuo.h"

class Poblacion {
private:
    std::vector<Individuo*> individuos;
    std::vector<Individuo*> elite; // individuos con mejor fitness
    void inicializarPoblacion();
    void generaInidividuoAleatorio(std::vector<int>& v);
    void generaInidividuoGreedy(std::vector<int>& v);
public:
    Poblacion();
    Poblacion(int size);
    ~Poblacion();
    const std::vector<Individuo *> &getIndividuos() const;

    void calcularElite();
};


#endif //PR2META_POBLACION_H
