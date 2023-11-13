#include <iostream>
#include <algorithm>
#include <set>
#include "FileLoader.h"
#include "Individuo.h"
#include "Poblacion.h"
#include "EvolutivoGeneracional.h"


int main() {
    FileLoader* n = FileLoader::GetInstancia("parametros.txt");
    FileLoader* loader = FileLoader::GetInstancia();
   // n->ToString();

    //todo semilla? ya que genero random individups

    //Individuo* x = new Individuo(v);
    //std::cout << "El coste es: " << x->getCosteAsociado() << std::endl;

//    Poblacion* pob = new Poblacion();
  //  std::cout << "->>>> " << pob->getIndividuos()[1]->getCosteAsociado();

  EvolutivoGeneracional* aas = new EvolutivoGeneracional();
    aas->executeEvolutivo();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
