#include "practica2.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cstring>

using namespace std;

//Variables globales
vector<regla> bc; //base de conocimientos

void print_help()
{
    cout << "+----------------------+" << endl;
    cout << "|  Practica 2 - SBR    |" << endl;
    cout << "+----------------------+" << endl;
    cout << "Uso: practica2 <base_conocimiento> <base_hechos> [fichero_salida]" << endl;
    cout << "Ejemplo: practica2 bc.txt bh.txt" << endl;
}

string lista_hechos_string(vector<hecho> list)
{
    string s = "{";
    for(size_t i = 0; i < list.size(); i++)
    {
        // s += "[" + list[i].id + ", " + to_string(list[i].fc) + "]";
        s += list[i].id;
        if(i < list.size() - 1)
            s += ", ";
    }
    s+= "}";
    return s;
}

string regla_string(regla bc){
    string s;
    s += "id="+to_string(bc.id);
    s += "; op=";
    bc.op == AND ? s+="AND" : s+="OR";
    s+="; alpha=";
    s += lista_hechos_string(bc.alpha);
    s += "; beta=" + bc.beta.id + "; fc=" + to_string(bc.fc) +"\n";
    return s;
}

string lista_reglas_string(vector<regla> list)
{
    string s = "{";
    for(size_t i = 0; i < list.size(); i++)
    {
        s += "R" + to_string(list[i].id);
        // s += regla_string(list[i]);
        if(i < list.size() - 1)
            s += ", ";
    }
    s+= "}";
    return s;
}

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

hecho inicializa_bh(ifstream &bh_file, vector<hecho> &bh)
{
    hecho meta;
    int bh_size, i = 0;
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
    } while (line != "Objetivo" && !bh_file.eof());

    
    getline(bh_file, line);
    meta.id = line.substr(0, line.length());
    return meta;
}

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

vector<hecho> extraer_antecedentes(regla bc)
{
    vector<hecho> antecedentes;
    for(size_t i = 0; i < bc.alpha.size(); i++)
    {
        antecedentes.push_back(bc.alpha[i]);
    }
    return antecedentes;
}

