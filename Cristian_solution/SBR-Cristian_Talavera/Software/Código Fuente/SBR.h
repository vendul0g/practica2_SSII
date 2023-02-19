//////////////////////////////////////////////////////////
///////////////// Funciones auxiliares ///////////////////
//////////////////////////////////////////////////////////

#ifndef SBR_H
#define SBR_H

#include <iostream>
#include <fstream> 
#include <string>
#include <vector>

#include <string.h>
#include <stdlib.h>

using namespace std;

typedef unsigned int uint;

// Par que se corresponde con un hecho/antecedente/consecuente
typedef struct Par {
    string key;
    string op;
    string strValue;
    int intValue;    
} Par;

// Estructura de datos en la que almacenamos los valores de cada regla
typedef struct Reglas {
    vector<Par> listaAntecedentes;
    Par consecuente;
    int prioridad;
    bool marcada;
    string str;
    int id;
} Regla;

///////// Ficheros /////////
vector<Regla> leeBaseConocimiento(char *bc);
string leeConfiguracion(char *config, vector<Regla> &reglas);
vector<Par> leeBaseHechos(char *bh);

///////// Motor de Inferencias /////////
bool encadenamientoHaciaDelante(vector<Par> hechosIniciales, vector<Regla> &bc, string objetivo, int numReglas);
bool contenido(string objetivo, vector<Par> bh);
Regla extraerCualquierRegla(vector<Regla> bc, int numReglas);
vector<Regla> equiparar(vector<Regla> bc, vector<Par> bh, int num_reglas);
Regla resolver(vector<Regla> conjuntoConflicto);
Par aplicar(vector<Regla> &bc, Regla regla, vector<Par> bh);
void actualizar(vector<Par> &bh, Par nuevoHecho);

///////// Auxiliares /////////
void imprimeReglas(vector<Regla> reglas);
void checkOperator(int &index, int &offset, string &op, string search);

#endif /* SBR_H */
