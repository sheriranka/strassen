#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char **argv)
{

	//first argument is exponent of 2 to
	//make matrix size
	
	
	//unsigned int randval;
	FILE *f1, *f2;
	time_t t;
	char* p;
	srand((unsigned) time(&t));
	//s = fopen("/dev/random","r");

	/*generating*/

	printf("\nGenerating Random Matrixes.\n");

	

	long i = strtol(argv[1], &p, 10);
	long size = pow(2,i);
	
	f1 = fopen("a.txt", "a");
	f2 = fopen("b.txt", "a");
	
	int x, y;

	for (long k = 0; k<size; k++)
	{	
		for(long l = 0; l<size; l++){
			x = rand()%4;
			y = rand()%4;
			
			if(l < size-1)
			{
				fprintf(f1, "%d,", x);
				fprintf(f2, "%d,", y);
			}
			else
			{
				fprintf(f1, "%d", x);
				fprintf(f2, "%d", y);
			}
				
		}
		if(k < size-1){
			fprintf(f1,"\n");
			fprintf(f2,"\n");
		}
	}

	printf("Saved matrixes to a.txt and b.txt\n");
	fclose(f1);
	fclose(f2);
	//fclose(s);

	return 0;

}