float calcular_combinacion_antecedentes(regla r, float r_fc[])
{   /**
     * Caso 1: combinación de antecedentes: es necesario combinar las piezas
     * de evidencia, e1 y e2, que afectan al factor de certaza de h
     * > R: Si e1 y/o e2 entonces h
     * -- Se usa cuando se tienen dos o más antecedentes para calcular el
     * factor de certeza de la consecuencia de todos ellos para luego combinarlo
     * con el factor de certeza de la regla
    */
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

float calcular_combinacion_reglas(hecho &meta, float m_fc[])
{   /**
    * Caso 2. Adquisición incremental de evidencia: Se combinan dos piezas de
    * evidencia, e1 y e2, que afectan al factor de una misma hipotesis
    * > Si e1 entonces h
    * > Si e2 entonces h
    * -- Se usa cuando se tienen dos o más reglas que afectan al mismo hecho
    */
    cout << "Caso 2. Calculamos FC de antecedentes de "<<meta.id<<" --> FC("<<meta.id<<") = ";
    float sol;
    //Caso de ambos positivos
    if(m_fc[0] > 0 && m_fc[1] > 0)
    {
        sol = m_fc[0] + m_fc[1] * (1 - m_fc[0]);
        cout << sol << endl;
        meta.fc = sol;
        return sol;
    }
    //Caso de ambos negativos
    else if(m_fc[0] < 0 && m_fc[1] < 0)
    {
        sol = m_fc[0] + m_fc[1] * (1 + m_fc[0]);
        cout << sol << endl;
        meta.fc = sol;
        return sol;
    }
    //Caso de uno positivo y otro negativo
    else
    {
        sol = (m_fc[0] + m_fc[1]) / (1 - min(abs(m_fc[0]), abs(m_fc[1])));
        cout << sol << endl;
        meta.fc = sol;
        return sol;
    }

}

float calcular_combinacion_evidencia(regla bc, float ant_fc)
{    /**
     * Caso 3. Encadenamiento de evidencia: se combinan dos reglas de manera
     * que el resultado de una regla es la entrada de la otra
     * > R1: si e entonces s 
     * > R2: si s entonces h
     * -- Se usa cuando una regla tiene un antecedente
    */
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
 * @param bc Base de conocimiento
 * @return true si la meta está en la base de hechos y las reglas para llegar a ella
*/
solucion verificar (hecho meta, vector<hecho> &bh, string profundidad)
{
    solucion sol;
    sol.verificado = false;
    int i, k;
    
    // Comprobamos si la meta está en la base de hechos
    solucion aux;
    if( (aux = contenida(meta, bh)).verificado)
    {
        sol.verificado = true;
        sol.fc = aux.fc;
        return sol;
    }

    cout << endl;

    //equiparar(consecuentes(bc), meta) bc (global)
    vector<regla> cc = equiparar(meta);
    float m_fc[cc.size()];

    //Inicializamos el índice del bucle
    i = 0;
    while(cc.size() > 0 /*&& !sol.verificado*/)
    {
        cout << profundidad << "CC=" << lista_reglas_string(cc) << endl;

        //R = Resolver(CC)
        regla r = cc[0];
        cout << profundidad << "R={R" << r.id << "}" << endl;

        //Creamos el contenedor donde se calcula el fc global de R
        float r_fc[r.alpha.size()];

        //Eliminar(R,CC)
        cc.erase(cc.begin());
        cout<<profundidad<<"Eliminar R"<<r.id<<" --> CC=" << lista_reglas_string(cc) << endl;

        vector<hecho> nuevas_metas = extraer_antecedentes(r);
        cout << profundidad << "NuevasMetas=" << lista_hechos_string(nuevas_metas) << endl;

        sol.verificado = true;
        
        //Inicializamos el índice del bucle
        k = 0;
        while(nuevas_metas.size() > 0 /*&& sol.verificado*/)
        {
            //Nmet = SeleccionarMeta(NuevasMetas);
            hecho n_met = nuevas_metas[0];
            cout << profundidad << "Meta=" << n_met.id << endl;

            //Eliminar(Nmet,NuevasMetas)
            nuevas_metas.erase(nuevas_metas.begin());
            cout << profundidad << "NuevasMetas=" << lista_hechos_string(nuevas_metas) << endl;

            cout << profundidad << "Verificar(" << n_met.id << ", BH="<< lista_hechos_string(bh) << ")";
            solucion aux = verificar(n_met, bh, profundidad+": ");
            sol.verificado = aux.verificado;
            r_fc[k]=aux.fc;
            
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
            ant_fc = calcular_combinacion_antecedentes(r, r_fc);
        }
        else{
            ant_fc = r_fc[0];
        }
        cout << profundidad;
        m_fc[i] = calcular_combinacion_evidencia(r, ant_fc);
        meta.fc = m_fc[i];

        if(sol.verificado && cc.size() == 0 && i > 0)//Si i > 0 significa que el CC tuvo más de una regla
        {
            //Añadir(Meta, BH)
            bh.push_back(meta);

            //Calculamos el FC de la meta
            cout << profundidad;
            sol.fc = calcular_combinacion_reglas(meta, m_fc);
        }

        //Actualizamos el índice del bucle
        i++;
    }
    if(sol.verificado)
    {
        cout<<profundidad<<"verificado = ";
        sol.verificado == true ? cout << "TRUE" : cout << "FALSE";
        cout<<"; CC="<<lista_reglas_string(cc)<< "; BH=" << lista_hechos_string(bh) << endl;
        
        sol.fc = meta.fc;
    }
    return sol;
}

void encadenamiento_hacia_atrás(ifstream &bh_file)
{
    // Recuperamos la meta del fichero de hechos
    // Inicializamos la base de hechos
    //bh = HechosIniciales
    vector<hecho> bh;
    hecho meta = inicializa_bh(bh_file, bh); 
    bh_file.close();

    //Mostramos lo leído
    // cout << "Meta: " << meta.id << endl;
    // cout << "Base de Hechos: " << lista_hechos_string(bh) << endl;
    // cout << "Base de Conocimiento: " << lista_reglas_string(bc) << endl;
    // exit(EXIT_FAILURE);

    solucion sol = verificar(meta, bh, "");

    cout << "---------------------------------------------------------" <<endl;
    if(sol.verificado)
    {
        cout << "Solucion\tFC=" << setprecision(2) << sol.fc << endl;
        if(sol.fc > 0.5)
            cout << "Con esta información. Creemos que la meta "<< meta.id <<" es cierto" << endl;
        else
            cout << "Con esta información. Creemos que la meta "<< meta.id <<" es falso" << endl;
    }
    else
    {
        cout << "No se ha encontrado solución" << endl;
    }
}

int main(int argc, char **argv){
    if(argc < 3  || argc > 4 || strcmp(argv[1], "-h") == 0)
    {
        print_help();
        exit(EXIT_FAILURE);
    }

    ifstream bc_file;
    bc_file.open(argv[1], ios::in);
    if(!bc_file.is_open())
    {
        cerr << "Error al abrir el fichero de base de conocimiento" << endl;
        exit(EXIT_FAILURE);
    }
    //Base de conocimiento = Variable Global
    inicializa_bc(bc_file);
    bc_file.close();
    
    ifstream bh_file;
    bh_file.open(argv[2], ios::in);
    if(!bh_file.is_open())
    {
        cerr << "Error al abrir el fichero de base de hechos" << endl;
        exit(EXIT_FAILURE);
    }
    encadenamiento_hacia_atrás(bh_file);

    
    exit(EXIT_SUCCESS);
}