#ifndef DATA_H
#define DATA_H

#include <vector>

class Data
{

private:
    static Data *instance;
    Data();

public:
    int totalRequests;
    int *productionTimes;
    int *deadlines;
    int *delayPenalties;
    int **preparationTimes;

    static Data &getInstance();

    static void read(int, char **);
};
extern void readData(int, char **, int *, int **, int **, int **, int ***);
#endif // DATA_H
