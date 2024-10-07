#ifndef DATA_H
#define DATA_H

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

    static void clear();
};

void readData(int, char **, int *, int **, int **, int **, int ***);

#endif // DATA_H
