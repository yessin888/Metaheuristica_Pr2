#include <iostream>
#include <algorithm>
#include <set>
#include "FileLoader.h"
#include "Individuo.h"
#include "Poblacion.h"
#include "EvolutivoGeneracional.h"
#include "EvolutivoDiferencial.h"


int main() {
    FileLoader* n = FileLoader::GetInstancia("parametros.txt");
    FileLoader* loader = FileLoader::GetInstancia();
   // n->ToString();

    //todo semilla? ya que genero random individups

    //Individuo* x = new Individuo(v);
    //std::cout << "El coste es: " << x->getCosteAsociado() << std::endl;

//    Poblacion* pob = new Poblacion();
  //  std::cout << "->>>> " << pob->getIndividuos()[1]->getCosteAsociado();
    std::srand(26567904);


    try{
      EvolutivoGeneracional* aas = new EvolutivoGeneracional();
      aas->executeEvolutivo();
      EvolutivoDiferencial* tsp = new EvolutivoDiferencial();
      tsp->executeTSP();
  }catch(const std::exception& e) {
      std::cout << e.what() << std::endl;
  }

    return 0;
}
