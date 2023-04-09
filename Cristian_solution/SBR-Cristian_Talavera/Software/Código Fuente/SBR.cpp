#include "SBR.h"

// Fichero en el que se almacenar� la salida del programa
ofstream salida;

int 
main(int argc, char *argv[])
{
	// Comprobamos los par�metros
    if (argc != 4) 
    {
        cerr << "USO: SBR.exe <BC.txt> <config.txt> <BH.txt>" << endl;
        return -1;    
    }
    
    char *bConocimiento = argv[1];
    char *config = argv[2];
    char *bHechos = argv[3];
    
    char out[50];
    strcpy(out, "outputs/SBR_");
    strcat(out, bHechos);
    
    // Abrimos el fichero de salida
    salida.open(out);

    // Lee los ficheros y llena las estructuras de datos
    vector<Regla> reglas = leeBaseConocimiento(bConocimiento);
    string objetivo = leeConfiguracion(config, reglas);
    vector<Par> hechosIniciales = leeBaseHechos(bHechos);
    
    // Imprimos la base de hechos inicial y el objetivo
    salida << " ---- Base de Hechos Inicial ---- " << endl;
    for(vector<Par>::iterator it = hechosIniciales.begin(); it != hechosIniciales.end(); ++it) 
    {
        Par hecho = *it;
        salida << " " << hecho.key << " " << hecho.op << " " << hecho.strValue << endl;     
    }  
    salida << endl << " -------------------------------- " << endl;
    salida << " Objetivo: " << objetivo << endl;
    salida << " -------------------------------- " << endl << endl;
	
	// Ejecutamos el algoritmo principal del Motor de Inferencias
	int ok = encadenamientoHaciaDelante(hechosIniciales, reglas, objetivo, reglas.size());
	if (ok)
		cout << " - " << bConocimiento << " : proceso finalizado CON exito. (revisar fichero " << out << " para mas detalles)" << endl;
	else
		cout << " - " << bConocimiento << " : proceso finalizado SIN exito. (revisar fichero " << out << " para mas detalles)" << endl;

	return 0;
}

