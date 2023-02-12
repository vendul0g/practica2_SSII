#ifndef PRACTICA2_H

enum operador
{
    AND,
    OR
};

struct regla
{
    int id;
    int alpha_size;
    struct hecho *alpha;
    enum operador op;
    //int a_index;
    string beta;//será char*
};

struct hecho
{
    char id;
    float fc; //factor de certeza
};

#endif // !Practica2_H