/*Libraria de Grafo Dinamico Implementado Mediante Lista de Adyacencia*/
#ifndef _GRAFO_D_H
#define _GRAFO_D_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef Vertice
#define Vertice int
#endif

#ifndef Arista
#define Arista int
#endif

/*-----------------------Definicion de los tipos de datos internos---------------------*/

typedef struct vect_v {
    size_t tamano;
    Vertice* vertices[];
} Vect_V;

typedef struct vect_a {
    size_t tamano;
    Arista* aristas[];
} Vect_A;

struct nodo_a;
typedef struct nodo_a Nodo_A;
struct nodo_v;
typedef struct nodo_v Nodo_V;

struct nodo_a {
    struct nodo_a* sig;
    Arista ar;
    Nodo_V* fin;
};

struct nodo_v {
    struct nodo_v* sig;
    Vertice vt;
    size_t grado_s;
    Nodo_A* lista_ady;
};

typedef struct grafo_d {
    Nodo_V* lista_ady;
    size_t orden;
    size_t tamano;
    bool (*cmp_vt)(Vertice*, Vertice*);
    bool (*cmp_ar)(Arista*, Arista*);
} Grafo_D;

/*---------------------------Operaciones basicas del grafo------------------------------*/

/*Crear un grafo vacio para empezar a realizar operaciones*/
static Grafo_D* grafo_d_crear(bool (*cmp_vt)(Vertice*, Vertice*), bool (*cmp_ar)(Arista*, Arista*)) {
    Grafo_D* grafo = (Grafo_D*)malloc(sizeof(Grafo_D));
    if(!grafo) return NULL;
    grafo->lista_ady=NULL;
    grafo->orden=0;
    grafo->tamano=0;
    grafo->cmp_vt=cmp_vt;
    grafo->cmp_ar=cmp_ar;
    return grafo;
}

/*Regresa si el grafo se encuentra vacio*/
static inline bool grafo_d_isempty(Grafo_D* grafo) {
    return grafo->lista_ady==NULL && grafo->orden==0;
}

/*Libera la memoria reservada dinamicamente para el grafo*/
static void grafo_d_destruir(Grafo_D* grafo) {
    Nodo_V* vptr = grafo->lista_ady, *vtmp;
    while(vptr!=NULL) {
        Nodo_A *aptr = vptr->lista_ady, *atmp;
        while(aptr!=NULL) {
            atmp=aptr;
            aptr=aptr->sig;
            free(atmp);
        }
        vtmp=vptr;
        vptr=vptr->sig;
        free(vtmp);
    }
    free(grafo);
    return;
}

/*----------------------------Operaciones sobre los vertices-----------------------------*/
static Vertice* grafo_d_insertar_vertice(Grafo_D* grafo, Vertice vt) {
    Nodo_V* nuevo = (Nodo_V*)malloc(sizeof(Nodo_V));
    if(!nuevo) return NULL;
    nuevo->vt=vt;
    nuevo->lista_ady=NULL;
    nuevo->grado_s=0;
    nuevo->sig=grafo->lista_ady;
    grafo->lista_ady=nuevo;
    ++(grafo->orden);
    return &(nuevo->vt);
}

static Vertice* grafo_d_buscar_vertice(Grafo_D* grafo, Vertice ref) {
    Nodo_V* vptr = grafo->lista_ady;
    while(vptr!=NULL) {
        if(grafo->cmp_vt(&(vptr->vt),&ref))
            return &(vptr->vt);
        vptr=vptr->sig;
    }
    return NULL;
}

static const Vect_V* grafo_d_buscar_vertices(Grafo_D* grafo, Vertice ref) {
    Vect_V* vectmp = (Vect_V*)malloc(sizeof(Vect_V)+sizeof(Vertice*)*(grafo->orden));
    if(!vectmp) return NULL;
    vectmp->tamano=0;
    Nodo_V* vptr=grafo->lista_ady;
    while(vptr!=NULL) {
        if(grafo->cmp_vt(&(vptr->vt), &ref)) {
            vectmp->vertices[vectmp->tamano]=&(vptr->vt);
            ++(vectmp->tamano);
        }
        vptr=vptr->sig;
    }
    Vect_V* vector = (Vect_V*)malloc(sizeof(Vect_V)+sizeof(Vertice*)*(vectmp->tamano));
    for(int i=0; i<vectmp->tamano; ++i) vector->vertices[i]=vectmp->vertices[i];
    vector->tamano=vectmp->tamano;
    free(vectmp);
    return (const Vect_V*)vector;
}

