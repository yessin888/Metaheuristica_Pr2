#pragma once

#include <utility>

class CompareGreedy
{

public:
    bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const;


};

