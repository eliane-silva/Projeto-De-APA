#include "solution.h"
#include <iostream>

void Solution::print()
{
    std::cout << "Route: ";
    for (int i = 0; i < Data::getInstance().totalRequests; i++)
    {
        std::cout << route[i] << " - ";
    }
    std::cout << route[Data::getInstance().totalRequests] << std::endl;
    std::cout << "Cost: " << cost << std::endl;
}

void Solution::copy(const Solution &other)
{
    route = std::vector<int>(other.route);
    cost = other.cost;
}

struct InsertionInfo
{
    int insertedNode;
    int removedEdge;
    double cost;
};

std::vector<InsertionInfo> calculateInsertionCosts(Solution &s, int solutionSize, std::vector<int> &CL)
{
    Data &data = Data::getInstance();
    std::vector<InsertionInfo> costsList = std::vector<InsertionInfo>((solutionSize - 1) * CL.size());

    int c = 0;
    for (int e = 0; e < solutionSize - 1; e++)
    {
        int i = s.route[e];
        int j = s.route[e + 1];

        for (int k : CL)
        {
            costsList[c].cost = data.matrizAdj[i][k] + data.matrizAdj[k][j] - data.matrizAdj[i][j];
            costsList[c].insertedNode = k;
            costsList[c].removedEdge = e;
            c++;
        }
    }

    std::sort(costsList.begin(), costsList.end(), [](InsertionInfo a, InsertionInfo b)
              { return a.cost < b.cost; });

    return costsList;
}

void Solution::greedyBuild()
{
    Data &data = Data::getInstance();
    int i;

    // Criação do CL
    std::vector<int> CL;
    for (i = 2; i <= data.totalRequests; i++)
        CL.push_back(i);

    // Escolha de 3 nós aleatórios
    std::set<int> initialNodes;
    while (initialNodes.size() < 3)
        initialNodes.insert(2 + std::rand() % (data.totalRequests - 1));

    // Inserção dos nós e ajuste do CL
    i = 0;
    route[i++] = 1;
    for (int node : initialNodes)
    {
        cost += data.matrizAdj[route[i - 1]][node];
        route[i++] = node;
        CL.erase(std::find(CL.begin(), CL.end(), node));
    }
    cost += data.matrizAdj[route[i - 1]][1];
    route[i] = 1;

    int solutionSize = 5;
    while (!CL.empty())
    {
        std::vector<InsertionInfo> costsList = calculateInsertionCosts(*this, solutionSize, CL);
        double alpha = std::min((double)rand() / RAND_MAX + 1e-6, 1.0);

        int selectedNode = rand() % ((int)ceil(alpha * costsList.size()));
        InsertionInfo addedNode = costsList[selectedNode];

        route.insert(route.begin() + addedNode.removedEdge + 1, addedNode.insertedNode);
        cost += addedNode.cost;
        solutionSize++;
        CL.erase(std::find(CL.begin(), CL.end(), addedNode.insertedNode));
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

        a_subtrair = data.matrizAdj[route[i - 1]][route[i]] + data.matrizAdj[route[j]][route[j + 1]]; // arcos que serão "cortados" da solução
        a_somar = data.matrizAdj[route[i - 1]][route[j]] + data.matrizAdj[route[i]][route[j + 1]];    // arcos que serão "adicionados" na solução
    }
    else
    {
        a_subtrair = data.matrizAdj[route[i - 1]][route[i]] + data.matrizAdj[route[i]][route[i + 1]] + data.matrizAdj[route[j - 1]][route[j]] + data.matrizAdj[route[j]][route[j + 1]];

        a_somar = data.matrizAdj[route[i - 1]][route[j]] + data.matrizAdj[route[j]][route[i + 1]] + data.matrizAdj[route[j - 1]][route[i]] + data.matrizAdj[route[i]][route[j + 1]];
    }

    delta = a_somar - a_subtrair; // calcula a diferença de custo da solução após sofrer um movimento

    //

    return delta;
}

