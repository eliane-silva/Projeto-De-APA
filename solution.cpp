#include "solution.h"
#include <iostream>

void Solution::print()
{
    std::cout << "Penalty: ";
    for (int i = 0; i < Data::getInstance().totalRequests - 1; i++)
    {
        std::cout << sequence[i] << " - ";
    }
    std::cout << sequence[Data::getInstance().totalRequests - 1] << std::endl;
    std::cout << "Total Penalty: " << penalty << std::endl;
}

void Solution::copy(const Solution &other)
{
    sequence = std::vector<int>(other.sequence);
    penalty = other.penalty;
}

struct InsertionInfo
{
    int selectedJuice;
    int insertPosition;
    double penalty;
};

double calculateSingleInsertionDelta(Solution &s, int insertionPosition, int selectedJuice, int solutionSize)
{
    Data &data = Data::getInstance();
    int actualTime = 0;
    double penalty = 0;
    if (insertionPosition == 0)
    {
        Solution::connect(data, actualTime, penalty, data.totalRequests, selectedJuice);
        Solution::connect(data, actualTime, penalty, selectedJuice, s.sequence[0]);

        for (int i = 1; i < solutionSize; i++)
        {
            Solution::connect(data, actualTime, penalty, s.sequence[i - 1], s.sequence[i]);
        }
    }
    else if (insertionPosition == solutionSize)
    {
        Solution::connect(data, actualTime, penalty, data.totalRequests, s.sequence[0]);

        for (int i = 1; i < solutionSize; i++)
        {
            Solution::connect(data, actualTime, penalty, s.sequence[i - 1], s.sequence[i]);
        }

        Solution::connect(data, actualTime, penalty, s.sequence[solutionSize - 1], selectedJuice);
    }
    else 
    {
        Solution::connect(data, actualTime, penalty, data.totalRequests, s.sequence[0]);

        for (int i = 1; i < insertionPosition; i++)
        {
            Solution::connect(data, actualTime, penalty, s.sequence[i - 1], s.sequence[i]);
        }

        Solution::connect(data, actualTime, penalty, s.sequence[insertionPosition - 1], selectedJuice);
        Solution::connect(data, actualTime, penalty, selectedJuice, s.sequence[insertionPosition]);

        for (int i = insertionPosition + 1; i < solutionSize; i++)
        {
            Solution::connect(data, actualTime, penalty, s.sequence[i - 1], s.sequence[i]);
        }
    }

    return penalty - s.penalty;
}

std::vector<InsertionInfo> calculateInsertionPenaltiesList(Solution &s, int solutionSize, std::vector<int> &CL)
{
    Data &data = Data::getInstance();
    std::vector<InsertionInfo> penaltiesList = std::vector<InsertionInfo>((solutionSize + 1) * CL.size());

    int penaltyListIndex = 0;
    for (int insertPosition = 0; insertPosition <= solutionSize; insertPosition++)
    {
        for (int juice : CL)
        {
            penaltiesList[penaltyListIndex].penalty = calculateSingleInsertionDelta(s, insertPosition, juice, solutionSize);
            penaltiesList[penaltyListIndex].selectedJuice = juice;
            penaltiesList[penaltyListIndex].insertPosition = insertPosition;
            penaltyListIndex++;
        }
    }

    std::sort(penaltiesList.begin(), penaltiesList.end(), [](InsertionInfo a, InsertionInfo b) { return a.penalty < b.penalty; });

    return penaltiesList;
}

void Solution::connect(Data &data, int &actualTime, double &penalty, int a, int b)
{
    actualTime += data.preparationTimes[a][b];
    actualTime += data.productionTimes[b];
    penalty += std::max(0, (actualTime - data.deadlines[b]) * data.delayPenalties[b]);
}

