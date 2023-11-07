cmake_minimum_required(VERSION 3.26)
project(Pr2Meta)

set(CMAKE_CXX_STANDARD 17)

add_executable(Pr2Meta main.cpp
        FileLoader.h
        FileLoader.cpp
        Poblacion.cpp
        Poblacion.h
        Poblacion.h
        Individuo.cpp
        Individuo.h
        Individuo.h
        EvolutivoGeneracional.cpp
        EvolutivoGeneracional.h
        EvolutivoGeneracional.cpp
        CompareGreedy.cpp
        CompareGreedy.h)
