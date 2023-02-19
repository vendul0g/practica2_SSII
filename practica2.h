#ifndef PRACTICA2_H
#define PRACTICA2_H
#include <iostream>
#include <vector>

using namespace std;

enum operador
{
    AND,
    OR
};

struct _hecho
{
    string id;
    float fc; //factor de certeza
};
typedef struct _hecho hecho;

struct _regla
{
    int id;
    vector<hecho> alpha;
    enum operador op;
    hecho beta;
    float fc;
};
typedef struct _regla regla;

#endif // !Practica2_H