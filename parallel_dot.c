//Programa que calcula el producto punto por segmentación.

#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>


#define MAX_LOCAL_ORDER 300000

int main(int argc, char* argv[]) {
    float  local_x[MAX_LOCAL_ORDER];
    float  local_y[MAX_LOCAL_ORDER];
    int    n;
    int    n_bar;  /* = n/p */
    float  dot;
    int    p;
    int    my_rank;

    srand((unsigned int)time(NULL));

    void Read_vector(float local_v[], int n_bar, int p, int my_rank);
    
    float Parallel_dot(float local_x[], float local_y[], int n_bar);
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

//    if (my_rank == 0) {
//        printf("Enter the order of the vectors\n");
//        scanf("%d", &n);
//    }

    if (my_rank == 0) {
       n = 262144;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    n_bar = n/p;

    Read_vector(local_x, n_bar, p, my_rank);
    Read_vector(local_y, n_bar, p, my_rank);
    dot = Parallel_dot(local_x, local_y, n_bar);

    if (my_rank == 0)
        printf("The dot product is %f\n", dot);

    MPI_Finalize();
}  /* main */
   
/*****************************************************************/
void Read_vector(
         float  local_v[]  /* out */,
         int    n_bar      /* in  */,
         int    p          /* in  */,
         int    my_rank    /* in  */) {
    int i, q;
    float a = 5.0;
    float temp[MAX_LOCAL_ORDER];
    MPI_Status status;

    if (my_rank == 0) {

       for(i=0; i<n_bar; i++){
          local_v[i]=((float)rand()/(float)(RAND_MAX)) * a;
       }
       for (q = 1; q < p; q++) {
            for (i = 0; i < n_bar; i++){
                //scanf("%f", &temp[i]);
                temp[i]=((float)rand()/(float)(RAND_MAX)) * a;
            }
            MPI_Send(temp, n_bar, MPI_FLOAT, q, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(local_v, n_bar, MPI_FLOAT, 0, 0, MPI_COMM_WORLD,
            &status);
    }
}  /* Read_vector */

/*****************************************************************/
float Serial_dot(
          float  x[]  /* in */, 
          float  y[]  /* in */, 
          int    n    /* in */) {

    int    i; 
    float  sum = 0.0;

    for (i = 0; i < n; i++)
        sum = sum + x[i]*y[i];
    return sum;
} /* Serial_dot */

/*****************************************************************/
float Parallel_dot(
          float  local_x[]  /* in */,
          float  local_y[]  /* in */,
          int  n_bar        /* in */) {

    float  local_dot;
    float  dot = 0.0;
    float  Serial_dot(float x[], float y[], int m);

    local_dot = Serial_dot(local_x, local_y, n_bar);
    MPI_Reduce(&local_dot, &dot, 1, MPI_FLOAT,
        MPI_SUM, 0, MPI_COMM_WORLD);
    return dot;
} /* Parallel_dot */


