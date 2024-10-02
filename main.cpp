#include "data.h"
#include <time.h>
#include <chrono>
#include <iostream>
#include "localSearch.h"

void executeTSPAlgorithm(char **argv)
{
    Data &data = Data::getInstance();

    data.read(2, argv);

    auto start = std::chrono::high_resolution_clock::now();

    Solution s = solve();
    // int maxIterILS = data.totalRequests >= 150 ? data.totalRequests / 2 : data.totalRequests;
    // Solution s = ILS(50, maxIterILS);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> programDuration = end - start;
    std::cout << "\nChosen instance: " << argv[1] << '\n';
    std::cout << "Number of vertices: " << data.totalRequests << '\n';
    std::cout << "Algorithm execution time: " << programDuration.count() << " seconds\n";
    std::cout << "\nFinal solution:" << std::endl;
    s.print();
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cout << "\nNo arguments were provided. Using a default instance: 'instancias/exemplo_simples.txt'.\n";
        argc++;
        argv[1] = (char *)"instancias/exemplo_simples.txt";
    }
    else if (argc > 2)
    {
        std::cout << "Too many arguments provided, terminating program execution.\n";
        std::cout << "Please use the following format: tsp.exe [Instance Path].\n";
        return 1;
    }

    srand(time(NULL));
    executeTSPAlgorithm(argv);

    return 0;
}