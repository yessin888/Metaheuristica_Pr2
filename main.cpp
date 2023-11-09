#include <iostream>
#include "FileLoader.h"
#include "Individuo.h"
#include "Poblacion.h"

int main() {
    FileLoader* n = FileLoader::GetInstancia("parametros.txt");
    n->ToString();
    std::vector<int> v(5);

    v[0] = 4;
    v[1] = 2;
    v[2] = 0;
    v[3] = 1;
    v[4] = 3;

    std::swap(v[0],v[2]);

    for (int i = 0; i < 5; i++) {
        std::cout << v[i] << " ";
    }
/*
    //todo semilla? ya que genero random individups

    //Individuo* x = new Individuo(v);
    //std::cout << "El coste es: " << x->getCosteAsociado() << std::endl;

    Poblacion* pob = new Poblacion();
    std::cout << "->>>> " << pob->getIndividuos()[1]->getCosteAsociado();

    std::cout << "Hello, World!" << std::endl;*/
    return 0;
}
