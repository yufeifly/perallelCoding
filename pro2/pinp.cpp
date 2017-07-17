#include <iostream>
#include <stdlib.h>
#include <omp.h>
using namespace std;
const long long int cyl = 100000000;

int main(int argc, char const *argv[])
{
    double x, y, dis;
    long long int incly = 0;
    unsigned int seed = 1234;
    for (long long int i = 0; i < cyl; ++i)
    {
        x = (double)rand_r(&seed)/(double)RAND_MAX;
        y = (double)rand_r(&seed)/(double)RAND_MAX;
        dis = x * x + y * y ;
        // cout << x << endl;
        if(dis < 1.0)
            incly++;
    }
    double pi = (double)incly / cyl * 4.0;
    cout << pi << endl;
    return 0;
}
