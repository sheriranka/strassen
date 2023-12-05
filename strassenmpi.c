# include <stdlib.h>
# include <stdio.h>
# include <mpi.h>
# define f1 "a.txt"
# define f2 "b.txt"
# define f3 "c.txt"

//change the above to fit whatever
//file youre going for



void printMat(int** m, int n){
	
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++)
			printf("%d ",m[i][j]);
		printf("\n");
	}
	
}


int readSize(FILE *f){
	
	//read rows
	int rows = 0;
	char buffer[1024];
    while (fgets(buffer, 1024, f)){		
		rows++;	
	} 
	
	//printf("size:%d\n",rows);
	
	return rows;
	
}


void readMatrix(FILE *f, int n, int** matrix){
	
	int num;
	
	//Read numbers

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			fscanf(f, "%d,", &num);
			//printf("i:%d, j: %d\n",i,j);
			matrix[i][j] = num;		
			//printf("%d ",matrix[i][j]);
		}
		//printf("\n");
	}
	
	//printf("\n");

	
}

//Function to allocate space for new matrix
void allocateMatrix(int n, int** matrix){
	
	for(int i = 0; i < n; i++)
		matrix[i] = malloc(n*sizeof(int));
	
	for(int i = 0; i  < n; i++){
		for(int j = 0; j < n; j++)
			matrix[i][j] = 0;
	}
	
}

//Creates subdivision matrixes
void subMatrix(int n, int** A, int index_i, int index_j, int** submat){
	
	//Allocate new size matrix
	int m = n/2;
	allocateMatrix(m,submat);
	
	//Assign values based on offset values of bigger matrix
	//#pragma omp parallel for collapse(2)
	for(int i = 0; i < m; i++){
		for(int j = 0; j < m; j++){	
			submat[i][j] = A[index_i + i][index_j + j];
		}
	}
	
	//return submat;
	
}

//Turn 4 smaller submatrixes into a big one

void combineMatrix(int n, int** A11, int** A12, int** A21, int** A22, int** matrix){
	
	
	int m = n/2;
	allocateMatrix(n,matrix);
	
	//-m for index on submatrixes to big matrix
	//#pragma omp parallel for collapse(2)
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if(j < m && i < m)
				matrix[i][j] = A11[i][j];
			else if(i < m)
				matrix[i][j] = A12[i][j-m];
			else if(j < m)
				matrix[i][j] = A21[i-m][j];
			else
				matrix[i][j] = A22[i-m][j-m];	
			
		}
	}

} 


//Adds and substracts matrixes to make the submatrixes
void addMatrix(int n, int** A, int** B, int** matrix){
	
	allocateMatrix(n,matrix);
	//printf("In addMatrix, matrix allocated\n");
	
	//#pragma omp parallel for collapse(2)
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			//printf("In addMatrix, %d,%d calculating\n", i, j);
			matrix[i][j] = A[i][j] + B[i][j];
			//printf("In addMatrix, %d,%d calculated\n", i, j);
		}
	}
}

void subsMatrix(int n, int** A, int** B, int** matrix){
	
	allocateMatrix(n,matrix);
	
	
	//#pragma omp parallel for collapse(2)
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			matrix[i][j] = A[i][j] - B[i][j];
		}
	}
	
}


void freeMat(int n, int** A){
	
	for(int i = 0; i < n; i++)
		free(A[i]);
	//free(A);
}

void regularMult(int n, int** A, int** B, int** C){
	
	//#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
	
}


