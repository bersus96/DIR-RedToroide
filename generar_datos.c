#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char const *argv[])  {
	int i;
	int num = atoi(argv[1]); /* Cuantos numeros que queremos obtener */
	char filename[] = "datos.dat";
	FILE *archivo;

	if((archivo = fopen(filename, "w+")) == NULL) {
		printf("Error en apertura de archivo");
	}

	double alea(int desde, int hasta) {
		return rand() % (hasta - desde + 1) + desde;
	}

	srand(time(NULL)); /* Semilla */

	for(i=0; i<num; i++) {

		fprintf(archivo, "%lf,", alea(-1000,1000));
	}

	fclose(archivo); 
	return 0;
}
