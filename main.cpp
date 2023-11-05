#include <iostream>
#include "FileLoader.h"
#include "Individuo.h"

int main() {
    FileLoader* n = FileLoader::GetInstancia("parametros.txt");
    n->ToString();
    std::vector<int> v(5);

    v[0] = 4;
    v[1] = 2;
    v[2] = 0;
    v[3] = 1;
    v[4] = 3;

    //todo semilla? ya que genero random individups

    Individuo* x = new Individuo(v);
    std::cout << "El coste es: " << x->getCosteAsociado() << std::endl;


    std::cout << "Hello, World!" << std::endl;
    return 0;
}