void Solution::greedyBuild()
{
    Data &data = Data::getInstance();

    // Criação do Candidate List
    std::vector<int> CL;
    for (int i = 0; i < data.totalRequests; i++)
        CL.push_back(i);

    // Escolha de um suco aleatório
    int initialJuice = std::rand() % data.totalRequests;
    sequence[0] = initialJuice;

    // Inserção do suco na sequência e ajuste do Candidate List
    int actualTime = data.preparationTimes[data.totalRequests][sequence[0]];
    actualTime += data.productionTimes[sequence[0]];
    penalty = std::max(0, (actualTime - data.deadlines[sequence[0]]) * data.delayPenalties[sequence[0]]);
    CL.erase(std::find(CL.begin(), CL.end(), sequence[0]));

    int solutionSize = 1;
    while (!CL.empty())
    {
        std::vector<InsertionInfo> penaltiesList = calculateInsertionPenaltiesList(*this, solutionSize, CL);
        double alpha = 0.1;

        int selectedJuice = rand() % ((int)std::ceil(alpha * penaltiesList.size()));
        InsertionInfo addedJuice = penaltiesList[selectedJuice];

        sequence.insert(sequence.begin() + addedJuice.insertPosition, addedJuice.selectedJuice);
        penalty += addedJuice.penalty;
        solutionSize++;
        CL.erase(std::find(CL.begin(), CL.end(), addedJuice.selectedJuice));
    }
}

void Solution::perturbation()
{
    Data &data = Data::getInstance();

    int maxSegmentSize = std::max(3.0, std::ceil(data.totalRequests / 10.0));
    int len1, len2, pos1, pos2;
    do
    {
        len1 = 2 + rand() % (maxSegmentSize - 2);
        len2 = 2 + rand() % (maxSegmentSize - 2);

        pos1 = rand() % (data.totalRequests - len1 + 1);
        pos2 = rand() % (data.totalRequests - len2 + 1);
        if (pos1 > pos2)
        {
            int tmp = pos1;
            pos1 = pos2;
            pos2 = tmp;
            tmp = len1;
            len1 = len2;
            len2 = tmp;
        }
    } while (pos1 + len1 > pos2);
    doubleBridge(pos1, len1, pos2, len2);
}

double Solution::evaluateSwap(const int i, const int j)
{
    Data &data = Data::getInstance();

    double a_subtrair, a_somar, delta;

    if ((j == i + 1))
    {

        a_subtrair = data.matrizAdj[sequence[i - 1]][sequence[i]] + data.matrizAdj[sequence[j]][sequence[j + 1]]; // arcos que serão "cortados" da solução
        a_somar = data.matrizAdj[sequence[i - 1]][sequence[j]] + data.matrizAdj[sequence[i]][sequence[j + 1]];    // arcos que serão "adicionados" na solução
    }
    else
    {
        a_subtrair = data.matrizAdj[sequence[i - 1]][sequence[i]] + data.matrizAdj[sequence[i]][sequence[i + 1]] + data.matrizAdj[sequence[j - 1]][sequence[j]] + data.matrizAdj[sequence[j]][sequence[j + 1]];

        a_somar = data.matrizAdj[sequence[i - 1]][sequence[j]] + data.matrizAdj[sequence[j]][sequence[i + 1]] + data.matrizAdj[sequence[j - 1]][sequence[i]] + data.matrizAdj[sequence[i]][sequence[j + 1]];
    }

    delta = a_somar - a_subtrair; // calcula a diferença de custo da solução após sofrer um movimento

    //

    return delta;
}

double Solution::evaluate2Opt(const int i, const int j)
{
    Data &data = Data::getInstance();

    double a_subtrair, a_somar, delta;

    a_subtrair = data.matrizAdj[sequence[i - 1]][sequence[i]] + data.matrizAdj[sequence[j]][sequence[j + 1]];
    a_somar = data.matrizAdj[sequence[i]][sequence[j + 1]] + data.matrizAdj[sequence[j]][sequence[i - 1]];

    delta = a_somar - a_subtrair;

    return delta;
}

