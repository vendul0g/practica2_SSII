#include "practica2.h"  // Definición de objetos del algoritmo : regla, hecho, etc.
#include <iostream>     // Imprimir por pantalla
#include <vector>       // Vectores de almacenamiento
#include <iomanip>      // setprecision() : Indicar precisión de números PuntoFlotante
#include <fstream>      // Ficheros de entrada
#include <cstring>      // strcmp() : Comparación de cadenas (string)
#include <unistd.h>     // dup2() : Redirigir la salida estándar
#include <fcntl.h>      // open() : Abrir el fichero de salida

#define RECURSION ": " // Indicador de la profundidad del algoritmo

using namespace std;

//Variables globales
vector<regla> bc; //base de conocimientos

/******************************************************************************
 ****************** FUNCIONES PARA IMPRIMIR POR PANTALLA **********************
 ******************************************************************************/

void print_titulo()
{
    cout <<endl<< "+-------------------------+" << endl;
    cout << "|   Practica 2 - SBR-FC   |" << endl;
    cout << "+-------------------------+" << endl<<endl;

}

/**
 * @brief Imprime la ayuda del comando cuando no se cumplen los requisitos de ejecución o con el parámetro -h
*/
void print_help()
{
    print_titulo();
    cout << "Uso: SBR-FC.exe <base_conocimiento> <base_hechos> [fichero_salida]" <<endl;
    cout << "\t[-h] : Muestra esta ayuda" << endl<<endl;
    cout << "El fichero de salida es opcional. Si no se especifica, se imprimirá por pantalla." << endl;
    cout << "Ejemplo: SBR-FC.exe bc.txt bh.txt" << endl;
}

/**
 * @brief Convierte un vector de hechos en una cadena para imprimir por pantalla
 * @param list Vector de hechos que se quieren imprimir
 * @return La cadena (string) formateada 
*/
string lista_hechos_string(vector<hecho> list)
{
    string s = "{";
    for(size_t i = 0; i < list.size(); i++)
    {
        s += list[i].id;
        // s += "[" + list[i].id + ", " + to_string(list[i].fc) + "]";
        if(i < list.size() - 1)
            s += ", ";
    }
    s+= "}";
    return s;
}

/**
 * @brief Convierte una regla en una cadena para imprimir por pantalla
 * @param bc Base de conocimiento, es decir, lista de reglas
 * @return La cadena (string) formateada
*/
string regla_string(regla bc)
{
    string s;
    s += "id="+to_string(bc.id);
    s += "; op=";
    bc.op == AND ? s+="AND" : s+="OR";
    s+="; alpha=";
    s += lista_hechos_string(bc.alpha);
    s += "; beta=" + bc.beta.id + "; fc=" + to_string(bc.fc) +"\n";
    return s;
}

/**
 * @brief Convierte una lista de reglas en una cadena para imprimir por pantalla
 * @param list Lista de reglas para imprimir
 * @param mode Modo en el que se quiere imprimir: 0 = sencillo; 1 = detallado
 * @return Cadena (string) formateada
*/
string lista_reglas_string(vector<regla> list, int mode)
{
    string s = "{";
    for(size_t i = 0; i < list.size(); i++)
    {
        if(mode == 0)
            s += "R" + to_string(list[i].id);
        else s += regla_string(list[i]);
        if(i < list.size() - 1)
            s += ", ";
    }
    s+= "}";
    return s;
}

/******************************************************************************
 ***************** FUNCIONES PARA INICIALIZAR EL PROGRAMA *********************
 ******************************************************************************/

