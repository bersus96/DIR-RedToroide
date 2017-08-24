# Diseño de infraestructura de red: Red toroide

## Enunciado
Dado un archivo con nombre datos.dat, cuyo contenido es una lista de valores separados por comas, nuestro programa realizará lo siguiente:
El proceso de rank 0 destribuirá a cada uno de los nodos de un toroide de lado L, los L x L numeros reales que estarán contenidos en el archivo datos.dat. 

* En caso de que no se hayan lanzado suficientes elementos de proceso para los datos del programa, éste emitirá un error y todos los procesos finalizarán.
* En caso de que todos los procesos han recibido su correspondiente elemento,comenzará el proceso normal del programa.

Se pide calcular el elemento menor de toda la red, el elemento de proceso con rank 0 mostrará en su salida estándar el valor obtenido.
La complejidad del algoritmo no superará O(raiz_cuadrada(n)) Con n número de elementos de la red.

## Instalar OpenMPI
Para que le proyecto funcione correctamente, es necesario tener instaladas las librerias de OpenMPI. Las instrucciones podeis encontrarlas en el siguiente enlace: [http://lsi.ugr.es/jmantas/pdp/ayuda/instalacion.php?ins=openmpi](http://lsi.ugr.es/jmantas/pdp/ayuda/instalacion.php?ins=openmpi) 

## Compilación y ejecución del proyecto
Primeramente debemos de generar los datos del fichero datos.dat. Para ello existe un programa denominado generar_datos.c, que debe ser compilado y ejecutado con la instrucción:
```bash
make generarDatos
```

Para compilar el programa ejecutar la instrucción:
```bash
make compilarToroide
```
Para ejecutar el programa, ejecutar la instrucción:
```bash
make ejecutarToroide
```
## Informe
Todos las explicaciones pertinentes del diseño del programa y como ejecutarlo, están reflejadas en el [informe en PDF](https://github.com/critramo/DIR-RedHipercubo/Informe.pdf).