void strassen(int** A, int** B, int** C, int n, int rank, int count){
	
	//n is size of array, rows or columns
	//doesnt matter bc its square
	
	//printf("Size of mat is %d\n", n);
	

	if(n <= 32){
		
		regularMult(n, A, B, C);
	}
	else{
		
		
		int m = n/2;
		
		
		
		//ASSIGN SUBMATRIXES
		
		int* A11[m];
		subMatrix(n, A, 0, 0, A11);
		
		//printf("A11 calculated\n");
		
		int* A12[m];
		subMatrix(n, A, 0, m, A12);
		
		//printf("A12 calculated\n");
		
		int* A21[m];
		subMatrix(n, A, m, 0, A21);
		
		//printf("A21 calculated\n");
		
		int* A22[m];
		subMatrix(n, A, m, m, A22);
		
		//printf("A22 calculated\n");
		
		int* B11[m];
		subMatrix(n, B, 0, 0, B11);
		
		//printf("B11 calculated\n");
		
		int* B12[m];
		subMatrix(n, B, 0, m, B12);
		
		//printf("B12 calculated\n");
		
		int* B21[m];
		subMatrix(n, B, m, 0, B21);
		
		//printf("B21 calculated\n");
		
		int* B22[m];
		subMatrix(n, B, m, m, B22);
		
		//printf("B22 calculated\n");
		
		//SPAWN THREADS
		//DIVIDE MPI PROCESSES
		
		
		int* P1[m];
		allocateMatrix(m,P1);
		int* P2[m];
		allocateMatrix(m,P2);
		int* P3[m];
		allocateMatrix(m,P3);
		int* P4[m];
		allocateMatrix(m,P4);
		int* P5[m];
		allocateMatrix(m,P5);
		int* P6[m];
		allocateMatrix(m,P6);
		int* P7[m];
		allocateMatrix(m,P7);
		
		
		if(count == 0){
			if(rank == 0){
				//matrix all. rank 0 needs access to all matrixes so recv
				//source = which processor rank
				printf("process %d\n", rank);
				MPI_Recv(&(P1[0][0]), m*m, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&(P2[0][0]), m*m, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&(P3[0][0]), m*m, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&(P4[0][0]), m*m, MPI_INT, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&(P5[0][0]), m*m, MPI_INT, 5, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&(P6[0][0]), m*m, MPI_INT, 6, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&(P7[0][0]), m*m, MPI_INT, 7, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				
			}
			
			if(rank == 1){
				
				//make P1
				printf("process %d\n", rank);
				int* S1[m];
				subsMatrix(m, B12, B22, S1);
				strassen(A11, S1, P1, m, 0, 1);
				freeMat(m, S1);	
				MPI_Send(&(P1[0][0]), m*m, MPI_INT, 0, 0, MPI_COMM_WORLD);
				
			}
			
			if(rank == 2){
				//make p2
				printf("process %d\n", rank);
				int* S2[m];
				addMatrix(m, A11, A12, S2);
				strassen(S2, B22, P2, m, 0, 1);
				freeMat(m, S2);
				MPI_Send(&(P2[0][0]), m*m, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
			
			if(rank == 3){
				//make p3
				printf("process %d\n", rank);
				int* S3[m];
				addMatrix(m, A21, A22, S3);
				strassen(S3, B11, P3, m, 0, 1);
				freeMat(m, S3);
				MPI_Send(&(P3[0][0]), m*m, MPI_INT, 0, 0, MPI_COMM_WORLD);
				
			}
			
			if(rank == 4){
				//make p4
				printf("process %d\n", rank);
				int* S4[m];
				subsMatrix(m, B21, B11, S4);
				strassen(A22, S4, P4, m, 0, 1);			
				freeMat(m, S4);
				MPI_Send(&(P4[0][0]), m*m, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
			
			if(rank == 5){
				//make p5
				printf("process %d\n", rank);
				int* S5[m];
				int* S6[m];
				addMatrix(m, A11, A22, S5);
				addMatrix(m, B11, B22, S6);
				strassen(S5, S6, P5, m, 0, 1);
				freeMat(m, S5);
				freeMat(m, S6);
				MPI_Send(&(P5[0][0]), m*m, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
			
			if(rank == 6){
				//make p6
				printf("process %d\n", rank);
				int* S7[m];
				int* S8[m];
				subsMatrix(m, A12, A22, S7);
				addMatrix(m, B21, B22, S8);		
				strassen(S7, S8, P6, m, 0, 1);
				freeMat(m, S7);
				freeMat(m, S8);
				MPI_Send(&(P6[0][0]), m*m, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
			
			if(rank == 7){
				//make p7
				printf("process %d\n", rank);
				int* S9[m];
				int* S10[m];
				subsMatrix(m, A11, A21, S9);
				addMatrix(m, B11, B12, S10);
				strassen(S9, S10, P7, m, 0, 1);
				freeMat(m, S9);
				freeMat(m, S10);
				MPI_Send(&(P7[0][0]), m*m, MPI_INT, 0, 0, MPI_COMM_WORLD);
				
			}
			
			MPI_Barrier(MPI_COMM_WORLD);
			
			freeMat(m, A11);
			freeMat(m, A12);
			freeMat(m, A21);
			freeMat(m, A22);
			freeMat(m, B11);
			freeMat(m, B12);
			freeMat(m, B21);
			freeMat(m, B22);
			
			
			
			if(rank == 0){
		
			int* C11[m];
		

			//printf("Starting C11\n");
			int* P56[m];
			int* P42[m];
			
			addMatrix(m, P5, P6, P56);
			subsMatrix(m, P4, P2, P42);
			
			//printf("subsMatrix\n");
			addMatrix(m, P56, P42, C11);
			
			freeMat(m, P56);
			freeMat(m, P42);
			//printf("Finished C11\n");

		
		
			int* C12[m];
		

			//printf("Starting C12\n");
			addMatrix(m, P1, P2, C12);
			//printf("Finished C12\n");
			

		
			int* C21[m];
		

			
			//printf("Starting C21\n");
			addMatrix(m, P3, P4, C21);
			//printf("Finished C21\n");
			

		
			int* C22[m];
		

			//printf("Starting C22\n");	
			int* P37[m];
			int* P51[m];
			subsMatrix(m, P5, P7, P37);
			subsMatrix(m, P1, P3, P51);
			
			//printf("subsMatrix\n");		
			addMatrix(m, P51, P37, C22);
				
			freeMat(m, P51);
			freeMat(m, P37);
			//printf("Finished C22\n");

			combineMatrix(n, C11, C12, C21, C22, C);
		
			freeMat(m, C11);
			freeMat(m, C12);
			freeMat(m, C21);
			freeMat(m, C22);
		
		}
		}else{
			
			//p1
			
			int* S1[m];
			subsMatrix(m, B12, B22, S1);
			strassen(A11, S1, P1, m, 0, 1);
			freeMat(m, S1);	
			
			
			//p2
			int* S2[m];
			addMatrix(m, A11, A12, S2);
			strassen(S2, B22, P2, m, 0, 1);
			freeMat(m, S2);
			
			//p3
			int* S3[m];
			addMatrix(m, A21, A22, S3);
			strassen(S3, B11, P3, m, 0, 1);
			freeMat(m, S3);
			
			//p4
			int* S4[m];
			subsMatrix(m, B21, B11, S4);
			strassen(A22, S4, P4, m, 0, 1);			
			freeMat(m, S4);
			
			//p5
			int* S5[m];
			int* S6[m];
			addMatrix(m, A11, A22, S5);
			addMatrix(m, B11, B22, S6);
			strassen(S5, S6, P5, m, 0, 1);
			freeMat(m, S5);
			freeMat(m, S6);
			
			//p6
			int* S7[m];
			int* S8[m];
			subsMatrix(m, A12, A22, S7);
			addMatrix(m, B21, B22, S8);		
			strassen(S7, S8, P6, m, 0, 1);
			freeMat(m, S7);
			freeMat(m, S8);
			
			//p7
			int* S9[m];
			int* S10[m];
			subsMatrix(m, A11, A21, S9);
			addMatrix(m, B11, B12, S10);
			strassen(S9, S10, P7, m, 0, 1);
			freeMat(m, S9);
			freeMat(m, S10);	
			
			
			
			freeMat(m, A11);
			freeMat(m, A12);
			freeMat(m, A21);
			freeMat(m, A22);
			freeMat(m, B11);
			freeMat(m, B12);
			freeMat(m, B21);
			freeMat(m, B22);
			
			
			int* C11[m];
		

			//printf("Starting C11\n");
			int* P56[m];
			int* P42[m];
			
			addMatrix(m, P5, P6, P56);
			subsMatrix(m, P4, P2, P42);
			
			//printf("subsMatrix\n");
			addMatrix(m, P56, P42, C11);
			
			freeMat(m, P56);
			freeMat(m, P42);
			//printf("Finished C11\n");

		
		
			int* C12[m];
		

			//printf("Starting C12\n");
			addMatrix(m, P1, P2, C12);
			//printf("Finished C12\n");
			

		
			int* C21[m];
		

			
			//printf("Starting C21\n");
			addMatrix(m, P3, P4, C21);
			//printf("Finished C21\n");
			

		
			int* C22[m];
		

			//printf("Starting C22\n");	
			int* P37[m];
			int* P51[m];
			subsMatrix(m, P5, P7, P37);
			subsMatrix(m, P1, P3, P51);
			
			//printf("subsMatrix\n");		
			addMatrix(m, P51, P37, C22);
				
			freeMat(m, P51);
			freeMat(m, P37);
			//printf("Finished C22\n");

			combineMatrix(n, C11, C12, C21, C22, C);
		
			freeMat(m, C11);
			freeMat(m, C12);
			freeMat(m, C21);
			freeMat(m, C22);
			
		}
		
		
		 
		 //FREE P MATRIXES
		 
		 //printf("Freeing matrixes\n");
		 
		 freeMat(m, P1);
		 
		 //printf("P1 free\n");
		 
		 freeMat(m, P2);
		 
		 //printf("P2 free\n");
		 
		 freeMat(m, P3);
		 
		 //printf("P3 free\n");
		 
		 freeMat(m, P4);
		 
		 //printf("P4 free\n");
		 
		 freeMat(m, P5);
		 
		 //printf("P5 free\n");
		 
		 freeMat(m, P6);
		 
		 //printf("P6 free\n");
		 
		 freeMat(m, P7);
		 
		 //printf("P7 free\n");
		 
		
		
	}
	
	
}


int main (int argc, char *argv[] )
{
	FILE *file1, *file2, *file3;
	

	//open matrixes
	file1 = fopen(f1, "r");
	file2 = fopen(f2, "r");

	//output file
	file3 = fopen(f3, "a");
	
	
	int p;
    int rank;
	//init
	MPI_Init( & argc, & argv);
	//rank
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//prcesses
    MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	int size;

	//read matrix files 
	
	if(rank == 0)
		size = readSize(file1);
	
	
	//barrier for size reading
	//broadcast size for everyone (everyone needs it)
	//broadcast from process 0
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	int* mat1[size];
	int* mat2[size];
	int* res[size];
	
	//allocate result matrix
	
	allocateMatrix(size,res);
	allocateMatrix(size,mat1);
	allocateMatrix(size,mat2);
	
	

	//reset pointer to start of file + read
	//matrix nums
	
	if(rank == 0) {
		
	fseek(file1, 0, SEEK_SET);
	
	readMatrix(file1, size, mat1);
	
	readMatrix(file2, size, mat2);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	//broadcast matrix for everyone (everyone needs it)
	MPI_Bcast(&(mat1[0][0]), size * size, MPI_INT, 0, MPI_COMM_WORLD);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
    MPI_Bcast(&(mat2[0][0]), size * size, MPI_INT, 0, MPI_COMM_WORLD);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	double wtime = MPI_Wtime();
	strassen(mat1, mat2, res, size, rank, 0);
	wtime = MPI_Wtime() - wtime;
		
	if(rank == 0)
	{
		//print result and write to file
		for(int i = 0; i < size; i++){
			for(int j = 0; j < size; j++){
				//printf("%d ",res[i][j]);
				if(j == size-1)
					fprintf(file3, "%d ", res[i][j]);
				else
					fprintf(file3, "%d, ", res[i][j]);
			}
			//printf("\n");
			fprintf(file3, "\n");
		}
		
		
		
		//printMat(res,size);
		
		fclose(file1);
		fclose(file2);
		fclose(file3);
	}
	
	
	printf("time elapsed is %f\n", wtime);
	printf("result in c.txt\n");
	printf("rank %d\n",rank);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	
	return 0;
	
}

