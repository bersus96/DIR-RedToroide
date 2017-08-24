LADO := 3
 
all: desplegarToroide

limpiarDirectorio:
	-rm -f toroide
	-rm -f datos.dat
	-rm -f generarDatos
	
generarDatos:
	gcc generar_datos.c -o generarDatos
	@read -p "Introduce el numero de datos a generar en el fichero datos.dat: " numeroDatos; \
	./generarDatos $$numeroDatos

compilarToroide: 
	@read -p "Introduce el lado del toroide: " lado; \
	mpicc -g toroide.c -o toroide -DL=$$lado

ejecutarToroide: 
	@read -p "Numero de procesos para lanzar el toroide: " procesos; \
	mpirun -n $$procesos toroide

desplegarToroide: limpiarDirectorio generarDatos compilarToroide ejecutarToroide

