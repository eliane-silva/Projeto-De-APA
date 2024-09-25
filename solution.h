#ifndef SOLUTION_H
#define SOLUTION_H

#include "data.h"
#include <vector>
#include <algorithm>
#include <set>
#include <cmath>

struct Solution
{
    std::vector<int> route;
    double cost;

    Solution(std::vector<int> route, double cost) : route(route), cost(cost) {}
    Solution() : route(std::vector<int>(Data::getInstance().totalRequests + 1)), cost(0) {}
    Solution(const Solution &s) : route(s.route), cost(s.cost) {}

    void buildTrivial();

    void greedyBuild();

    void perturbation();

    void print();

    void copy(const Solution &other);

    double evaluateSwap(const int i, const int j);

    double evaluate2Opt(const int i, const int j);

    double evaluateOrOpt(const int i, const int j, const int n);

    double evaluateDoubleBridge(const int pos1, const int len1, const int pos2, const int len2);

    void swap(const int i, const int j);

    void twoOpt(const int i, const int j);

    void orOpt(const int i, const int j, const int n);

    void doubleBridge(const int pos1, const int len1, const int pos2, const int len2);
};

#endif