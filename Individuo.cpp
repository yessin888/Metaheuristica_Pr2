//
// Created by yessi on 31/10/2023.
//

#include "Individuo.h"
#include "FileLoader.h"

Individuo::Individuo(std::vector<int> &vIndividuo):evaluado(true) {

    //todo al evaluar los individuos inicialmente les pongo 'evaluado' a tru, si al mutar , combinar o lo que sea cambio el inidividuo le pongo 'evaluado' a false
    //asi cuando tenga la nueva poblacion y tenga que evaluarla lo hago solo sobre los modificados

    this->vIndividuo = vIndividuo;
    costeAsociado = calcularCosteAsociado(); // inicializo el coste del individuo

}

Individuo::Individuo(const Individuo &copia) {

    vIndividuo = copia.vIndividuo;
    evaluado = copia.evaluado;
    costeAsociado = copia.costeAsociado;

}

bool Individuo::operator<(const Individuo &otro) const {
    return this->costeAsociado < otro.costeAsociado;
}



Individuo::~Individuo() {

}

double Individuo::calcularCosteAsociado() {

    FileLoader* loader = FileLoader::GetInstancia();
    double coste = 0;

    for (int i = 0; i < loader->getTamDatos() - 1; i++) {
        if( i == 16 )
            std::cout << "";// todo ERRORRR!!!! meto en vIndividuo[10] un 287, fuera de rango
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

double Individuo::getCosteAsociado() const {
    return costeAsociado;
}

void Individuo::setVIndividuo(const std::vector<int> &vIndividuo) {
    Individuo::vIndividuo = vIndividuo;
}

void Individuo::setEvaluado(bool valor) {
    evaluado = valor;
}

bool Individuo::operator==(const Individuo &otro) const {

    if( vIndividuo.size() != otro.vIndividuo.size() ) {
        return false;
    }

    for (int i = 0; i < this->vIndividuo.size(); ++i) {
        if( vIndividuo[i] != otro.vIndividuo[i] )
            return false;
    }

    return true;

}

void Individuo::setCoste() {
    costeAsociado = calcularCosteAsociado();
}
