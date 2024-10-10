#include "localSearch.h"
#include <chrono>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>

void RVND(Solution &s)
{
    std::vector<int> NL = {1, 2, 3, 4, 5};
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
            NL = {1, 2, 3, 4, 5};
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

Solution ILS(int constructions, int maxIterIls)
{
    Solution bestOfAll;
    bestOfAll.penalty = INFINITY;

    for (int i = 0; i < constructions; i++)
    {
        std::cout << "Current Construction.....: " << i + 1 << "/" << constructions << "\n";
        std::cout << "Current BestOfAll Penalty: " << bestOfAll.penalty << "\n\n";
        Solution s;
        s.greedyBuild();
        Solution best;
        best.copy(s);

        int iterIls = 0;
        while (iterIls <= maxIterIls)
        {
            RVND(s);
            if (s.penalty < best.penalty)
            {
                best.copy(s);
                iterIls = 0;
            }
            s.copy(best);
            s.perturbation();
            s.updatePenalty();
            iterIls++;
        }

        if (best.penalty < bestOfAll.penalty)
        {
            std::cout << "\tOld BestOfAll Penalty: " << bestOfAll.penalty << "\n";
            std::cout << "\tNew BestOfAll Penalty: " << best.penalty << "\n";
            std::cout << "\tDelta........: " << best.penalty - bestOfAll.penalty << "\n\n";
            bestOfAll.copy(best);
        }
    }
    return bestOfAll;
}

void ILSBenchmark(int constructions, int maxIterIls)
{
    auto metaheuristic_start = std::chrono::high_resolution_clock::now();

    std::chrono::_V2::system_clock::time_point start;
    std::chrono::_V2::system_clock::time_point end;

    std::vector<double> construction_times;
    std::vector<double> construction_penalties;
    std::vector<double> rvnd_times;
    std::vector<double> rvnd_penalties;

    Solution bestOfAll;
    bestOfAll.penalty = INFINITY;

    for (int i = 0; i < constructions; i++)
    {
        Solution s;

        start = std::chrono::high_resolution_clock::now();
        s.greedyBuild();
        end = std::chrono::high_resolution_clock::now();
        construction_times.push_back(std::chrono::duration<double>(end - start).count());
        construction_penalties.push_back(s.penalty);

        Solution best;
        best.copy(s);

        int iterIls = 0;
        while (iterIls <= maxIterIls)
        {
            start = std::chrono::high_resolution_clock::now();
            RVND(s);
            end = std::chrono::high_resolution_clock::now();
            rvnd_times.push_back(std::chrono::duration<double>(end - start).count());
            rvnd_penalties.push_back(s.penalty);

            if (s.penalty < best.penalty)
            {
                best.copy(s);
                iterIls = 0;
            }
            s.copy(best);
            s.perturbation();
            s.updatePenalty();
            iterIls++;
        }

        if (best.penalty < bestOfAll.penalty)
        {
            bestOfAll.copy(best);
        }
    }

    auto metaheuristic_end = std::chrono::high_resolution_clock::now();
    double metaheuristic_time = std::chrono::duration<double>(metaheuristic_end - metaheuristic_start).count();
    double metaheuristic_penalty = bestOfAll.penalty;

    double construction_mean_time = std::accumulate(construction_times.begin(), construction_times.end(), 0.0) / construction_times.size();
    double rvnd_mean_time = std::accumulate(rvnd_times.begin(), rvnd_times.end(), 0.0) / rvnd_times.size();

    double construction_best_penalty = *std::min_element(construction_penalties.begin(), construction_penalties.end());
    double rvnd_best_penalty = *std::min_element(rvnd_penalties.begin(), rvnd_penalties.end());

    double construction_mean_penalty = std::accumulate(construction_penalties.begin(), construction_penalties.end(), 0) / construction_penalties.size();
    double rvnd_mean_penalty = std::accumulate(rvnd_penalties.begin(), rvnd_penalties.end(), 0) / rvnd_penalties.size();

    std::cout << construction_times.size() << ",";
    std::cout << construction_mean_penalty << ",";
    std::cout << construction_best_penalty << ",";
    std::cout << construction_mean_time << "\n";

    std::cout << rvnd_times.size() << ",";
    std::cout << rvnd_mean_penalty << ",";
    std::cout << rvnd_best_penalty << ",";
    std::cout << rvnd_mean_time << "\n";

    std::cout << metaheuristic_penalty << ",";
    std::cout << metaheuristic_time << std::endl;

    bestOfAll.print();
}
