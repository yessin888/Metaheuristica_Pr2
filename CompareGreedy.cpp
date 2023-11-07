#include "CompareGreedy.h"


bool CompareGreedy::operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const {
    if (lhs.first == rhs.first) {
        // Si el first es igual, ordenar por second de menor a mayor
        return lhs.second > rhs.second;
    }
    // Ordenar por first de mayor a menor
    return lhs.first < rhs.first;
}