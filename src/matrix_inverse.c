/***************************************************************************
 *
 * Sequential version of Matrix Inverse
 * An adapted version of the code by H�kan Grahn
 ***************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "../include/formula.h"

#define MAX_SIZE 4096

typedef double matrix[MAX_SIZE][MAX_SIZE];

// extern int N;
int	M;		/* matrix size		*/
int	maxnum;		/* max number of element*/
char* Init;		/* matrix init type	*/
int	PRINT;		/* print switch		*/
matrix	A;		/* matrix A		*/
matrix I = {0.0};  /* The A inverse matrix, which will be initialized to the identity matrix */

/* forward declarations */
void* find_inverse(void*);
void Init_Matrix(void);
void Print_Matrix(matrix Mat, char name[]);
void Init_Default(void);
int Read_Options(int, char**);
void write_matrix_tofile(int soln, int clientId);

int main_matinv(int argc, char** argv, int soln, int clientId)
{
    // printf("Matrix Inverse\n");
    int i, timestart, timeend, iter;

    Init_Default();		/* Init default values	*/
    Read_Options(argc, argv);	/* Read arguments	*/
    Init_Matrix();		/* Init the matrix	*/

    // declaring four threads
    pthread_t *threads = (pthread_t*)malloc(M * sizeof(pthread_t));
    for (int i = 0; i < M; i++) {
        int* p;
        pthread_create(&threads[i], NULL, find_inverse, (void*)(p));
        pthread_join(threads[i], NULL);
    }

    if (PRINT == 1)
    {
        //Print_Matrix(A, "End: Input");
        Print_Matrix(I, "Inversed");
    }

    write_matrix_tofile(soln, clientId);
}

int step = 0;
void* find_inverse(void* arg)
{
    int row, col, p; // 'p' stands for pivot (numbered from 0 to N-1)
    double pivalue; // pivot value

    p = step++;

    /* Bringing the matrix A to the identity form */
    // for (p = 0; p < M; p++) 
    { 
        pivalue = A[p][p];
        for (col = 0; col < M; col++)
        {
            A[p][col] = A[p][col] / pivalue; /* Division step on A */
            I[p][col] = I[p][col] / pivalue; /* Division step on I */
        }
        assert(A[p][p] == 1.0);

        double multiplier;
        for (row = 0; row < M; row++) {
            multiplier = A[row][p];
            if (row != p) // Perform elimination on all except the current pivot row 
            {
                for (col = 0; col < M; col++)
                {
                    A[row][col] = A[row][col] - A[p][col] * multiplier; /* Elimination step on A */
                    I[row][col] = I[row][col] - I[p][col] * multiplier; /* Elimination step on I */
                }      
                assert(A[row][p] == 0.0);
            }
        }
    }

}

void Init_Matrix()
{
    int row, col;

    // Set the diagonal elements of the inverse matrix to 1.0
    // So that you get an identity matrix to begin with
    for (row = 0; row < M; row++) {
        for (col = 0; col < M; col++) {
            if (row == col)
                I[row][col] = 1.0;
        }
    }

    // printf("\nsize      = %dx%d ", M, M);
    // printf("\nmaxnum    = %d \n", maxnum);
    // printf("Init	  = %s \n", Init);
    // printf("Initializing matrix...");

    if (strcmp(Init, "rand") == 0) {
        for (row = 0; row < M; row++) {
            for (col = 0; col < M; col++) {
                if (row == col) /* diagonal dominance */
                    A[row][col] = (double)(rand() % maxnum) + 5.0;
                else
                    A[row][col] = (double)(rand() % maxnum) + 1.0;
            }
        }
    }
    if (strcmp(Init, "fast") == 0) {
        for (row = 0; row < M; row++) {
            for (col = 0; col < M; col++) {
                if (row == col) /* diagonal dominance */
                    A[row][col] = 5.0;
                else
                    A[row][col] = 2.0;
            }
        }
    }

    printf("done \n\n");
    if (PRINT == 1)
    {
        //Print_Matrix(A, "Begin: Input");
        //Print_Matrix(I, "Begin: Inverse");
    }
}

void Print_Matrix(matrix Mat, char name[])
{
    int row, col;

    printf("%s Matrix:\n", name);
    for (row = 0; row < M; row++) {
        for (col = 0; col < M; col++)
            printf(" %5.2f", Mat[row][col]);
        printf("\n");
    }
    printf("\n\n");
}

void write_matrix_tofile(int soln, int clientId)
{
    char fileName[128];
    sprintf(fileName, "../computed_results/matinv_client%d_soln%d.txt", clientId, soln);
    FILE* fp = fopen(fileName, "w");
    if (fp == NULL) {
        perror("Cannot open the file");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(fp, "A=\n");
        for (int row = 0; row < M; row++) {
            for (int col = 0; col < M; col++)
                fprintf(fp, " %5.2f", A[row][col]);
            fprintf(fp, "\n");
        }

        fprintf(fp, "\nI=\n");
        for (int row = 0; row < M; row++) {
            for (int col = 0; col < M; col++)
                fprintf(fp, " %5.2f", I[row][col]);
            fprintf(fp, "\n");
        }        
    }
    // printf("Wrote the results to a file!\n");
    fclose(fp);
} 

void Init_Default()
{
    M = 5;
    Init = "fast";
    maxnum = 15.0;
    PRINT = 1;
}

int Read_Options(int argc, char** argv)
{
    char* prog;

    prog = *argv;
    while (++argv, --argc > 0)
        if (**argv == '-')
            switch (*++ * argv) {
            case 'n':
                --argc;
                M = atoi(*++argv);
                break;
            case 'h':
                printf("\nHELP: try matinv -u \n\n");
                exit(0);
                break;
            case 'u':
                printf("\nUsage: matinv [-n problemsize]\n");
                printf("           [-D] show default values \n");
                printf("           [-h] help \n");
                printf("           [-I init_type] fast/rand \n");
                printf("           [-m maxnum] max random no \n");
                printf("           [-P print_switch] 0/1 \n");
                exit(0);
                break;
            case 'D':
                printf("\nDefault:  n         = %d ", M);
                printf("\n          Init      = rand");
                printf("\n          maxnum    = 5 ");
                printf("\n          P         = 0 \n\n");
                exit(0);
                break;
            case 'I':
                --argc;
                Init = *++argv;
                break;
            case 'm':
                --argc;
                maxnum = atoi(*++argv);
                break;
            case 'P':
                --argc;
                PRINT = atoi(*++argv);
                break;
            default:
                printf("%s: ignored option: -%s\n", prog, *argv);
                printf("HELP: try %s -u \n\n", prog);
                break;
            }
}
