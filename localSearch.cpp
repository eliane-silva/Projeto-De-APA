#include "localSearch.h"
#include "stdlib.h"
#include <iostream>

void RVND(Solution &s)
{

    std::vector<int> NL = {1, 2, 3, 4, 5}; // NL = {1, 2, 3, 4, 5} após implementar os movimentos de vizinhança
    bool improved = false;

    while (NL.empty() == false)
    {
        int n = (rand() % NL.size());

        switch (NL[n])
        {
        case 1:
            improved = bestImprovementSwap(s);
            break;
        case 2:
            improved = bestImprovement2Opt(s);
            break;
        case 3:
            improved = bestImprovementOrOpt(s, 1); // Reinsertion
            break;
        case 4:
            improved = bestImprovementOrOpt(s, 2); // Or-opt2
            break;
        case 5:
            improved = bestImprovementOrOpt(s, 3); // Or-opt3
            break;
        default:
            break;
        }

        if (improved)
            NL = {1, 2, 3, 4, 5}; // NL = {1, 2, 3, 4, 5} após implementar os movimentos de vizinhança
        else
            NL.erase(NL.begin() + n);
    }
}

bool bestImprovementSwap(Solution &s)
{
    Data &data = Data::getInstance();
    double delta, bestDelta = 0.0;
    int bestI, bestJ, i, j;
    bool improved = false;

    for (i = 0; i < data.totalRequests - 1; i++)
    {
        for (j = i + 1; j < data.totalRequests; j++)
        {
            delta = s.evaluateSwap(i, j);
            if (delta < bestDelta)
            {
                bestDelta = delta;
                bestI = i;
                bestJ = j;
                improved = true;
            }
        }
    }

    if (improved)
        s.swap(bestI, bestJ);

    return improved;
}

bool bestImprovement2Opt(Solution &s)
{
    Data &data = Data::getInstance();
    double delta, bestDelta = 0.0;
    int bestI, bestJ, i, j;
    bool improved = false;

    for (i = 0; i < data.totalRequests - 1; i++)
    {
        for (j = i + 1; j < data.totalRequests; j++)
        {
            delta = s.evaluate2Opt(i, j);
            if (delta < bestDelta)
            {
                bestDelta = delta;
                bestI = i;
                bestJ = j;
                improved = true;
            }
        }
    }

    if (improved)
        s.twoOpt(bestI, bestJ);

    return improved;
}

bool bestImprovementOrOpt(Solution &s, int n)
{
    Data &data = Data::getInstance();
    double delta, bestDelta = 0.0;
    int bestI, bestJ, i, j;
    bool improved = false;

    for (i = -1; i < data.totalRequests; i++)
    {
        for (j = 0; j < data.totalRequests - n + 1; j++)
        {
            if (i + 1 == j || i >= j && j + n - 1 >= i)
                continue;
            delta = s.evaluateOrOpt(i, j, n);
            if (delta < bestDelta)
            {
                bestDelta = delta;
                bestI = i;
                bestJ = j;
                improved = true;
            }
        }
    }

    if (improved)
        s.orOpt(bestI, bestJ, n);

    return improved;
}

Solution solve()
{
    Solution s = Solution();
    // s.buildTrivial();
    s.greedyBuild();
    std::cout << "Solucao inicial:" << std::endl;
    s.print();
    std::cout << "Solucao final:" << std::endl;
    RVND(s);
    return s;
}

Solution ILS(int maxIter, int maxIterIls)
{
    Solution bestOfAll;
    bestOfAll.penalty = INFINITY;

    for (int i = 0; i < maxIter; i++)
    {
        Solution s;
        s.greedyBuild();
        Solution best = s;

        int iterIls = 0;
        while (iterIls <= maxIterIls)
        {
            RVND(s);
            if (s.penalty < best.penalty)
            {
                best = s;
                iterIls = 0;
            }
            s.copy(best);
            s.perturbation();
            iterIls++;
        }

        if (best.penalty < bestOfAll.penalty)
            bestOfAll = best;
    }
    return bestOfAll;
}
