/*Libraria de Grafo Dinamico Implementado Mediante Lista de Adyacencia*/
#ifndef _GRAFO_D_H
#define _GRAFO_D_H

#include <stdlib.h>

#ifndef Vertice
#define Vertice int
#endif

#ifndef Arista
#define Arista int
#endif

/*-----------------------Definicion de los tipos de datos internos---------------------*/

typedef struct nodo_a {
    struct nodo_a* sig;
    Arista arista;
    Nodo_V* adyacente; //Nodo_V* y no Vertice* para facilitar recorridos
} Nodo_A;

typedef struct nodo_v {
    struct nodo_v* sig;
    Vertice v;
    Lista_A aristas;
} Nodo_V;

typedef struct lista_a {
    Nodo_A* cabeza;
    size_t tamano;
} Lista_A;

typedef struct lista_v {
    Nodo_V* cabeza;
    size_t tamano;
} Lista_V;

typedef struct grafo_d {
    Lista_V vertices; //vertices.tamano == orden
    size_t tamano;
    int (*cmp_vt) (Vertice, Vertice);
    int (*cmp_ar) (Arista, Arista);
} Grafo_D;

/*---------------------------Operaciones basicas del grafo------------------------------*/

/*Crear un grafo vacio para empezar a realizar operaciones
Nota: Seria buena idea forzar al usario a registrar las funciones cmp_vt y cmp_ar?*/
static Grafo_D* grafo_d_crear(void) {
    Grafo_D* grafo = (Grafo_D*)malloc(sizeof(Grafo_D));
    if(!grafo) return NULL;
    grafo->vertices.cabeza=NULL;
    grafo->vertices.tamano=0;
    grafo->tamano=0;
    grafo->cmp_vt=NULL;
    grafo->cmp_ar=NULL;
    return;
}

/*Regresa si el grafo se encuentra vacio*/
static inline bool grafo_d_isempty(Grafo_D* grafo) {
    return grafo->vertices.cabeza==NULL && grafo->vertices.tamano==0;
}

/*Libera la memoria reservada dinamicamente para el grafo*/
static void grafo_d_destruir(Grafo_D* grafo) {
    Nodo_V* vptr = grafo->vertices.cabeza, *vtmp;
    while(vptr!=NULL) {
        Nodo_A *aptr = vptr->aristas.cabeza, *atmp;
        while(aptr!=NULL) {
            atmp=aptr;
            aptr=aptr->sig;
            free(atmp);
        }
        vtmp=vptr;
        vptr=vptr->sig;
        free(vtmp);
    }
    return;
}

/*----------------------------Operaciones sobre los vertices-----------------------------*/
static Nodo_V* grafo_d_insertar_vertice(Grafo_D* grafo, Vertice v) {
    Nodo_V* nodo = (Nodo_V*)malloc(sizeof(Nodo_V));
    if(!nodo) return NULL;
    nodo->v=v;
    nodo->aristas.cabeza=NULL;
    nodo->aristas.tamano=0;
    nodo->sig=grafo->vertices.cabeza;
    grafo->vertices.cabeza=nodo;
    return nodo;
}

/*Que pasa si hay varios vertices iguales?????*/
static bool grafo_d_eliminar_vertice(Grafo_D* grafo, Vertice v) {
    
}

static Vertice grafo_d_buscar_vetice(Grafo_D* grafo, Vertice ref);
static Lista_V* grafo_d_buscar_vertices(Grafo_D* grafo, Vertice ref);
static const Lista_A* grafo_d_get_aristas_vertice(Grafo_D* grafo, Vertice v);

/*----------------------------Operaciones sobre las aristas------------------------------*/
static Nodo_V* grafo_d_insertar_arista(Grafo_D* grafo, Arista a, Vertice ini, Vertice fin) {

}

static bool grafo_d_elminar_arista(Grafo_D* grafo, Arista a, Vertice ini, Vertice fin) {

}

static Arista grafo_d_buscar_arista(Grafo_D* grafo, Arista ref);
static Lista_A* grafo_d_buscar_aristas(Grafo_D* grafo, Arista ref);
static Arista grafo_d_buscar_arista_entre_vertices(Grafo_D* grafo, Vertice ini, Vertice fin);
static Lista_A* grafo_d_buscar_aristas_entre_vertices(Grafo_D* grafo, Vertice ini, Vertice fin);
static Arista grafo_d_buscar_arista_estricto(Grafo_D* grafo, Arista ref, Vertice ini, Vertice fin);
static Lista_A* grafo_d_buscar_aristas_estricto(Grafo_D* grafo, Arista ref, Vertice ini, Vertice fin);

/*------------------------------Operaciones sobre el grafo--------------------------------*/
static Arista** grafo_d_generar_mat_adj(Grafo_D* grafo);

#endif