/**
 * @brief Crea un vector de hechos a partir de una cadena de texto
 * @param s Cadena de texto que contiene los hechos
 * @param op Operador de la regla
 * @return Vector de hechos
 * @note El operador se actualiza en la función
*/
vector<hecho> create_vector_by_string(string s, operador &op)
{
    vector<hecho> alpha;
    unsigned int inicio = 0, fin;
    do{
        hecho h;
        fin = s.find(" ");
        if(fin > s.length()){
            fin = s.length();
        }
        h.id = s.substr(inicio, fin - inicio);
        //Actualizao el operador
        if(h.id == "y" || h.id == "Y"){
            op = AND;
        }else if(h.id == "o" || h.id == "O"){
            op = OR;
        }else{
            alpha.push_back(h);
        }

        if(fin+1 < s.length()){
            s = s.substr(fin+1);
            fin = 0;
        }
    }while(fin < s.length());
    return alpha;
}

/**
 * @brief Inicializa la base de conocimientos a partir de un fichero
 * @param bc_file Fichero de entrada con la base de conocimientos
 * @note La base de conocimientos se almacena en la variable global bc
*/
void inicializa_bc(ifstream &bc_file)
{
    int bc_size, i = 0, aux;
    string line, alpha_string;

    getline(bc_file, line);
    bc_size = stoi(line);
    bc.resize(bc_size);    

        getline(bc_file, line);
    do{
        //id
        bc[i].id = i+1;
        aux = line.find("Si ")+3;
        alpha_string = line.substr(aux, line.find(" Entonces")-aux);
        //vector alpha y operador
        bc[i].alpha = create_vector_by_string(alpha_string, bc[i].op);
        //beta
        aux = line.find("Entonces ")+9;
        bc[i].beta.id = line.substr(aux, line.find(",") - aux);
        //fc
        aux = line.find("FC=")+3;
        bc[i].fc = stof(line.substr(aux, line.length()-1));

        i++;
        getline(bc_file, line);
    }while (!bc_file.eof());
}

/**
 * @brief Inicializa la base de hechos a partir de un fichero
 * @param bh_file Fichero de entrada con la base de hechos
 * @param bh Vector de hechos donde se almacenan los hechos
 * @return El hecho meta identificado en el fichero
*/
hecho inicializa_bh(ifstream &bh_file, vector<hecho> &bh)
{
    hecho meta;
    int bh_size, i = 0, fin;
    string line;

    getline(bh_file, line);
    bh_size = stoi(line);
    bh.resize(bh_size);

    getline(bh_file, line);
    do{
        bh[i].id = line.substr(0, line.find(","));
        bh[i].fc = stof(line.substr(line.find("=") + 1, line.length()-1));
        getline(bh_file, line);
        i++;
    } while (line.substr(0,7) != "Objetiv" && !bh_file.eof());

    
    getline(bh_file, line);
    if(line[line.length()-1] == 13){
        fin = line.length()-1;
    }else{
        fin = line.length();
    }
    meta.id = line.substr(0, fin);
    return meta;
}

/******************************************************************************
 ************************* FUNCIONES DEL ALGORITMO ****************************
 ******************************************************************************/

/**
 * @brief Comprueba si un hecho está contenido en la base de hechos
 * @param meta Hecho a comprobar
 * @param bh Base de hechos
 * @return La solución encontrada
*/
solucion contenida(hecho meta, vector<hecho> bh)
{
    solucion s;
    for(size_t i = 0; i < bh.size(); i++)
    {
        if(bh[i].id == meta.id)
        {
            s.verificado = true;
            s.fc = bh[i].fc;
            return s;
        }
    }
    s.verificado = false;
    return s;
}

/**
 * @brief Comprueba si un hecho está contenido en la base de conocimiento
 * @param meta Hecho a comprobar
 * @return El vector de reglas que contienen el hecho meta como consecuente
 * @note La base de conocimientos accedida está en la variable global bc
*/
vector<regla> equiparar(hecho meta)
{
    vector<regla> cc;
    for(size_t i = 0; i < bc.size(); i++)
    {
        if(bc[i].beta.id == meta.id)
        {
            cc.push_back(bc[i]);
        }

    }
    return cc;
}

/**
 * @brief Extrae los antecedentes de una regla
 * @param r Regla de la que extraer los antecedentes
 * @return Vector de hechos con los antecedentes de la regla
*/
vector<hecho> extraer_antecedentes(regla r)
{
    vector<hecho> antecedentes;
    for(size_t i = 0; i < r.alpha.size(); i++)
    {
        antecedentes.push_back(r.alpha[i]);
    }
    return antecedentes;
}