double Solution::evaluateOrOpt(const int i, const int j, const int n)
{
    Data &data = Data::getInstance();

    double a_subtrair, a_somar, delta;

    a_subtrair = data.matrizAdj[sequence[i]][sequence[i + 1]] + data.matrizAdj[sequence[j]][sequence[j - 1]] + data.matrizAdj[sequence[j + n - 1]][sequence[j + n]];
    a_somar = data.matrizAdj[sequence[i]][sequence[j]] + data.matrizAdj[sequence[j + n - 1]][sequence[i + 1]] + data.matrizAdj[sequence[j - 1]][sequence[j + n]];

    delta = a_somar - a_subtrair;

    return delta;
}

double Solution::evaluateDoubleBridge(const int pos1, const int len1, const int pos2, const int len2)
{
    Data &data = Data::getInstance();

    double a_subtrair, a_somar, delta;

    if (pos1 + len1 == pos2)
    {
        a_subtrair = data.matrizAdj[sequence[pos1 - 1]][sequence[pos1]] + data.matrizAdj[sequence[pos2 - 1]][sequence[pos2]] + data.matrizAdj[sequence[pos2 + len2 - 1]][sequence[pos2 + len2]];
        a_somar = data.matrizAdj[sequence[pos1 - 1]][sequence[pos2]] + data.matrizAdj[sequence[pos2 + len2 - 1]][sequence[pos1]] + data.matrizAdj[sequence[pos2 - 1]][sequence[pos2 + len2]];
    }
    else
    {
        a_subtrair = data.matrizAdj[sequence[pos1 - 1]][sequence[pos1]] + data.matrizAdj[sequence[pos1 + len1 - 1]][sequence[pos1 + len1]] + data.matrizAdj[sequence[pos2 - 1]][sequence[pos2]] + data.matrizAdj[sequence[pos2 + len2 - 1]][sequence[pos2 + len2]];
        a_somar = data.matrizAdj[sequence[pos1 - 1]][sequence[pos2]] + data.matrizAdj[sequence[pos2 + len2 - 1]][sequence[pos1 + len1]] + data.matrizAdj[sequence[pos2 - 1]][sequence[pos1]] + data.matrizAdj[sequence[pos1 + len1 - 1]][sequence[pos2 + len2]];
    }
    delta = a_somar - a_subtrair;

    return delta;
}

void Solution::swap(const int i, const int j)
{
    Data &data = Data::getInstance();
    penalty += evaluateSwap(i, j); // somar a diferença de custo da solução após sofrer um movimento
                                // implica em atualizar o custo da solução
    int aux = sequence[i];
    sequence[i] = sequence[j];
    sequence[j] = aux;
}

void Solution::twoOpt(const int i, const int j)
{
    Data &data = Data::getInstance();
    penalty += evaluate2Opt(i, j);

    std::reverse(sequence.begin() + i, sequence.begin() + j + 1);
}

void Solution::orOpt(const int i, const int j, const int n)
{
    Data &data = Data::getInstance();
    penalty += evaluateOrOpt(i, j, n);

    std::vector<int> temp(sequence.begin() + j, sequence.begin() + j + n);

    sequence.erase(sequence.begin() + j, sequence.begin() + j + n);

    if (i > j)
        sequence.insert(sequence.begin() + i - n + 1, temp.begin(), temp.end());
    else
        sequence.insert(sequence.begin() + i + 1, temp.begin(), temp.end());
}

void Solution::doubleBridge(const int pos1, const int len1, const int pos2, const int len2)
{
    Data &data = Data::getInstance();
    penalty += evaluateDoubleBridge(pos1, len1, pos2, len2);

    std::vector<int> tmp1(sequence.begin() + pos1, sequence.begin() + pos1 + len1);
    std::vector<int> tmp2(sequence.begin() + pos2, sequence.begin() + pos2 + len2);

    sequence.erase(sequence.begin() + pos2, sequence.begin() + pos2 + len2);
    sequence.insert(sequence.begin() + pos2, tmp1.begin(), tmp1.end());

    sequence.erase(sequence.begin() + pos1, sequence.begin() + pos1 + len1);
    sequence.insert(sequence.begin() + pos1, tmp2.begin(), tmp2.end());
}