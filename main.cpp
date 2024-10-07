#include "data.h"
#include "localSearch.h"
#include <chrono>
#include <iostream>

void executeAlgorithm(char **argv)
{
    Data &data = Data::getInstance();

    data.read(2, argv);

    auto start = std::chrono::high_resolution_clock::now();

    int maxIterILS = data.totalRequests;
    Solution s = ILS(50, maxIterILS);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> programDuration = end - start;
    std::cout << "\nChosen instance: " << argv[1] << '\n';
    std::cout << "Number of juices: " << data.totalRequests << '\n';
    std::cout << "Algorithm execution time: " << programDuration.count() << " seconds\n";
    std::cout << "\nFinal solution:" << std::endl;
    s.print();
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cout << "\nNo arguments were provided. Using a default instance: 'instancias/n60A.txt'.\n";
        argc++;
        argv[1] = (char *)"instancias/n60A.txt";
    }
    else if (argc > 2)
    {
        std::cout << "Too many arguments provided, terminating program execution.\n";
        std::cout << "Please use the following format: suco.exe [Instance Path].\n";
        return 1;
    }

    srand(time(NULL));
    executeAlgorithm(argv);

    return 0;
}