/**
 * @brief Caso 1: combinación de antecedentes: es necesario combinar las piezas
 * de evidencia, e1 y e2, que afectan al factor de certaza de h
 * > R: Si e1 y/o e2 entonces h
 * -- Se usa cuando se tienen dos o más antecedentes para calcular el
 * factor de certeza de la consecuencia de todos ellos para luego combinarlo
 * con el factor de certeza de la regla
 * @param r Regla a la que se le calcula el factor de certeza de sus antecedentes
 * @param r_fc Vector de factores de certeza de los antecedentes de la regla
 * @return Factor de certeza de los antecedentes de la regla
*/
float calcular_combinacion_antecedentes(regla r, float r_fc[])
{   
    cout << "\tCaso 1. Calculamos FC de antecedentes de R" << r.id <<" --> FC(Antecedentes R"<<r.id<<") = ";
    float sol;
    //Caso de AND
    if(r.op == AND)
    {
        sol = r_fc[0];
        for(unsigned int i = 1; i<r.alpha.size(); i++)
        {
            if(r_fc[i] < sol)
                sol = r_fc[i];
        }
        cout << sol << endl;
        return sol;
    }
    //Caso de OR
    else
    {
        cout << "2. FC(R"<<r.id<<") = ";
        sol = r_fc[0];
        for(unsigned int i = 0; i<r.alpha.size(); i++)
        {
            if(r_fc[i] > sol)
                sol = r_fc[i];
        }
        cout << sol << endl;
        return sol;
    }
}

/**
 * @brief Caso 2. Adquisición incremental de evidencia: Se combinan dos piezas de
 * evidencia, e1 y e2, que afectan al factor de una misma hipotesis
 * > Si e1 entonces h
 * > Si e2 entonces h
 * -- Se usa cuando se tienen dos o más reglas que afectan al mismo hecho
 * @param meta Hecho meta de la que se quiere calcular el FC
 * @param m_fc Vector de factores de certeza de las reglas anteriores
 * @param n Número de reglas anteriores
 * @return Factor de certeza de la meta
 * @note se actualiza el fc de la meta
*/
float calcular_combinacion_reglas(hecho &meta, float m_fc[], int n)
{   
    cout << "Caso 2. Calculamos FC de antecedentes de "<<meta.id<<" --> FC("<<meta.id<<") = ";
    float sol, aux = m_fc[0];

    for(int i = 1; i < n; i++)
    {
        //Caso de ambos positivos
        if(aux >= 0 && m_fc[i] >= 0)
        {
            aux =  m_fc[i] + aux * (1 - m_fc[i]);
        }

        //Caso de ambos negativos
        else if(aux <= 0 && m_fc[i] <= 0)
        {
            aux = m_fc[i] + aux * (1 + m_fc[i]);
        }
        
        //Caso de uno positivo y otro negativo
        else
        {
            aux = (aux + m_fc[i]) / (1 - min(abs(aux), abs(m_fc[i])));
        }
    }
    sol = aux;
    cout << sol << endl;
    meta.fc = sol;
    return sol;    
}

/**
 * @brief Caso 3. Encadenamiento de evidencia: se combinan dos reglas de manera
 * que el resultado de una regla es la entrada de la otra
 * > R1: si e entonces s
 * > R2: si s entonces h
 * -- Se usa cuando una regla tiene un antecedente
 * @param bc Regla que se quiere calcular el FC
 * @param ant_fc Factor de certeza de los hechos anteriores
 * @return Factor de certeza de la regla
*/
float calcular_combinacion_evidencia(regla bc, float ant_fc)
{    
    cout << "\tCaso 3. Calculamos FC de R"<<bc.id<<" y sus antecedentes --> FC(R"<<bc.id<<") = ";    
    float sol, aux;

    if(ant_fc < 0){
        aux = 0;
    }
    else{
        aux = ant_fc;
    }
    sol = bc.fc * aux;
    cout << sol << endl;
    return sol;
}

