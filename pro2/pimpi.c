#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
const long long int total_cyl = 100000000;

int main(int argc, char *argv[])
{
    int rank, tot;
    MPI_Status state;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tot);
    double x, y, dis;
    long long int total_incly = 0, incly = 0;
    long long int cyl = total_cyl/tot;
    unsigned int seed = 1234 + 17*rank;
    for (long long int i = 0; i < cyl; ++i)
    {
        x = (double)rand_r(&seed)/(double)RAND_MAX;
        y = (double)rand_r(&seed)/(double)RAND_MAX;
        dis = x * x + y * y ;
        // cout << x << endl;
        if(dis < 1.0)
            incly++;
    }
    MPI_Reduce(&incly, &total_incly, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Finalize();

    if(rank == 0)
    {
        double pi = (double)total_incly / total_cyl * 4.0;
        printf("%f\n", pi);
    }
    return 0;
}
