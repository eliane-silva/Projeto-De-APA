#include "data.h"
#include <time.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "localSearch.h"

void executeAlgorithm(const std::string &path_output, std::string &instance)
{
    Data &data = Data::getInstance();

    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time = *std::localtime(&time);

    int min = local_time.tm_min;
    int sec = local_time.tm_sec;
    int milisec = static_cast<int>(now_ms.count());

    std::ostringstream time_stream;
    time_stream << std::setfill('0') << std::setw(2)
                << min << "-"
                << std::setw(2) << sec << "-"
                << std::setw(3) << milisec;

    std::string time_str = time_stream.str();

    std::string output_file = path_output + "\\" + instance + "\\" + instance + "_" + time_str + ".txt";

    std::ofstream file(output_file);

    std::cout << "Saida redirecionada para o arquivo: " << output_file << std::endl;
    std::streambuf *cout_buffer = std::cout.rdbuf();
    std::cout.rdbuf(file.rdbuf());

    int max_iter_ILS = data.totalRequests >= 150 ? data.totalRequests / 2 : data.totalRequests;
    ILSBenchmark(50, max_iter_ILS);

    std::cout.rdbuf(cout_buffer);
    file.close();
}

int main()
{
    std::string path_input_lucas = "C:\\Users\\Lucas\\Documents\\GitHub\\Projeto-De-APA\\instancias";
    std::string path_input_eliane = "";
    std::string path_input_weslley = "";

    std::string path_output_lucas = "C:\\Users\\Lucas\\Documents\\GitHub\\Projeto-De-APA\\benchmark\\instancias";
    std::string path_output_eliane = "";
    std::string path_output_weslley = "";

    std::string instances[] = {
        "n60A", "n60B", "n60C", "n60D", "n60E",
        "n60F", "n60G", "n60H", "n60I", "n60J",
        "n60K", "n60L", "n60M", "n60N", "n60O", "n60P"};

    std::string path_input = path_input_lucas;
    std::string path_output = path_output_lucas;

    int executions = 5;

    int numInstances = sizeof(instances) / sizeof(instances[0]);
    std::cout << "\nTotal de instancias que serao executadas: " << numInstances * executions << std::endl;

    Data &data = Data::getInstance();
    for (int i = 0; i < numInstances; i++)
    {
        std::string instance_path = path_input + "\\" + instances[i] + ".txt";
        char *argv[2];
        argv[1] = const_cast<char *>(instance_path.c_str());

        data.read(2, argv);
        std::cout << "\n\nInstancia atual: " << instances[i] << "\n";
        for (int j = 0; j < executions; j++)
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto ticks = now.time_since_epoch().count();
            int seed = static_cast<int>(ticks & 0x0000FFFF);
            srand(seed);
            std::cout << "\nIniciando execucao {" << j + 1 << "/" << executions << "} da instancia: " << instances[i] << "\n";
            executeAlgorithm(path_output, instances[i]);
        }
        data.clear();
    }

    return 0;
}