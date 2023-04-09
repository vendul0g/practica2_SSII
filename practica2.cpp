#include "practica2.h"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

//Variables globales
vector<regla> bc;

string print_lista_reglas(vector<regla> list)
{
    string s = "{";
    for(size_t i = 0; i < list.size(); i++)
    {
        s += "R" + to_string(list[i].id);
        if(i < list.size() - 1)
            s += ", ";
    }
    s+= "}";
    return s;
}

string print_lista_hechos(vector<hecho> list)
{
    string s = "{";
    for(size_t i = 0; i < list.size(); i++)
    {
        s += list[i].id;
        if(i < list.size() - 1)
            s += ", ";
    }
    s+= "}";
    return s;
}

vector<regla> inicializa_bc()
{//TODO borrar
    vector<regla> bc(3);

    bc[0].id = 1;
	bc[0].alpha.resize(1);
	bc[0].op = OR;
	bc[0].alpha[0].id = 'a';
	bc[0].beta.id = 'g';
	bc[0].fc = 0.5;

    bc[1].id = 2;
    bc[1].alpha.resize(2);
    bc[1].op = AND;
    bc[1].alpha[0].id = 'b';
    bc[1].alpha[1].id = 'c';
    bc[1].beta.id = 'g';
    bc[1].fc = 0.9;

    bc[2].id = 3;
    bc[2].alpha.resize(1);
    bc[2].op = OR;
    bc[2].alpha[0].id = 'g';
    bc[2].beta.id = 'r';
    bc[2].fc = 0.99;

    // bc[3].id = 4;
    // bc[3].alpha.resize(2);
    // bc[3].op = AND;
    // bc[3].alpha[0].id = 'n';
    // bc[3].alpha[1].id = 'g';
    // bc[3].beta.id = 'm';
    // bc[3].fc = 0.6;

    // bc[4].id = 5;
    // bc[4].alpha.resize(2);
    // bc[4].op = AND;
    // bc[4].alpha[0].id = 'h';
    // bc[4].alpha[1].id = 'm';
    // bc[4].beta.id = 'q';
    // bc[4].fc = 0.9;

    return bc;
}

vector<hecho> inicializa_bh()
{//TODO borrar
    vector<hecho> bh(3);
    
	bh[0].id = 'a';
	bh[0].fc = 1;

	bh[1].id = 'b';
	bh[1].fc = 1;

	bh[2].id = 'c';
	bh[2].fc = 0.5;

	// bh[3].id = 'f';
	// bh[3].fc = 0.7;

	// bh[4].id = 'g';
	// bh[4].fc = 0.8;

	// bh[5].id = 'h';
	// bh[5].fc = -0.3;

    return bh;
}

hecho recuperar_meta()
{
    hecho meta;
    meta.id = 'r';
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

vector<regla> equiparar(vector<regla> bc, hecho meta)
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

vector<hecho> extraer_antecedentes(regla r)
{
    vector<hecho> antecedentes;
    for(size_t i = 0; i < r.alpha.size(); i++)
    {
        antecedentes.push_back(r.alpha[i]);
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
    cout << "Calculamos FC de antecedentes de regla " << r.id <<" --> Caso 1. FC("<<r.id<<") = ";
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
        cout << "2. FC("<<r.id<<") = ";
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
    cout << "Calculamos FC de antecedentes de "<<meta.id<<" --> Caso 2. FC("<<meta.id<<") = ";
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

float calcular_combinacion_evidencia(regla r, float ant_fc)
{    /**
     * Caso 3. Encadenamiento de evidencia: se combinan dos reglas de manera
     * que el resultado de una regla es la entrada de la otra
     * > R1: si e entonces s 
     * > R2: si s entonces h
     * -- Se usa cuando una regla tiene un antecedente
    */
    cout << "Calculamos FC de regla "<<r.id<<" y sus antecedentes --> Caso 3. FC("<<r.id<<") = ";    
    float sol, aux;

    if(ant_fc < 0){
        aux = 0;
    }
    else{
        aux = ant_fc;
    }
    sol = r.fc * aux;
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
    
    vector<regla> cc = equiparar(bc, meta);
    float m_fc[cc.size()];

    //Inicializamos el índice del bucle
    i = 0;
    while(cc.size() > 0 /*&& !sol.verificado*/)
    {
        cout << profundidad << "CC=" << print_lista_reglas(cc) << endl;

        //R = Resolver(CC)
        regla r = cc[0];
        cout << profundidad << "R={R" << r.id << "}" << endl;

        //Creamos el contenedor donde se calcula el fc global de R
        float r_fc[r.alpha.size()];

        //Eliminar(R,CC)
        cc.erase(cc.begin());
        cout<<profundidad<<"Eliminar R"<<r.id<<" --> CC=" << print_lista_reglas(cc) << endl;

        vector<hecho> nuevas_metas = extraer_antecedentes(r);
        cout << profundidad << "NuevasMetas=" << print_lista_hechos(nuevas_metas) << endl;

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
            cout << profundidad << "NuevasMetas=" << print_lista_hechos(nuevas_metas) << endl;

            cout << profundidad << "Verificar(" << n_met.id << ", "<< print_lista_hechos(bh) << ")";
            solucion aux = verificar(n_met, bh, ": ");
            sol.verificado = aux.verificado;
            r_fc[k]=aux.fc;
            
            cout << profundidad << " --> ";
            sol.verificado == true ? cout << "TRUE" : cout << "FALSE";
            cout << endl;

            cout << profundidad << "BH=" << print_lista_hechos(bh) << endl;

            //Actualizamos el índice del bucle
            k++;
        }

        //Calcular FC de la regla r
        float ant_fc;
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
        cout<<"; CC="<<print_lista_reglas(cc)<< "; BH=" << print_lista_hechos(bh) << endl;
        
        sol.fc = meta.fc;
    }
    return sol;
}

void encadenamiento_hacia_atrás()
{
    // Recuperamos la meta del fichero de hechos
    hecho meta = recuperar_meta();

    // Inicializamos la base de hechos
    vector<hecho> bh = inicializa_bh(); //bh = HechosIniciales
    solucion sol = verificar(meta, bh, "");
    cout << "---------------------------------------------------------" <<endl;
    if(sol.verificado)
    {
        cout << "Solucion\tFC=" << setprecision(2) << sol.fc << endl;
        if(sol.fc > 0.5)
            cout << "Con esta información. Creemos que "<< meta.id <<" es cierto" << endl;
        else
            cout << "Con esta información. Creemos que "<< meta.id <<" es falso" << endl;
    }
    else
    {
        cout << "No se ha encontrado solución" << endl;
    }
}

int main(){
    //Base de conocimiento = Global
    bc = inicializa_bc();
    encadenamiento_hacia_atrás();
    return 0;
}