/**
 * @brief Verifica si la meta está en la base de hechos
 * @param meta Meta a verificar
 * @param bh Base de Hechos
 * @param profundidad Profundidad de la recursividad para mostrar por pantalla
 * @return true si la meta está en la base de hechos y las reglas para llegar a ella
 * @note bc Base de conocimiento global
*/
solucion verificar (hecho meta, vector<hecho> &bh, string profundidad)
{
    int i, k;
    solucion sol;
    sol.verificado = false;
    sol.fc = 0;
    
    // Comprobamos si la meta está en la base de hechos
    solucion aux;
    if( (aux = contenida(meta, bh)).verificado) //Contenida(Meta, BH)
    {
        sol.verificado = true;
        sol.fc = aux.fc;
        return sol;
    }

    cout << endl;

    //CC = equiparar(consecuentes(bc), meta) (bc (global))
    vector<regla> cc = equiparar(meta);
    int cc_size = cc.size();
    float m_fc[cc_size];//lista_fc_meta = vacio()

    //Inicializamos el índice del bucle
    i = 0;
    while(cc.size() > 0 /*&& !sol.verificado*/)
    {
        cout << profundidad << "CC=" << lista_reglas_string(cc, 0) << endl;

        //R = Resolver(CC)
        regla r = cc[0];
        cout << profundidad << "R={R" << r.id << "}" << endl;

        //Creamos el contenedor donde se calcula el fc global de R
        float r_fc[r.alpha.size()]; //lista_fc_R = vacio()

        //Eliminar(R,CC)
        cc.erase(cc.begin());
        cout<<profundidad<<"Eliminar R"<<r.id<<" --> CC=" << lista_reglas_string(cc, 0) << endl;

        //NuevasMetas = extraer_antecedentes(R)
        vector<hecho> nuevas_metas = extraer_antecedentes(r);
        cout << profundidad << "NuevasMetas=" << lista_hechos_string(nuevas_metas) << endl;

        //Inicializamos el índice del bucle
        k = 0;
        while(nuevas_metas.size() > 0 /*&& sol.verificado*/) //Mientras NoVacio(NuevasMetas)
        {
            //Nmet = SeleccionarMeta(NuevasMetas);
            hecho n_met = nuevas_metas[0];
            cout << profundidad << "Meta=" << n_met.id << endl;

            //Eliminar(Nmet,NuevasMetas)
            nuevas_metas.erase(nuevas_metas.begin());
            cout << profundidad << "NuevasMetas=" << lista_hechos_string(nuevas_metas) << endl;

            cout << profundidad << "Verificar(" << n_met.id << ", BH="<< lista_hechos_string(bh) << ")";
            solucion sol_nmet = verificar(n_met, bh, profundidad+ RECURSION); //(verificado, fc_Nmet) = verificar(Nmet, BH)
            
            if(sol_nmet.verificado)
            {// Si v_nmet == verdadero entonces verificado = v_nmet
                sol.verificado = sol_nmet.verificado;
            }

            r_fc[k]=sol_nmet.fc; // Añadir(fc_Nmet, lista_fc_R)
            
            cout << profundidad << " --> ";
            sol.verificado == true ? cout << "TRUE" : cout << "FALSE";
            cout << endl;

            cout << profundidad << "BH=" << lista_hechos_string(bh) << endl;

            //Actualizamos el índice del bucle
            k++;
        }

        //Calcular FC de la regla r
        float ant_fc;
        cout << profundidad << "R"<<r.id<<" (regla activada)"<<endl;
        if(r.alpha.size() > 1){
            cout << profundidad;
            // fc_antecedentes_R = calcular_combinacion_antecedentes(lista_fc_R) //Caso 1
            ant_fc = calcular_combinacion_antecedentes(r, r_fc);
        }
        else{
            // fc_antecedentes_R = calcular_combinacion_antecedentes(lista_fc_R) //Caso 1
            ant_fc = r_fc[0];
        }
        cout << profundidad;
        // fc_R = calcular_combinacion_evidencia(R, fc_antecedentes_R) //Caso 3
        m_fc[i] = calcular_combinacion_evidencia(r, ant_fc); //Añadir(fc_R, lista_fc_meta)
        meta.fc = m_fc[i]; // Meta.fc = fc_R

        if(sol.verificado && cc.size() == 0)//Si verificado y vacio(CC)
        {
            //Añadir(Meta, BH)
            bh.push_back(meta);

            //Calculamos el FC de la meta
            cout << profundidad;
            // fc = calcular_combinacion_reglas(lista_fc_meta) //Caso 2
            sol.fc = calcular_combinacion_reglas(meta, m_fc, cc_size); //Meta.fc = fc
        }

        //Actualizamos el índice del bucle
        i++;
    }
    if(sol.verificado) // Si verificado 
    {
        cout<<profundidad<<"verificado = ";
        sol.verificado == true ? cout << "TRUE" : cout << "FALSE";
        cout<<"; CC="<<lista_reglas_string(cc, 0)<< "; BH=" << lista_hechos_string(bh) << endl;
        //fc = Meta.fc
        sol.fc = meta.fc;
    }
    return sol;
}