static void grafo_d_eliminar_vertice(Grafo_D* grafo, Vertice* vt) {
    Nodo_V* vptr = grafo->lista_ady, *vprev=NULL, *vtmp;
    while(vptr!=NULL) {
        Nodo_A* aptr = vptr->lista_ady, *aprev=NULL, *atmp;
        //Si el nodo es el que contiene el vertice lo eliminamos
        if(vt==&(vptr->vt)) {
            while(aptr!=NULL) {
                atmp=aptr;
                aptr=aptr->sig;
                free(atmp);
                --(grafo->tamano);
            }
            vtmp=vptr;
            if(vprev!=NULL) vprev->sig=vptr->sig; 
            else grafo->lista_ady=vptr->sig;
            free(vtmp);
            --(grafo->orden);
        }
        //Si no borramos los arcos que conectan al vertice
        else {
            while(aptr!=NULL) {
                if(vt==&(aprev->fin->vt)) {
                    atmp=aptr;
                    if(aprev!=NULL) aprev->sig=aptr->sig;
                    else vptr->lista_ady=aptr->sig;
                    free(atmp);
                    --(vptr->grado_s);
                    --(grafo->tamano);
                }
                aprev=aptr;
                aptr=aptr->sig;
            }
        }
        vprev=vptr; 
        vptr=vptr->sig;
    }
    return;
}

/*----------------------------Operaciones sobre las aristas------------------------------*/
static Arista* grafo_d_insertar_arista(Grafo_D* grafo, Arista ar, Vertice* ini, Vertice* fin) {
    Nodo_A* nuevo = (Nodo_A*)malloc(sizeof(Nodo_A));
    if(!nuevo) return NULL;
    nuevo->ar=ar; nuevo->fin=NULL;
    Nodo_V* vptr=grafo->lista_ady, *inicio=NULL;
    while(nuevo->fin==NULL || inicio==NULL) {
        if(ini==&(vptr->vt)) inicio=vptr;
        if(fin==&(vptr->vt)) nuevo->fin=vptr;
        vptr=vptr->sig;
    }
    nuevo->sig=inicio->lista_ady;
    inicio->lista_ady=nuevo;
    ++(inicio->grado_s);
    ++(grafo->tamano);
    return &(nuevo->ar);
}

static Arista* grafo_d_buscar_arista(Grafo_D* grafo, Arista ref) {
    Nodo_V* vptr = grafo->lista_ady;
    while(vptr!=NULL) {
        Nodo_A* aptr = vptr->lista_ady;
        while(aptr!=NULL) {
            if(grafo->cmp_ar(&(aptr->ar),&ref)) {
                return &(aptr->ar);
            }
            aptr=aptr->sig;
        }
        vptr=vptr->sig;
    }
    return NULL;
}

static const Vect_A* grafo_d_buscar_aristas(Grafo_D* grafo, Arista ref) {
    Vect_A* vectmp = (Vect_A*)malloc(sizeof(Vect_A)+sizeof(Arista*)*(grafo->tamano));
    if(!vectmp) return NULL;
    vectmp->tamano=0;
    Nodo_V* vptr = grafo->lista_ady;
    while(vptr!=NULL) {
        Nodo_A* aptr = vptr->lista_ady;
        while(aptr!=NULL) {
            if(grafo->cmp_ar(&(aptr->ar), &ref)) {
                vectmp->aristas[vectmp->tamano]=&(aptr->ar);
                ++(vectmp->tamano); 
            }
            aptr=aptr->sig;
        }
        vptr=vptr->sig;
    }
    Vect_A* vector = (Vect_A*)malloc(sizeof(Vect_A)+sizeof(Arista*)*(vectmp->tamano));
    for(int i=0; i<vectmp->tamano; ++i) vector->aristas[i]=vectmp->aristas[i];
    free(vectmp);
    return (const Vect_A*)vector;
}

static Arista* grafo_d_buscar_arista_entre_vert(Grafo_D* grafo, Vertice* ini, Vertice* fin) {
    Nodo_V* vptr = grafo->lista_ady;
    while(ini!=&(vptr->vt)) vptr=vptr->sig;
    Nodo_A* aptr = vptr->lista_ady;
    while(aptr!=NULL) {
        if(fin==&(aptr->fin->vt))
            return &(aptr->ar);
    }
    return NULL;
}