double Solution::evaluate2Opt(const int i, const int j)
{
    Data &data = Data::getInstance();

    double a_subtrair, a_somar, delta;

    a_subtrair = data.matrizAdj[route[i - 1]][route[i]] + data.matrizAdj[route[j]][route[j + 1]];
    a_somar = data.matrizAdj[route[i]][route[j + 1]] + data.matrizAdj[route[j]][route[i - 1]];

    delta = a_somar - a_subtrair;

    return delta;
}

double Solution::evaluateOrOpt(const int i, const int j, const int n)
{
    Data &data = Data::getInstance();

    double a_subtrair, a_somar, delta;

    a_subtrair = data.matrizAdj[route[i]][route[i + 1]] + data.matrizAdj[route[j]][route[j - 1]] + data.matrizAdj[route[j + n - 1]][route[j + n]];
    a_somar = data.matrizAdj[route[i]][route[j]] + data.matrizAdj[route[j + n - 1]][route[i + 1]] + data.matrizAdj[route[j - 1]][route[j + n]];

    delta = a_somar - a_subtrair;

    return delta;
}

double Solution::evaluateDoubleBridge(const int pos1, const int len1, const int pos2, const int len2)
{
    Data &data = Data::getInstance();

    double a_subtrair, a_somar, delta;

    if (pos1 + len1 == pos2)
    {
        a_subtrair = data.matrizAdj[route[pos1 - 1]][route[pos1]] + data.matrizAdj[route[pos2 - 1]][route[pos2]] + data.matrizAdj[route[pos2 + len2 - 1]][route[pos2 + len2]];
        a_somar = data.matrizAdj[route[pos1 - 1]][route[pos2]] + data.matrizAdj[route[pos2 + len2 - 1]][route[pos1]] + data.matrizAdj[route[pos2 - 1]][route[pos2 + len2]];
    }
    else
    {
        a_subtrair = data.matrizAdj[route[pos1 - 1]][route[pos1]] + data.matrizAdj[route[pos1 + len1 - 1]][route[pos1 + len1]] + data.matrizAdj[route[pos2 - 1]][route[pos2]] + data.matrizAdj[route[pos2 + len2 - 1]][route[pos2 + len2]];
        a_somar = data.matrizAdj[route[pos1 - 1]][route[pos2]] + data.matrizAdj[route[pos2 + len2 - 1]][route[pos1 + len1]] + data.matrizAdj[route[pos2 - 1]][route[pos1]] + data.matrizAdj[route[pos1 + len1 - 1]][route[pos2 + len2]];
    }
    delta = a_somar - a_subtrair;

    return delta;
}

void Solution::swap(const int i, const int j)
{
    Data &data = Data::getInstance();
    cost += evaluateSwap(i, j); // somar a diferença de custo da solução após sofrer um movimento
                                // implica em atualizar o custo da solução
    int aux = route[i];
    route[i] = route[j];
    route[j] = aux;
}

void Solution::twoOpt(const int i, const int j)
{
    Data &data = Data::getInstance();
    cost += evaluate2Opt(i, j);

    std::reverse(route.begin() + i, route.begin() + j + 1);
}

void Solution::orOpt(const int i, const int j, const int n)
{
    Data &data = Data::getInstance();
    cost += evaluateOrOpt(i, j, n);

    std::vector<int> temp(route.begin() + j, route.begin() + j + n);

    route.erase(route.begin() + j, route.begin() + j + n);

    if (i > j)
        route.insert(route.begin() + i - n + 1, temp.begin(), temp.end());
    else
        route.insert(route.begin() + i + 1, temp.begin(), temp.end());
}

void Solution::doubleBridge(const int pos1, const int len1, const int pos2, const int len2)
{
    Data &data = Data::getInstance();
    cost += evaluateDoubleBridge(pos1, len1, pos2, len2);

    std::vector<int> tmp1(route.begin() + pos1, route.begin() + pos1 + len1);
    std::vector<int> tmp2(route.begin() + pos2, route.begin() + pos2 + len2);

    route.erase(route.begin() + pos2, route.begin() + pos2 + len2);
    route.insert(route.begin() + pos2, tmp1.begin(), tmp1.end());

    route.erase(route.begin() + pos1, route.begin() + pos1 + len1);
    route.insert(route.begin() + pos1, tmp2.begin(), tmp2.end());
}