// Algoritmo principal del MI
bool
encadenamientoHaciaDelante(vector<Par> hechosIniciales, vector<Regla> &bc, string objetivo, int numReglas) 
{
	vector<Par> bh = hechosIniciales;
	vector<Regla> conjuntoConflicto = equiparar(bc, bh, numReglas);  
    
	while(!contenido(objetivo, bh) && !conjuntoConflicto.empty())  
    {
		conjuntoConflicto = equiparar(bc, bh, numReglas);
    	if (!conjuntoConflicto.empty()) 
        {
            Regla bc = resolver(conjuntoConflicto); 
    		Par nuevoHecho = aplicar(bc, bc, bh);
    		actualizar(bh, nuevoHecho);
    	}   
    }
    
    // Imprimimos la base de hechos final y la conclusi�n obtenida
    Par solucion;
    salida << endl << " ----- Base de Hechos Final ----- " << endl;
    for(vector<Par>::iterator it = bh.begin(); it != bh.end(); ++it)
    {
    	Par hecho = *it;
        salida << " " << hecho.key << " " << hecho.op << " " << hecho.strValue << endl;
        solucion = hecho;
    }
    salida << " -------------------------------- " << endl << endl;
	if (contenido(objetivo, bh)) {
        salida << " ---- Exito: " << solucion.key << " = " << solucion.strValue << " ---- " << endl;
        return true;
	}
	else {
	    salida << " ---- No Exito ---- " << endl;
		return false;    
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////// Funciones Motor de Inferencia /////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Comprueba si est� contenido el objetivo en la base de hechos
bool
contenido(string objetivo, vector<Par> bh)
{
    for(vector<Par>::iterator it = bh.begin(); it != bh.end(); ++it)
    {
        Par hecho = *it;                
        if (objetivo.compare(hecho.key)==0) return true;
    }
    return false;
}

// Cada antecedente se compara con todos los hechos de la base de hechos y si
// es igual, incrementa el contador, cuando dicho contador sea igual al
// n�mero de antecedentes de la regla, a�ade dicha regla al conjunto conflicto
vector<Regla>
equiparar(vector<Regla> bc, vector<Par> bh, int num_reglas)
{
    vector<Regla> conjuntoConflicto;
    
    for(int i=0; i<num_reglas; i++)
    {
        if (bc[i].marcada == false) 
		{
			vector<Par> antecedentes = bc[i].listaAntecedentes;
	        int numAntecedentes = antecedentes.size();
	        int antTrue = 0; // Contador de antecedentes verdaderos
	        
	        for(vector<Par>::iterator it = antecedentes.begin(); it != antecedentes.end(); ++it) 
	        {
	            Par antecedente = *it;
	            string keyA = antecedente.key;
	            string valueA = antecedente.strValue;
	            string op = antecedente.op;
	            
	            for(vector<Par>::iterator it1 = bh.begin(); it1 != bh.end(); ++it1)
	            {
	                Par hecho = *it1;
	                string keyH = hecho.key;
	                string valueH = hecho.strValue;
	                
	                // Comprobar los valores
	                if (keyA.compare(keyH) == 0)
	                {	
						if (hecho.intValue != 0 && antecedente.intValue != 0) {
		                    if (op.compare(">") == 0) {
		                        if (hecho.intValue > antecedente.intValue)	antTrue++;   
		                    }    
		                    else if (op.compare("<") == 0) {
		                        if (hecho.intValue < antecedente.intValue) 	antTrue++;   
		                    }   
		                    else if (op.compare(">=") == 0) {
		                        if (hecho.intValue >= antecedente.intValue)	antTrue++;
		                    }   
		                    else if (op.compare("<=") == 0) {
		                        if (hecho.intValue <= antecedente.intValue) antTrue++;
		                    }
							else if (op.compare("=") == 0) {
		                        if (hecho.intValue == antecedente.intValue) antTrue++;
		                    }      
		                }
	                	else if (valueA.compare(valueH) == 0)
							antTrue++; 
					}
	            }
	        }
	        // Si todos los antecedentes son verdaderos, a�ado dicha regla al conjunto conflicto
	        if (antTrue == numAntecedentes)	
				conjuntoConflicto.push_back(bc[i]);
    	}
    }
    
    return conjuntoConflicto;
}


// Selecciona una regla del conjunto conflicto, la de mayor prioridad, o en caso de empate
// retorna la regla con menor �ndice
Regla
resolver(vector<Regla> conjuntoConflicto)
{
    Regla reglaMax, reglaMin;
	for(vector<Regla>::iterator it = conjuntoConflicto.begin(); it != conjuntoConflicto.end(); ++it)
    {
        Regla regla = *it;
        
        if (regla.prioridad > reglaMax.prioridad)
        	reglaMax = regla;
		else {
        	reglaMin = regla;
			salida << " Aplicando R" << reglaMin.id << ":\t" << reglaMin.str << endl;
			return reglaMin;
		}
    }
    
	salida << " Aplicando R" << reglaMax.id << ":\t" << reglaMax.str << endl;
	return reglaMax;	
}

// Marca la regla que se ejecutar� y retorna el consecuente que ser� 
// a�adido a la base de hechos
Par
aplicar(vector<Regla> &bc, Regla regla, vector<Par> bh)
{
	Par consecuente = regla.consecuente;
	bc[regla.id-1].marcada = true; 
	return consecuente;
}

// A�ade el nuevo hecho a la base de hechos
void
actualizar(vector<Par> &bh, Par nuevoHecho)
{
	if (!contenido(nuevoHecho.key, bh))
		bh.push_back(nuevoHecho);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Funciones Ficheros //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Lee el contenido de la base de conocimiento y almacena el conjunto de 
// reglas en una estructura de datos
vector<Regla> 
leeBaseConocimiento(char *bc) 
{
    ifstream f(bc);
    string linea, value;
    
    if (f == NULL) 
    { 
        cerr << "Error: Archivo " << bc << " no encontrado." << endl;
        exit(1);
    }
    
    getline(f, linea);
    salida << " --- " << linea << " --- " << endl;
    
    getline(f, linea);
    int num_reglas = atoi(linea.c_str());
    vector<Regla> reglas;
    
    for(int i=0; i<num_reglas; i++) 
    {
        getline(f, linea);
        int found_tp = linea.find(":");
        
        string reglaStr = linea.substr(found_tp+1);
        
        int found_si = reglaStr.find("Si");
        int found_entonces = reglaStr.find("Entonces");
        
        ///////////////////////// Lista de antecedentes ////////////////////////
        vector<Par> listaAntecedentes;
        string antecedentes = reglaStr.substr(found_si+3, found_entonces-5);
        
        string delimiter = " y ";
        size_t pos = 0;
        string antecedente;
        int index, offset;
        while ((pos = antecedentes.find(delimiter)) != string::npos) //  NumAntecedentes > 1
        { 
            antecedente = antecedentes.substr(0, pos);
            
        	Par ante;
			checkOperator(index, offset, ante.op, antecedente);
      
            string key = antecedente.substr(0, index-1);
            string value = antecedente.substr(index+offset);

            ante.key = key;
            ante.strValue = value;
            ante.intValue = atoi(value.c_str());
    
            listaAntecedentes.push_back(ante);
            antecedentes.erase(0, pos + delimiter.length());
        }
        // A�ade el �ltimo antecedente a la lista (1 antecedente)
        Par ante;
		checkOperator(index, offset, ante.op, antecedentes);
             
        string key = antecedentes.substr(0, index-1);
        string value = antecedentes.substr(index+offset);
       
		ante.key = key;
        ante.strValue = value;
        ante.intValue = atoi(value.c_str());
        
        listaAntecedentes.push_back(ante);
        ////////////////////////////////////////////////////////////////////////
        
        
        ////////////////////////////// Consecuente  ////////////////////////////
        Par consecuente;
        string consecuenteStr = reglaStr.substr(found_entonces+9);
    
        checkOperator(index, offset, consecuente.op, consecuenteStr);       
        
		key = consecuenteStr.substr(0, index-1);
        value = consecuenteStr.substr(index+offset);
        
		consecuente.key = key;
        consecuente.strValue = value;
        consecuente.intValue = atoi(value.c_str());
        ////////////////////////////////////////////////////////////////////////
        
        // Se establecen los campos de la regla y se a�ade al conjunto
        Regla regla;
        regla.listaAntecedentes = listaAntecedentes;
        regla.consecuente = consecuente;
        regla.prioridad = 0;
        regla.str = reglaStr;
        regla.marcada = false;
        regla.id = i+1;
        reglas.push_back(regla);
    }
        
    f.close();
    return reglas;
}

// Establece la meta que estamos buscando y la prioridad de las reglas
string 
leeConfiguracion(char *config, vector<Regla> &reglas) 
{    
    ifstream f(config);
    string linea, objetivo;
    
    if (f == NULL) 
    { 
        cerr << "Error: Archivo " << config << " no encontrado." << endl;
        exit(1);
    }
    
    while(!f.eof()) 
    {
        getline(f, linea);
        if (linea.compare("OBJETIVO") == 0) 
        {
            // Establece el objetivo que queremos obtener 
            getline(f, linea);
            objetivo = linea;
        }
        else if (linea.compare("PRIORIDADES-REGLAS") == 0) 
        {
            // Establece las distintas prioridades de las reglas 
            getline(f, linea);
            int num_reglas = atoi(linea.c_str());  
            for(int i = 0; i < num_reglas; i++) 
            {
                getline(f, linea);
                reglas[i].prioridad = atoi(linea.c_str());  
            }
        }
    }
    
    f.close();
    return objetivo;
}

// Lee el contenido de la base de hechos inicial
vector<Par> 
leeBaseHechos(char *bh) 
{
    ifstream f(bh);
    string linea;
    
    if (f == NULL) 
    { 
        cerr << "Error: Archivo " << bh << " no encontrado." << endl;
        exit(1);
    }
    
    getline(f, linea); 
    int num_hechos = atoi(linea.c_str());  

    vector<Par> hechos;
    for(int i=0; i<num_hechos; i++) 
    {
        getline(f, linea); 

        Par hecho;
        int index, offset;
        checkOperator(index, offset, hecho.op, linea);
		      
        string key = linea.substr(0, index-1);
        string value = linea.substr(index+offset);
        
        hecho.key = key;
        hecho.strValue = value;
        hecho.intValue = atoi(value.c_str());  
    
        hechos.push_back(hecho);
    }

    f.close();
    return hechos;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Funciones Auxiliares //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void 
imprimeReglas(vector<Regla> reglas) 
{
    // Imprimimos la lista de reglas
    for(vector<Regla>::iterator it = reglas.begin(); it != reglas.end(); ++it)
    {
    	Regla regla = *it;
        cout << "-----------------------------------------" << endl;
        string marca;
        if (regla.marcada) 
            marca = "true";
        else 
            marca = "false";
        
        cout << "R" << regla.id << " - prioridad: " << regla.prioridad << " - marcada: " << marca << endl;    
        cout << regla.str << endl;
        cout << "\tLista de Antecedentes: " << endl;
        vector<Par> antecedentes = regla.listaAntecedentes;
        for(vector<Par>::iterator it = antecedentes.begin(); it != antecedentes.end(); ++it)
        {
            Par antecedente = *it;
            cout << "\t\t" << antecedente.key << antecedente.op << antecedente.strValue << endl;
        }
        
        cout << "\tConsecuente: " << endl;
        Par consecuente = regla.consecuente;
        cout << "\t\t" << consecuente.key << consecuente.op << consecuente.strValue << endl;
        cout << "-----------------------------------------" << endl;
    }
}

// Para un antecedente/consecuente dado, establecemos el operador que tendr� y 
// su correspondiente desplazamiento, que ser� usado posteriormente para obtener
// su valor y su identificador
void 
checkOperator(int &index, int &offset, string &op, string search)
{
	int found_ge = search.find(">=");   
    int found_le = search.find("<=");
    int found_eq = search.find("=");
    int found_gt = search.find(">");
	int found_lt = search.find("<");  
     
	offset = 2;  
    
	if ((uint)found_ge != string::npos){ 
		op = ">=";
        index = found_ge;
        offset++;
    }
    else if ((uint)found_le != string::npos)  
    {   
		op = "<=";
        index = found_le;
        offset++;
    }
    else if ((uint)found_eq != string::npos)  {
        op = "=";
        index = found_eq;
    }
    else if ((uint)found_gt != string::npos)  
    {
        op = ">";
        index = found_gt;   
    }
    else if ((uint)found_lt != string::npos)  {
        op = "<";
		index = found_lt;
    }
    else op = "";
}

