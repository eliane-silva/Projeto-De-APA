#include "data.h"
#include <iostream>
#include <fstream>

void readData(int argc, char **argv, int *totalRequests, int **productionTimes, int **deadlines, int **delayPenalties, int ***preparationTimes)
{
    if (argc < 2)
    {
        std::cout << "\nFaltando parametros\n";
        std::cout << " ./exec [Instancia] " << std::endl;
        exit(1);
    }
    else if (argc > 2)
    {
        std::cout << "\nMuitos parametros\n";
        std::cout << " ./exec [Instancia] " << std::endl;
        exit(1);
    }

    char *instancePath;
    instancePath = argv[1];

    std::ifstream file(instancePath, std::ios::in);

    if (!file)
    {
        std::cout << "arquivo nao pode ser aberto\n";
        exit(1);
    }

    int totr;
    file >> totr;

    int *prodt = new int[totr];
    for (int i = 0; i < totr; i++)
        file >> prodt[i];

    int *dl = new int[totr];
    for (int i = 0; i < totr; i++)
        file >> dl[i];

    int *dp = new int[totr];
    for (int i = 0; i < totr; i++)
        file >> dp[i];

    int **prept = new int *[totr + 1];

    prept[totr] = new int[totr];
    for (int i = 0; i < totr; i++)
        file >> prept[totr][i];

    for (int i = 0; i < totr; i++)
    {
        prept[i] = new int[totr];
        for (int j = 0; j < totr; j++)
            file >> prept[i][j];
    }

    *totalRequests = totr;
    *productionTimes = prodt;
    *deadlines = dl;
    *delayPenalties = dp;
    *preparationTimes = prept;
}

Data *Data::instance = nullptr;

Data::Data()
{
    this->totalRequests = -1;
    this->productionTimes = nullptr;
    this->deadlines = nullptr;
    this->delayPenalties = nullptr;
    this->preparationTimes = nullptr;
}

Data &Data::getInstance()
{
    if (instance == nullptr)
        instance = new Data();
    return *instance;
}

void Data::read(int argc, char **argv)
{
    int totalRequests;
    int *productionTimes;
    int *deadlines;
    int *delayPenalties;
    int **preparationTimes;

    readData(argc, argv, &totalRequests, &productionTimes, &deadlines, &delayPenalties, &preparationTimes);
    instance->totalRequests = totalRequests;
    instance->productionTimes = productionTimes;
    instance->deadlines = deadlines;
    instance->delayPenalties = delayPenalties;
    instance->preparationTimes = preparationTimes;
}

void Data::clear()
{
    Data &data = Data::getInstance();

    delete[] data.productionTimes;
    delete[] data.deadlines;
    delete[] data.delayPenalties;

    for (int i = 0; i <= data.totalRequests; i++)
        delete[] data.preparationTimes[i];

    delete[] data.preparationTimes;

    // Resetar os ponteiros e variáveis para valores seguros
    data.totalRequests = -1;
    data.productionTimes = nullptr;
    data.deadlines = nullptr;
    data.delayPenalties = nullptr;
    data.preparationTimes = nullptr;
}
