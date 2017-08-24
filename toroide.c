//Simulacion de una red toroide
//Cristian Trapero Mora
//Compilar: mpicc -g toroide.c -o toroide -DL=lado
//Ejecutar: mpirun -n lado*lado toroide

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

//Lado de la red toroide
//#define L 3

//Nombre del fichero del cual vamos a leer
#define DATOSDAT "datos.dat"

//Numero maximo de numeros contenidos en el fichero datos.dat
#define MAX_FILE 1024
#define MAX_ITEMS 1024

//Funcion para leer el fichero de datos, usado por el rank 0
int leerFichero(double *numeros);

//Funcion para obtener los vecinos de un rank
void obtenerVecinos(int rank, int *vecinoSuperior, int *vecinoInferior, int *vecinoIzquierdo, int *vecinoDerecho);

//Funcion para obtener el numero menor en toda la red toroide
double obtenerMinimo(int rank, double bufferNumero, int vecinoSuperior, int vecinoInferior, int vecinoIzquierdo, int vecinoDerecho);

int main(int argc, char *argv[]){

	int rank, size;
	MPI_Status status;

	//Buffer que almacena el numero para cada rank
	double bufferNumero;

	//Menor numero de toda la red
	double numeroMinimo;

	//Rank de los vecinos correspondientes al rank
	int vecinoSuperior, vecinoInferior, vecinoIzquierdo, vecinoDerecho;

	//Variable seguir nos dira si seguir con la ejecución(1) o no(0). 
	int seguir=1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//SI SOY EL RANK 0
	if (rank==0){

		//Si no hemos lanzado el numero de procesos adecuado al toroide
		if(L*L!=size){

			printf("Error: Se deben lanzar %d procesos para un toroide de lado %d \n", L*L, L);

			//Envio a los demas procesos que paren la ejecucion
			seguir=0;
			MPI_Bcast(&seguir, 1, MPI_INT, 0, MPI_COMM_WORLD);

		}else{

			//Vector de numeros que repartiremos entre los nodos
			double *numeros=malloc(MAX_ITEMS * sizeof(double));

			//Leemos del fichero
			int cantidadNumeros=leerFichero(numeros);

			//Si la cantidad de numeros del fichero no es correcta
			if(L*L!=cantidadNumeros){
				printf("Error: Cantidad de numeros contenidos en el fichero incorrecto. Se necesitan %d datos, y se tienen %d.\n", L*L, cantidadNumeros);

				//Envio a los demas procesos que paren la ejecucion
				seguir=0;
				MPI_Bcast(&seguir, 1, MPI_INT, 0, MPI_COMM_WORLD);

			}else{

				//Continuamos la ejecucion en los demas procesos
				MPI_Bcast(&seguir, 1, MPI_INT, 0, MPI_COMM_WORLD);

				int j;

				//Envio los numeros del fichero, uno a cada rank
				for (j = 0; j < cantidadNumeros; ++j){
					bufferNumero=numeros[j];
					MPI_Send(&bufferNumero, 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD);
				}

				//Libero el espacio de memoria asignado para los numeros
				free(numeros);
			}


		}

	}
	//Espero un broadcast del root para saber si continuar
	MPI_Bcast(&seguir, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//Si el rank 0 nos permite seguir
	if(seguir!=0){

		//Recibo el dato correspondiente a mi rank
		MPI_Recv(&bufferNumero, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		//Obtengo mis vecinos
		obtenerVecinos(rank, &vecinoSuperior, &vecinoInferior, &vecinoIzquierdo, &vecinoDerecho);	

		//Obtengo el numero menor
		numeroMinimo=obtenerMinimo(rank, bufferNumero, vecinoSuperior, vecinoInferior, vecinoIzquierdo, vecinoDerecho);

		//Solo el rank 0 imprime el menor de la red
		if(rank==0){
			printf("Soy el rank %d. El menor numero de la red es: %.3lf\n", rank, numeroMinimo);
		}

	}

	//Finalizamos la ejecucion
	MPI_Finalize();
	return 0;
}

//Obtenemos los numeros del fichero para guardarlos en un array
int leerFichero(double *numeros){

	//Vector auxiliar de char para trabajar con los numeros del fichero
	char *listaNumeros=malloc(MAX_FILE * sizeof(char));

	//Tamanio del vector de numeros
	int cantidadNumeros=0;

	//Caracter auxiliar para trabajar con los numeros del fichero
	char *numeroActual;

	//Abrimos el fichero con permisos de lectura
	FILE *fichero=fopen(DATOSDAT, "r");
	if(!fichero){
		fprintf(stderr,"ERROR: no se pudo abrir el fichero\n.");
		return 0;
	}

	//Copiamos los datos del fichero al vector auxiliar de char
	fscanf(fichero, "%s", listaNumeros);

	//Cerramos el fichero
	fclose(fichero);

	//Leemos el primer numero hasta la primera coma. Usamos la funcion strtok. Con atof transformamos el string a double
	numeros[cantidadNumeros++]=atof(strtok(listaNumeros,","));

	//Vamos leyendo hasta que no haya mas numeros delante de las comas
	while( (numeroActual = strtok(NULL, "," )) != NULL ){
		//Metemos en el vector el numero correspondiente
		numeros[cantidadNumeros++]=atof(numeroActual);
	}

	free(listaNumeros);
	return cantidadNumeros;
}

//Funcion que nos permitira obtener los rank de los vecinos correspondientes a mi rank
void obtenerVecinos(int rank, int *vecinoSuperior, int *vecinoInferior, int *vecinoIzquierdo, int *vecinoDerecho){
	int fila, columna;

	//Tendremos en cuenta los nodos en las filas y columnas exteriores
	//Las filas se enumeran de abajo a arriba y las columnas de izquiera a derecha
	fila=rank/L;
	columna=rank%L;

	//Si la fila es la inferior
	if(fila==0){
		//Obtengo el vecino inferior, correspondiente al norte de la red
		*vecinoInferior = ((L-1)*L)+columna;
	}else{
		*vecinoInferior = ((fila-1)*L)+columna;
	}

	//Si la fila es la superior
	if(fila==L-1){
		//Obtengo el vecino superior, correspondiente al sur de la red
		*vecinoSuperior = columna;
	}else{
		*vecinoSuperior = ((fila+1)*L)+columna;
	}

	//Si la columna es la izquierda
	if(columna==0){
		//Obtengo el vecino izquierdo, correspondiente al oeste de la red
		*vecinoIzquierdo = (fila*L)+(L-1);
	}else{
		*vecinoIzquierdo = (fila*L)+(columna-1);
	}

	//Si la columna es la derecha
	if(columna==L-1){
		//Obtengo el vecino derecho, correspondiente al este de la red
		*vecinoDerecho = (fila*L);
	}else{
		*vecinoDerecho = (fila*L)+(columna+1);
	}

}

//Funcion que nos permite obtener el menor numero de toda la red
double obtenerMinimo(int rank, double bufferNumero, int vecinoSuperior, int vecinoInferior, int vecinoIzquierdo, int vecinoDerecho){
	
	int i;

	//Menor numero de toda la red
	double minimo=DBL_MAX;

	MPI_Status status;

	//Calculo el numero menor por fila
	//Envio a mi vecino derecho el minimo tantas veces como elementos por fila
	for(i=0; i<L; i++){
		
		//Si mi numero actual es menor que el minimo, lo cambio
		if(bufferNumero<minimo){
			minimo=bufferNumero;
		}

		//Envio mi minimo a mi vecino derecho
		MPI_Send(&minimo, 1, MPI_DOUBLE, vecinoDerecho, i, MPI_COMM_WORLD);

		//Recibo el minimo de mi vecino izquierdo
		MPI_Recv(&bufferNumero, 1, MPI_DOUBLE, vecinoIzquierdo, i, MPI_COMM_WORLD, &status);

		//Si es menor el numero que me ha enviado, lo cambio por el minimo
		if(bufferNumero<minimo){
			minimo=bufferNumero;
		}
	}

	//En cada fila ya tengo el minimo correspondiente a dicha fila

	//Calculo el numero menor por columna, una vez tengo los menores por fila
	//Envio a mi vecino superior el minimo tantas veces como elementos por columna
	for(i=0; i<L; i++){

		//Envio mi minimo a mi vecino superior
		MPI_Send(&minimo, 1, MPI_DOUBLE, vecinoSuperior, i, MPI_COMM_WORLD);

		//Recibo el minimo de mi vecino inferior
		MPI_Recv(&bufferNumero, 1, MPI_DOUBLE, vecinoInferior, i, MPI_COMM_WORLD, &status);

		if(bufferNumero<minimo){
			minimo=bufferNumero;
		}
	}

	//Ya tengo el minimo de toda la red
	return minimo;
}
