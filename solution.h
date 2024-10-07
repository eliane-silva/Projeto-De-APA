#ifndef SOLUTION_H
#define SOLUTION_H

#include "data.h"
#include <vector>

struct Solution
{
    std::vector<int> sequence;
    double penalty;

    Solution(std::vector<int> sequence, double penalty) : sequence(sequence), penalty(penalty) {}
    Solution() : sequence(std::vector<int>()), penalty(0) {}
    Solution(const Solution &s) : sequence(s.sequence), penalty(s.penalty) {}

    void buildTrivial();

    void greedyBuild();

    void perturbation();

    void print();

    void updatePenalty();

    void copy(const Solution &other);

    static void connect(Data &data, int &actualTime, double &penalty, int a, int b);

    double evaluateSwap(const int pos1, const int pos2);

    double evaluate2Opt(const int i, const int j);

    double evaluateOrOpt(const int i, const int j, const int n);

    void swap(const int i, const int j);

    void twoOpt(const int i, const int j);

    void orOpt(const int i, const int j, const int n);

    void doubleBridge(const int pos1, const int len1, const int pos2, const int len2);
};

#endif