/**
 * @brief Función principal del encadenamiento hacia atrás
 * @param bh_file Fichero que contiene la base de hechos
 * @note bc Base de conocimiento global
*/
void encadenamiento_hacia_atras(ifstream &bh_file)
{
    // Recuperamos la meta del fichero de hechos
    // Inicializamos la base de hechos: bh = HechosIniciales
    vector<hecho> bh;
    hecho meta = inicializa_bh(bh_file, bh); 
    bh_file.close();

    cout << "Meta: " << meta.id << endl;
    cout << "----------------------------------------------------------" << endl;


    solucion sol = verificar(meta, bh,  RECURSION); //Verificar(Meta, BH)

    cout << "---------------------------------------------------------" <<endl;
    if(sol.verificado)
    {
        cout << "Solucion\tFC("<<meta.id<<")=" << setprecision(2) << sol.fc << endl;
    }
    else
    {
        cout << "No se ha encontrado solución" << endl;
    }
}

/*****************************************************************************
 **************************** FUNCION PRINCIPAL ******************************
 *****************************************************************************/

int main(int argc, char **argv){
    if(argc < 3  || argc > 4 || strcmp(argv[1], "-h") == 0)
    {
        print_help();
        exit(EXIT_FAILURE);
    }

    //Abrimos el fichero de base de conocimiento
    ifstream bc_file;
    bc_file.open(argv[1], ios::in);
    if(!bc_file.is_open())
    {
        cerr << "Error al abrir el fichero de base de conocimiento" << endl;
        exit(EXIT_FAILURE);
    }
    //Base de conocimiento (BC)= Variable Global
    inicializa_bc(bc_file);
    bc_file.close();
    
    //Abrimos el fichero de base de hechos
    ifstream bh_file;
    bh_file.open(argv[2], ios::in);
    if(!bh_file.is_open())
    {
        cerr << "Error al abrir el fichero de base de hechos" << endl;
        exit(EXIT_FAILURE);
    }

    //Abrimos el fichero de salida
    if(argc == 4)
    {
        int output_file;
        if( (output_file = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
        {
            cerr << "Error al abrir el fichero de salida" << endl;
            exit(EXIT_FAILURE);
        }
        //Redirigimos la salida estándar al fichero de salida
        dup2(output_file, STDOUT_FILENO);        
    }

    print_titulo();
    cout << "Fichero base de conocimiento: " << argv[1] << endl;
    cout << "Fichero base de hechos: " << argv[2] << endl;


    //Encadenamiento hacia atrás - Algoritmo
    encadenamiento_hacia_atras(bh_file);

    
    exit(EXIT_SUCCESS);
}