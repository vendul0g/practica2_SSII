+------------------------+
|     SBR-FC.exe         |
+------------------------+

MANUAL DE USUARIO - SINOPSIS

Uso: SBR-FC.exe <base_conocimiento> <base_hechos> [fichero_salida]
	[-h]: Muestra esta ayuda

El fichero de salida es opcional. Si no se especifica, se imprimirá por pantalla.
Ejemplo: SBR-FC.exe bc.txt bh.txt



DETALLADO

Tanto la ejecución con el parámetro -h como los errores de argumentos van a producir la salida de la ayuda mostrada arriba.

Es obligatorio para la ejecución del SBR introducir un fichero de entrada como base de conocimiento como primer argumento y un fichero de base de hechos como segundo parámetro.
De manera opcional, se puede indicar un fichero de salia al que volcar la ejecución. En caso de que no se indique ninguno, se mostrará el resultado por pantalla

FORMATO FICHEROS DE ENTRADA

Base de conocimientos.
El fichero de base de conocimientos incluye el conocimiento total que se tiene sobre el mundo en cuestión. Esta información se representa en forma de reglas y tiene la siguiente estructura:

	<num_reglas>
	R<i>: Si <hecho> [y/o <hecho2>...] Entonces <hechoDestino>, FC=<fc>

Donde 
	<num_reglas>	es un entero que representa el número de reglas que tiene la base de conocimiento
	<i> 	es un número entero que indica el número de la regla, comienza por el índice 1
	<hecho> [y/o <hecho2>...] 	son los ID de los hechos concatenados, si hay varios, por operadores Y/O.
	<hechoDestino>	el ID del hecho en el que deriva la regla
	<fc>	Factor de certeza de la regla

Un ejempolo de una base de conocimientos correcta sería la siguiente:

4
R1: Si A o B Entonces C, FC=0.7
R2: Si D y E y F Entonces C, FC=0.5
R3: Si C o H Entonces I, FC=0.65
R4: Si G Entonces H, FC=0.6





Base de hechos.
El fichero de la base de hechos contiene los hechos, con sus factores de certeza correspondientes, que se encuentran en un caso concreto, para obtener un objetivo específico. Tienen la siguiente estructura:
	<num_hechos>
	<id>, FC=<fc>
	Objetivo
	<idMeta>

Donde
	<num_hechos>	es un número entero que representa el número de hechos contenidos en este fichero
	<id>	es el ID del hecho
	<fc>	es el factor de certeza del hecho
	<idMeta>	es el identificador de la meta que se quiere buscar

Un ejemplo de una base de hechos correcta sería la siguiente:

6
A, FC=0.6
B, FC=0.4
D, FC=0.9
E, FC=0.7
F, FC=0.8
G, FC=-0.3
Objetivo
I


FICHEROS DE SALIDA

Los ficheros de salida nos muestran el proceso que ha seguido el motor de inferencias para la resolución del problema, y también para el cálculo de los factores de certeza.

Un estracto de fichero de salida sería el siguiente:

Fichero base de conocimiento: prueba1/bc.txt
Fichero base de hechos: prueba1/bh.txt
Meta: I
----------------------------------------------------------

: CC={R3}
: R={R3}
: Eliminar R3 --> CC={}
: NuevasMetas={C, H}
: Meta=C
: NuevasMetas={H}
: Verificar(C, BH={A, B, D, E, F, G})
: : CC={R1, R2} 
: : R={R1}
: : Eliminar R1 --> CC={R2}
: : NuevasMetas={A, B}
: : Meta=A
: : NuevasMetas={B}
: : Verificar(A, BH={A, B, D, E, F, G}): :  --> TRUE
: : BH={A, B, D, E, F, G}
: : Meta=B
: : NuevasMetas={}
: : Verificar(B, BH={A, B, D, E, F, G}): :  --> TRUE
: : BH={A, B, D, E, F, G}
: : R1 (regla activada)
: :     Caso 1. Calculamos FC de antecedentes de R1 --> FC(Antecedentes R1) = 2. FC(R1) = 0.6
: :     Caso 3. Calculamos FC de R1 y sus antecedentes --> FC(R1) = 0.42
<...>
: verificado = TRUE; CC={}; BH={A, B, D, E, F, G, C, H, I}
---------------------------------------------------------
Solucion	FC(I)=0.4


Como vemos, comienza indicando los ficheros de entrada que hemos introducido, así como la meta que hemos pedido. A continuación, comienza el cálculo del algoritmo. Vemos que se muestra para cada ejecución del algoritmo la sentencia de ": ", que nos indica el grado de profundidad en el que se está trabajando. Además, cada vez aue se consigue la suficiente información para calcular el factor de certeza de una regla, se mostrará el indicador de "Ri (regla activada)", y en las líneas sucesivas se mostrará qué casos se han identificado para el cálculo del factor de certeza.
Por último, vemo que se devuelve si la meta inicial ha sido verificada, mostrada con "TRUE" en caso verdadero, y "FALSE" en caso contrario. Seguidamente vemos la lista del conjunto conflicto cuando acaba el algoritmo junto con la lista de la base de hechos al final de la ejecución; que contiene todos los hechos que han sido verificados durante la ejecución del algoritmo.
