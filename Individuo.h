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
    int costeAsociado;
    int calcularCosteAsociado();
public:
    Individuo(std::vector<int> &vIndividuo);
    ~Individuo();

    std::vector<int> &getVIndividuo();

    bool isEvaluado() const;

    int getCosteAsociado() const;

    void setVIndividuo(const std::vector<int> &vIndividuo);
};


#endif //PR2META_INDIVIDUO_H
