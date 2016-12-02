#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 200


void printMe(int m[N][N]){
	int i, j;
	for (i = 0; i < N; ++i){
		for (j = 0; j < N; ++j){
			printf("%i ", m[i][j]);
		}
		printf("\n");
	}
}

void printYou(int n[N*N]){
	int i;
	for (i = 0; i < N*N; ++i){
		printf("%i ", n[i]);
	}
	printf("\n");
}

// int checkSquare(int a, int b, int c, int d, int m[N][N]){
// 	int sum, summation;

// 	sum = 0;
// 	summation = 0;
// 	sum = sum + m[a][a];
// 	sum = sum + m[a][b];
// 	sum = sum + m[a][c];
// 	sum = sum + m[a][d];
// 	summation = summation + sum;

// 	if(sum < 2 || sum > 3){
// 		return 0;
// 	}

// 	sum = 0;
// 	sum = sum + m[b][a];
// 	sum = sum + m[b][b];
// 	sum = sum + m[b][c];
// 	sum = sum + m[b][d];
// 	summation = summation + sum;

// 	if(sum < 2 || sum > 3){
// 		return 0;
// 	}

// 	sum = 0;
// 	sum = sum + m[c][a];
// 	sum = sum + m[c][b];
// 	sum = sum + m[c][c];
// 	sum = sum + m[c][d];
// 	summation = summation + sum;

// 	if(sum < 2 || sum > 3){
// 		return 0;
// 	}

// 	sum = 0;
// 	sum = sum + m[d][a];
// 	sum = sum + m[d][b];
// 	sum = sum + m[d][c];
// 	sum = sum + m[d][d];		
// 	summation = summation + sum;

// 	if(sum < 2 || sum > 3){
// 		return 0;
// 	}

// 	if(summation == 12){
// 		return 3;
// 	}
// 	return 1;
// }

int main(int argc, char **argv){
	
	int i, j, k, l;
	int count = 0;;
	int m[N][N];
	int n[N*N];


	int rank, size;
    int buf;

    const int root=0;

	srand(time(NULL));

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int globalsum = 0;
    int localsum = 0;

	int sizes[size];
	int disp[size + 1];

	/* Distribute the vetices to each processor */
	for (i = 0; i < size; ++i){

		/* Sizes */
		if (i < N%size){ // catch remainder
			sizes[i] = N/size + 1;
		}else{
			sizes[i] = N/size;
			
		}

		/* Displacement */
		if(i == 0){
			disp[i] = 0;
		}else{
			disp[i] = disp[i-1] + sizes[i-1];
		}

	}

	disp[size] = N; // last displacement

    if(rank == 0){
    	/* Initialization of matrix */
		for (i = 0; i < N; ++i){
			for (j = 0; j < N; ++j){
				m[i][j] = 0;
			}
		}

		/* Biuld matrix */
		for (i = 0; i < N; ++i){
			for (j = i; j < N; ++j){
				if(i == j){
					continue;
				}
				m[i][j] = (rand() % 2);
				// m[i][j] = 1;
				m[j][i] = m[i][j];
			}
		}

		/* String to broadcast */
		k = 0;
		for (i = 0; i < N; ++i){
			for (j = 0; j < N; ++j){
				n[k] = m[i][j];
				k++;
			}
		}
    }

    /* Broadcast adjacency matrix */
    MPI_Bcast(&n, N*N, MPI_INT, 0, MPI_COMM_WORLD);

    /* Recreate the array to matrix */
	k = 0;    
    for (i = 0; i < N; ++i){
		for (j = 0; j < N; ++j){
			m[i][j] = n[k];
			k++;
		}
	}

	/* Count the squares */
	count = 0;
	for (i = disp[rank]; i < disp[rank+1]; ++i){
		for (j = 0; j < N; ++j){
			if(m[i][j] == 1){
				for (k = 0; k < N; ++k){
					if(m[j][k] == 1 && k != i && k != j){
						for (l = 0; l < N; ++l){
							if(m[k][l] == 1 && l != k && l != j){
								if(m[l][i] == 1){
									// printf("%d %d %d %d %d\n", i, j, k, l, i);
									count++;
								}
							}
						}
					}
				}		
			}
		}
	}

	localsum = count/2; // number of square in the given vertices
		
	MPI_Reduce(&localsum,&globalsum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD); // add all the data

	if(rank==0){
		printf("\n");
		printf("N = %i\n", N);
		// printMe(m);
		printf("\n");
        printf("Sqaures = %d \n",globalsum/4);
    }

    MPI_Finalize();

	return 0;
}