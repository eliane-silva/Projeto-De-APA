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

double calculateSingleInsertionDelta(Solution &s, int insertionPosition, int selectedJuice)
{
    Data &data = Data::getInstance();
    int actualTime = 0;
    double penalty = 0;
    int solutionSize = s.sequence.size();

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

std::vector<InsertionInfo> calculateInsertionPenaltiesList(Solution &s, std::vector<int> &CL)
{
    Data &data = Data::getInstance();
    int solutionSize = s.sequence.size();
    std::vector<InsertionInfo> penaltiesList = std::vector<InsertionInfo>((solutionSize + 1) * CL.size());

    int penaltyListIndex = 0;
    for (int insertPosition = 0; insertPosition <= solutionSize; insertPosition++)
    {
        for (int juice : CL)
        {
            penaltiesList[penaltyListIndex].penalty = calculateSingleInsertionDelta(s, insertPosition, juice);
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
    sequence.insert(sequence.begin(), initialJuice);

    // Inserção do suco na sequência e ajuste do Candidate List
    int actualTime = data.preparationTimes[data.totalRequests][sequence[0]];
    actualTime += data.productionTimes[sequence[0]];
    penalty = std::max(0, (actualTime - data.deadlines[sequence[0]]) * data.delayPenalties[sequence[0]]);
    CL.erase(std::find(CL.begin(), CL.end(), sequence[0]));

    while (!CL.empty())
    {
        std::vector<InsertionInfo> penaltiesList = calculateInsertionPenaltiesList(*this, CL);
        double alpha = 0.1;

        int selectedJuice = rand() % ((int)std::ceil(alpha * penaltiesList.size()));
        InsertionInfo addedJuice = penaltiesList[selectedJuice];

        sequence.insert(sequence.begin() + addedJuice.insertPosition, addedJuice.selectedJuice);
        penalty += addedJuice.penalty;
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

double Solution::evaluateSwap(const int pos1, const int pos2)
{
    Data &data = Data::getInstance();

    int actualTime = 0;
    double newPenalty = 0;

    if (pos1 == pos2 - 1)
    {
        if (pos1 == 0)
        {
            int juice1 = this->sequence[pos1];
            int juice2 = this->sequence[pos2];
            int juiceAfter2 = this->sequence[pos2 + 1];

            Solution::connect(data, actualTime, newPenalty, data.totalRequests, juice2);
            Solution::connect(data, actualTime, newPenalty, juice2, juice1);
            Solution::connect(data, actualTime, newPenalty, juice1, juiceAfter2);

            for (int i = pos2 + 2; i < data.totalRequests; i++)
            {
                Solution::connect(data, actualTime, newPenalty, this->sequence[i - 1], this->sequence[i]);
            }
        }
        else if (pos2 == data.totalRequests - 1)
        {
            int juice1 = this->sequence[pos1];
            int juice2 = this->sequence[pos2];
            int juiceBefore1 = this->sequence[pos1 - 1];

            Solution::connect(data, actualTime, newPenalty, data.totalRequests, this->sequence[0]);

            for (int i = 1; i < pos1; i++)
            {
                Solution::connect(data, actualTime, newPenalty, this->sequence[i - 1], this->sequence[i]);
            }

            Solution::connect(data, actualTime, newPenalty, juiceBefore1, juice2);
            Solution::connect(data, actualTime, newPenalty, juice2, juice1);
        }
        else
        {
            int juice1 = this->sequence[pos1];
            int juice2 = this->sequence[pos2];
            int juiceBefore1 = this->sequence[pos1 - 1];
            int juiceAfter2 = this->sequence[pos2 + 1];

            Solution::connect(data, actualTime, newPenalty, data.totalRequests, this->sequence[0]);

            for (int i = 1; i < pos1; i++)
            {
                Solution::connect(data, actualTime, newPenalty, this->sequence[i - 1], this->sequence[i]);
            }

            Solution::connect(data, actualTime, newPenalty, juiceBefore1, juice2);
            Solution::connect(data, actualTime, newPenalty, juice2, juice1);
            Solution::connect(data, actualTime, newPenalty, juice1, juiceAfter2);

            for (int i = pos2 + 2; i < data.totalRequests; i++)
            {
                Solution::connect(data, actualTime, newPenalty, this->sequence[i - 1], this->sequence[i]);
            }
        }
    }
    else
    {
        if (pos1 == 0)
        {
            int juice2 = this->sequence[pos2];
            int juiceAfter1 = this->sequence[pos1 + 1];

            Solution::connect(data, actualTime, newPenalty, data.totalRequests, juice2);
            Solution::connect(data, actualTime, newPenalty, juice2, juiceAfter1);

            for (int i = pos1 + 2; i < pos2; i++)
            {
                Solution::connect(data, actualTime, newPenalty, this->sequence[i - 1], this->sequence[i]);
            }
        }
        else
        {
            int juice2 = this->sequence[pos2];
            int juiceBefore1 = this->sequence[pos1 - 1];
            int juiceAfter1 = this->sequence[pos1 + 1];

            Solution::connect(data, actualTime, newPenalty, data.totalRequests, this->sequence[0]);

            for (int i = 1; i < pos1; i++)
            {
                Solution::connect(data, actualTime, newPenalty, this->sequence[i - 1], this->sequence[i]);
            }

            Solution::connect(data, actualTime, newPenalty, juiceBefore1, juice2);
            Solution::connect(data, actualTime, newPenalty, juice2, juiceAfter1);

            for (int i = pos1 + 2; i < pos2; i++)
            {
                Solution::connect(data, actualTime, newPenalty, this->sequence[i - 1], this->sequence[i]);
            }
        }

        int juice1 = this->sequence[pos1];
        int juiceBefore2 = this->sequence[pos2 - 1];

        Solution::connect(data, actualTime, newPenalty, juiceBefore2, juice1);

        if (pos2 != data.totalRequests - 1)
        {
            int juiceAfter2 = this->sequence[pos2 + 1];

            Solution::connect(data, actualTime, newPenalty, juice1, juiceAfter2);

            for (int i = pos2 + 2; i < data.totalRequests; i++)
            {
                Solution::connect(data, actualTime, newPenalty, this->sequence[i - 1], this->sequence[i]);
            }
        }
    }

    return newPenalty - this->penalty;
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

    int actualTime = 0;
    double newPenalty = 0;

    //mesma sequência
    if(i == j-1)
    {
        return 0;
    }

    //i<j
    else if(i < j)
    {   
        //conexão do i ou até o i
        if(i == 0)
        {
            connect(data, actualTime, penalty, data.totalRequests, this->sequence[i]);
        }
        else{
            connect(data, actualTime, penalty, data.totalRequests, this->sequence[0]);
            for(int k = 0; k < i; k++)
            {
                connect(data, actualTime, penalty, this->sequence[k], this->sequence[k+1]);
            }
        }

        //conectando o i ate o j, como ja foi validado se i == j - 1, não preciso verificar se há algo entre os dois, pois sempre vai haver
        for(int k = i; k < j; k++)
        {
            connect(data, actualTime, penalty, this->sequence[k], this->sequence[k+1]);
        }

        //adicionando o j depois o i
        for(int l = j; l < j + n - 1; l++)
        {
            connect(data, actualTime, penalty, this->sequence[l], this->sequence[l+1]);
        }

        //verificando se há algo depois do j+n
        if(j+n-1<data.totalRequests)
        {
            for(int m= j + n - 1; m < data.totalRequests; m++)
            {
                connect(data, actualTime, penalty, this->sequence[m], this->sequence[m+1]);
            }
        }       
    }

    //j<i
    else
    {   
        //caso o j seja a primeira posição
        if(j == 0)
        {
            //verificando se há algo entre j+n ate o i
            //conexão até o i
            if(j+n == i)
            {
                connect(data, actualTime, penalty, data.totalRequests, this->sequence[i]);
            }
            else
            {
                connect(data, actualTime, penalty, data.totalRequests, this->sequence[j+n]);

                for(int k = j + n; k < i; k++)
                {
                    connect(data, actualTime, penalty, this->sequence[k], this->sequence[k+1]);
                }
            }
        }

        //caso o j nao seja a primeira posição
        else{
            connect(data, actualTime, penalty, data.totalRequests, this->sequence[0]);
            for(int k = 0; k < j - 1; k++)
            {
                connect(data, actualTime, penalty, this->sequence[k], this->sequence[k+1]);
            }

            //verificando se há algo entre j+n ate o i
            //conexão até o i
            if(j+n == i)
            {
                connect(data, actualTime, penalty, this->sequence[j+n-1], this->sequence[i]);
            }
            else
            {
                for(int k = j + n - 1; k < i; k++)
                {
                    connect(data, actualTime, penalty, this->sequence[k], this->sequence[k+1]);
                }
            }
        }

        //conectando o j + n depois do i
        for(int k = i; k < j + n - 1; k++)
        {
            connect(data, actualTime, penalty, this->sequence[k], this->sequence[k+1]);
        }

        //verificando se há algo depois do i para inserir depois do j + n
        if(i == data.totalRequests)
        {
            for(int k = j + n - 1; k < data.totalRequests; k++)
            {
                connect(data, actualTime, penalty, this->sequence[k], this->sequence[k+1]);
            }
        } 
    }

    return newPenalty - this->penalty;
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