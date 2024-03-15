#ifndef STRUCTS_H
#define STRUCTS_H

struct Shop{
    char id[15];
    char name[15];
};

struct Product{
    char id[15];
    char name[15];
};

int matcher(void *rec, void *key);
#endif