static const Vect_A* grafo_d_buscar_aristas_entre_vert(Grafo_D* grafo, Vertice* ini, Vertice* fin) {
    Nodo_V* vptr=grafo->lista_ady;
    while(vptr!=NULL && ini!=&(vptr->vt)) vptr=vptr->sig;
    Vect_A* vectmp = (Vect_A*)malloc(sizeof(Vect_A)+sizeof(Arista*)*vptr->grado_s);
    if(!vectmp) return NULL;
    vectmp->tamano=0;
    Nodo_A* aptr=vptr->lista_ady;
    while(aptr!=NULL) {
        if(fin==&(aptr->fin->vt)) {
            vectmp->aristas[vectmp->tamano]=&(aptr->ar);
            ++(vectmp->tamano); 
        }
        aptr=aptr->sig;
    }
    Vect_A* vector = (Vect_A*)malloc(sizeof(Vect_A)+sizeof(Arista*)*(vectmp->tamano));
    for(int i=0; i<vectmp->tamano; ++i) vector->aristas[i]=vectmp->aristas[i];
    free(vectmp);
    return (const Vect_A*)vector;
}

static Arista* grafo_d_buscar_arista_estricto(Grafo_D* grafo, Arista ref, Vertice* ini, Vertice* fin) {
    Nodo_V* vptr = grafo->lista_ady;
    while(ini!=&(vptr->vt)) vptr=vptr->sig;
    Nodo_A* aptr = vptr->lista_ady;
    while(aptr!=NULL) {
        if(fin==&(aptr->fin->vt) && grafo->cmp_ar(&(aptr->ar), &ref))
            return &(aptr->ar);
    }
    return NULL;
}

static const Vect_A* grafo_d_buscar_aristas_estricto(Grafo_D* grafo, Arista ref, Vertice* ini, Vertice* fin) {
    Nodo_V* vptr=grafo->lista_ady;
    while(vptr!=NULL && ini!=&(vptr->vt)) vptr=vptr->sig;
    Vect_A* vectmp = (Vect_A*)malloc(sizeof(Vect_A)+sizeof(Arista*)*vptr->grado_s);
    if(!vectmp) return NULL;
    vectmp->tamano=0;
    Nodo_A* aptr=vptr->lista_ady;
    while(aptr!=NULL) {
        if(fin==&(aptr->fin->vt) && grafo->cmp_ar(&(aptr->ar), &ref)) {
            vectmp->aristas[vectmp->tamano]=&(aptr->ar);
            ++(vectmp->tamano); 
        }
        aptr=aptr->sig;
    }
    Vect_A* vector = (Vect_A*)malloc(sizeof(Vect_A)+sizeof(Arista*)*(vectmp->tamano));
    for(int i=0; i<vectmp->tamano; ++i) vector->aristas[i]=vectmp->aristas[i];
    free(vectmp);
    return (const Vect_A*)vector;
}

static void grafo_d_elminar_arista(Grafo_D* grafo, Arista* ar) {
    Nodo_V* vptr = grafo->lista_ady;
    while(vptr!=NULL) {
        Nodo_A* aptr = vptr->lista_ady, *atmp;
        if(aptr!=NULL && ar==&(aptr->ar)) {
            vptr->lista_ady=aptr->sig;
            free(aptr);
            --(vptr->grado_s);
            --(grafo->tamano);
            return;
        }
        atmp=aptr; aptr=aptr->sig;        
        while(aptr!=NULL) {
            if(ar==&(aptr->ar)) {
                atmp->sig=aptr->sig;
                free(aptr);
                --(vptr->grado_s);
                --(grafo->tamano);
                return;
            }
            atmp=aptr;
            aptr=aptr->sig;
        }
        vptr=vptr->sig;
    }
    return;
}


/*------------------------------Operaciones sobre el grafo--------------------------------*/
static void grafo_d_print(Grafo_D* grafo) {
    printf("G: %p\n", grafo);
    Nodo_V* vptr=grafo->lista_ady;
    while(vptr!=NULL) {
        printf("*V:%p", &(vptr->vt));
        Nodo_A* aptr=vptr->lista_ady;
        while(aptr!=NULL) {
            printf(" -> A:%p", &(aptr->ar));
            aptr=aptr->sig;
        }
        printf("\n");
        vptr=vptr->sig;
    }
    return;
}

static Arista** grafo_d_generar_mat_ady(Grafo_D* grafo);

#endif