////////////////////////////////////////////
////////// Manual de Uso: SBR.exe //////////
////////////////////////////////////////////

1. Uso del programa:

	SBR.exe <BC.txt> <Config.txt> <BH.txt>

	- <BC.txt>: Fichero de entrada que contiene la base de conocimiento del SBR.
	Dicho fichero será parseado por el programa, almacenando en una estructura de datos
	el conjunto de reglas que componen el sistema.
	
	- <Config.txt>: Fichero de configuración, el cual se compone del número de atributos del sistema,
	el objetivo, y la prioridad de las reglas de la base de conocimiento.

	- <BH.txt>: base de hechos inicial del SBR
	
	Para facilitar la ejecución del conjunto de casos posibles, se ha creado un script llamado
	"pruebas.bat", el cual ejecutará cada situación con las distintas bases de hechos iniciales 
	que hemos propuesto. Como en el enunciado que se facilita en la asignatura no se especifica,
	hemos decidido añadir por nuestra cuenta para cada situación dos casos en los que no se 
	obtiene una solución con éxito.

2. Salida

	Los ficheros de salida del programa serán almacenados en la carpeta "outputs", cada uno de ellos
	identificado mediante el nombre de la base de hechos inicial que ha dado lugar a tal fichero.

	Cada fichero de salida se compone de varias partes, en primer lugar aparece la base de hechos 
	inicial y su contenido, después de esto el objetivo que se busca en ese caso, después irán apareciendo las reglas que se van aplicando (en orden) y finalmente cuando acabe el algoritmo mostrará el contenido de la base de hechos final y si se encontro la solución con éxito.