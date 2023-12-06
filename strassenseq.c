# include <stdlib.h>
# include <stdio.h>
# include <omp.h>

//Pass the name of the txt file with a matrix on it to read
//the matrix 1, and then another txt file with matrix 2


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
	
	//printf("size:%d\n",n);
	//Square matrix allocate
	//fsr it has to be n*2?
	for(int i = 0; i < n; i++)
		matrix[i] = (int*)malloc(n*sizeof(int));

	
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


void strassen(int** A, int** B, int** C, int n){
	
	//n is size of array, rows or columns
	//doesnt matter bc its square
	
	//printf("Size of mat is %d\n", n);
	
	//allocate result matrix
	
	allocateMatrix(n,C);
	
	if(n <= 32){
		
		regularMult(n, A, B, C);
	}
	else{
		
		
		int m = n/2;
		
		//printf("m calculated\n");
		
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
		//CALCULATE P MATRIXES AND FREE A,B AND S
		//SUBMATRIXES
		
		//printf("making P's in size %d\n",m);
		
		
		int* P1[m];
		
		//#pragma omp task shared(P1)
		//{
			int* S1[m];
			subsMatrix(m, B12, B22, S1);
			//printf("starting P1 in size %d\n",n);
			strassen(A11, S1, P1, m);
			freeMat(m, S1);	
			//printf("finished P1 in size %d\n",n);
			
					
		//}
		
		
		int* P2[m];
		
		//#pragma omp task shared(P2)
		//{
			int* S2[m];
			//printf("starting P2\n");
			addMatrix(m, A11, A12, S2);
			
			strassen(S2, B22, P2, m);
			
			freeMat(m, S2);
			//printf("finished P2\n");
		//}
		
		int* P3[m];
		
		//#pragma omp task shared(P3)
		//{
			//printf("starting P3\n");
			int* S3[m];
			addMatrix(m, A21, A22, S3);
			
			strassen(S3, B11, P3, m);
			
			freeMat(m, S3);
			//printf("finished P3\n");
		//}
		
		
		int* P4[m];
		
		//#pragma omp task shared(P4)
		//{
			int* S4[m];
			subsMatrix(m, B21, B11, S4);
			
			//printf("starting P4\n");
			strassen(A22, S4, P4, m);
			
			freeMat(m, S4);
			//printf("finished P4\n");
		//}
		
		
		int* P5[m];
		
		//#pragma omp task shared(P5)
		//{
			
			int* S5[m];
			int* S6[m];
			addMatrix(m, A11, A22, S5);
			addMatrix(m, B11, B22, S6);
			//printf("starting P5\n");
			strassen(S5, S6, P5, m);
			freeMat(m, S5);
			freeMat(m, S6);
			//printf("finished P5\n");
		//}
		
		
		int* P6[m];
		
		//#pragma omp task shared(P6)
		//{
			//printf("starting P6\n");
			int* S7[m];
			int* S8[m];
			subsMatrix(m, A12, A22, S7);
			addMatrix(m, B21, B22, S8);
		
			strassen(S7, S8, P6, m);
			freeMat(m, S7);
			freeMat(m, S8);
			//printf("finished P6\n");
		//}
		
		
		int* P7[m];
		
		//#pragma omp task shared(P7)
		//{
			int* S9[m];
			int* S10[m];
			
			subsMatrix(m, A11, A21, S9);
			addMatrix(m, B11, B12, S10);
			//printf("starting P7\n");
			strassen(S9, S10, P7, m);
			freeMat(m, S9);
			freeMat(m, S10);
			//printf("finished P7\n");
		//}
		
		
		
		//SYNC
		
		//#pragma omp taskwait
		
		
		//printf("freeing matrixes of matrix size %d\n",n);
		
		//free matrix
		
		freeMat(m, A11);
		freeMat(m, A12);
		freeMat(m, A21);
		freeMat(m, A22);
		freeMat(m, B11);
		freeMat(m, B12);
		freeMat(m, B21);
		freeMat(m, B22);
		
		
		
		//GENERATE C SUBMATRIXES AND FREE
		//SUBMATRIXES GENERATED
		
		
		int* C11[m];
		
		//#pragma omp task shared(C11)
		//{
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
		//}
		
		
		int* C12[m];
		
		//#pragma omp task shared(C12)
		//{
			
			//printf("Starting C12\n");
			addMatrix(m, P1, P2, C12);
			//printf("Finished C12\n");
			
		//}
		
		int* C21[m];
		
		//#pragma omp task shared(C21)
		//{
			
			//printf("Starting C21\n");
			addMatrix(m, P3, P4, C21);
			//printf("Finished C21\n");
			
		//}
		
		int* C22[m];
		
		//#pragma omp task shared(C22)
		//{
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
		//}
		
		//#pragma omp taskwait
		 
		 
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
		 
		 
		 //COMBINE C MATRIXES
		 
		 //printf("Combining matrixes to size %d\n",n);
		 combineMatrix(n, C11, C12, C21, C22, C);
		 
		 
		 //FREE C SUBMATRIXES
		 
		 //printf("Freeing matrixes\n");
		 freeMat(m, C11);
		 freeMat(m, C12);
		 freeMat(m, C21);
		 freeMat(m, C22);
		 
		 
		 //RETURN RESULT
		 //printf("Strassen done\n");
		
		
	}
	
	
}


int main (int argc, char *argv[] )
{
	FILE *file1, *file2, *file3;
	

	//open matrixes
	file1 = fopen(argv[1], "r");
	file2 = fopen(argv[2], "r");
	
	
	if (file1 == NULL || file2 == NULL){
		printf("Please enter 2 separate csv files.\n");
		return 0;
	}
	
	//output file
	if(argc > 3)
		file3 = fopen(argv[3], "a");
	else
	file3 = fopen("c.txt", "a");
	
	
	int size = readSize(file1);

	//reset pointer to start of file
	fseek(file1, 0, SEEK_SET);
	
	
	int* mat1[size];
	readMatrix(file1, size, mat1);
	int* mat2[size];
	readMatrix(file2, size, mat2);

	
	int* res[size];
	
	double wtime = omp_get_wtime();
	strassen(mat1, mat2, res, size);
	wtime = omp_get_wtime() - wtime;
	
	
	//freeMat(size, mat1);
	
	
	
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
	
	printf("time elapsed is %f\n", wtime);
	printf("result in c.txt\n");
	
	
	
	
	
	
	//printMat(mat1, size);
	
	fclose(file1);
	fclose(file2);
	fclose(file3);


	
	
	
	
	return 0;
}

