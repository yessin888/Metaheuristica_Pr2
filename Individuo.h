//
// Created by yessi on 31/10/2023.
//

#ifndef PR2META_INDIVIDUO_H
#define PR2META_INDIVIDUO_H


#include <vector>


class Individuo {
private:
    std::vector<int> vIndividuo;
    bool evaluado;
    double costeAsociado;
public:
    Individuo(std::vector<int> &vIndividuo);

    Individuo(const Individuo& copia);

    ~Individuo();

    bool operator<(const Individuo& otro) const;

    bool operator==(const Individuo& otro) const;

    std::vector<int> &getVIndividuo();

    bool isEvaluado() const;

    double getCosteAsociado() const;

    void setVIndividuo(const std::vector<int> &vIndividuo);

    void setCoste();

    void setEvaluado(bool valor);

    double calcularCosteAsociado();
};


#endif //PR2META_INDIVIDUO_H
