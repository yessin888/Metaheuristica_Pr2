//
// Created by yessi on 31/10/2023.
//

#include "Individuo.h"
#include "FileLoader.h"

Individuo::Individuo(std::vector<int> &vIndividuo):evaluado(true) {

    this->vIndividuo = vIndividuo;
    costeAsociado = calcularCosteAsociado(); // inicializo el coste del individuo

}
Individuo::~Individuo() {

}

int Individuo::calcularCosteAsociado() {

    FileLoader* loader = FileLoader::GetInstancia();
    int coste = 0;

    for (int i = 0; i < loader->getTamDatos() - 1; i++) {
        coste += loader->getDistancias()[vIndividuo[i]][vIndividuo[i+1]];
    }
    coste += loader->getDistancias()[vIndividuo[0]][vIndividuo[loader->getTamDatos() - 1]];

    return coste;

}

std::vector<int> &Individuo::getVIndividuo() {
    return vIndividuo;
}

bool Individuo::isEvaluado() const {
    return evaluado;
}

int Individuo::getCosteAsociado() const {
    return costeAsociado;
}

void Individuo::setVIndividuo(const std::vector<int> &vIndividuo) {
    Individuo::vIndividuo = vIndividuo;
}
