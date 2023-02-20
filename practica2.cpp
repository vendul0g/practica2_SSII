#include "practica2.h"
#include <iostream>
#include <vector>

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
    vector<regla> bc(5);

    bc[0].id = 1;
	bc[0].alpha.resize(2);
	bc[0].op = AND;
	bc[0].alpha[0].id = 'a';
	bc[0].alpha[1].id = 'b';
	bc[0].beta.id = 'p';
	bc[0].fc = 0.7;

    bc[1].id = 2;
    bc[1].alpha.resize(2);
    bc[1].op = AND;
    bc[1].alpha[0].id = 'b';
    bc[1].alpha[1].id = 'c';
    bc[1].beta.id = 'm';
    bc[1].fc = 0.5;

    bc[2].id = 3;
    bc[2].alpha.resize(3);
    bc[2].op = AND;
    bc[2].alpha[0].id = 'd';
    bc[2].alpha[1].id = 'e';
    bc[2].alpha[2].id = 'f';
    bc[2].beta.id = 'n';
    bc[2].fc = 0.65;

    bc[3].id = 4;
    bc[3].alpha.resize(2);
    bc[3].op = AND;
    bc[3].alpha[0].id = 'n';
    bc[3].alpha[1].id = 'g';
    bc[3].beta.id = 'm';
    bc[3].fc = 0.6;

    bc[4].id = 5;
    bc[4].alpha.resize(2);
    bc[4].op = AND;
    bc[4].alpha[0].id = 'h';
    bc[4].alpha[1].id = 'm';
    bc[4].beta.id = 'q';
    bc[4].fc = 0.9;

    return bc;
}

vector<hecho> inicializa_bh()
{//TODO borrar
    vector<hecho> bh(6);
    
	bh[0].id = 'a';
	bh[0].fc = 0.6;

	bh[1].id = 'b';
	bh[1].fc = 0.4;

	bh[2].id = 'c';
	bh[2].fc = 0.9;

	bh[3].id = 'f';
	bh[3].fc = 0.7;

	bh[4].id = 'g';
	bh[4].fc = 0.8;

	bh[5].id = 'h';
	bh[5].fc = -0.3;

    return bh;
}

hecho recuperar_meta()
{
    hecho meta;
    meta.id = 'q';
    return meta;
}

bool contenida(hecho meta, vector<hecho> bh)
{
    for(size_t i = 0; i < bh.size(); i++)
    {
        if(bh[i].id == meta.id)
            return true;
    }
    return false;
}

vector<regla> equiparar(vector<regla> bc, hecho meta)
{
    vector<regla> cc;
    for(size_t i = 0; i < bc.size(); i++)
    {
        if(bc[i].beta.id == meta.id)
            cc.push_back(bc[i]);
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

/**
 * @brief Verifica si la meta está en la base de hechos
 * @param meta Meta a verificar
 * @param bh Base de Hechos
 * @param bc Base de conocimiento
 * @return true si la meta está en la base de hechos y las reglas para llegar a ella
*/
solucion verificar (hecho meta, vector<hecho> bh)
{
    solucion sol;
    sol.verificado = false;
    
    // Comprobamos si la meta está en la base de hechos
    if(contenida(meta, bh))
    {
        sol.verificado = true;
        return sol;
    }
    
    cout << endl;
    
    vector<regla> cc = equiparar(bc, meta);

    while(cc.size() > 0 && !sol.verificado)
    {
        sol.reglas.resize(1);//Habrá al menos una regla en la solución

        cout << "CC=" << print_lista_reglas(cc) << endl;

        //R = Resolver(CC)
        regla r = cc[0];
        cout << "R={R" << r.id << "}" << endl;

        //Añadimos R a la solución
        sol.reglas.insert(sol.reglas.begin(), r);

        //Eliminar(R,CC)
        cc.erase(cc.begin());
        cout<<"Eliminar R"<<r.id<<" --> CC=" << print_lista_reglas(cc) << endl;

        vector<hecho> nuevas_metas = extraer_antecedentes(r);
        cout << "NuevasMetas=" << print_lista_hechos(nuevas_metas) << endl;

        sol.verificado = true;

        while(nuevas_metas.size() > 0 && sol.verificado)
        {
            //Nmet = SeleccionarMeta(NuevasMetas);
            hecho n_met = nuevas_metas[0];
            cout << "Meta=" << n_met.id << endl;

            //Eliminar(Nmet,NuevasMetas)
            nuevas_metas.erase(nuevas_metas.begin());
            cout << "NuevasMetas=" << print_lista_hechos(nuevas_metas) << endl;

            cout << "Verificar(" << n_met.id << ", "<< print_lista_hechos(bh) << ")";
            solucion aux = verificar(n_met, bh);
            sol.verificado = aux.verificado;

            //Añadimos las reglas de la solución auxiliar
            for(size_t i = 0; i < aux.reglas.size(); i++)
            {
                sol.reglas[i + sol.reglas.size() - aux.reglas.size()] = aux.reglas[i];
            }
            
            cout << " --> " << sol.verificado << endl;

            cout << "BH=" << print_lista_hechos(bh) << endl;
        }

        if(sol.verificado)
        {
            //Añadir(Meta, BH)
            bh.push_back(meta);
        }
    }
    if(cc.size() > 0 && sol.verificado)
    {
        cout<<"verificado = "<<sol.verificado<<"; CC="<<print_lista_reglas(cc)
            << "; BH=" << print_lista_hechos(bh) << endl;
    }
    return sol;
}

void encadenamiento_hacia_atrás()
{
    // Recuperamos la meta del fichero de hechos
    hecho meta = recuperar_meta();

    // Inicializamos la base de hechos
    vector<hecho> bh = inicializa_bh(); //bh = HechosIniciales
    solucion sol = verificar(meta, bh);
    if(sol.verificado)
    {
        cout << "Solución= "<<print_lista_reglas(sol.reglas) << endl;
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