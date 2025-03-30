/*Libraria de Grafo Dinamico Implementado Mediante Lista de Adyacencia*/
#ifndef _GRAFO_D_H
#define _GRAFO_D_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

/*------------------------Macros y Customizacion de la Libreria------------------------*/
#if defined(STRUCT_VERTICE) && defined(DATO_VERTICE)
#error "Defina exlusivamente STRUCT_VERTICE o DATO_VERTICE no ambos"
#endif

#ifdef STRUCT_VERTICE
#define Vertice STRUCT_VERTICE
#define cmp_vt_default NULL
#endif

#ifdef STRUCT_ARISTA
#define Arista STRUCT_ARISTA
#define cmp_ar_default NULL
#endif

#ifdef DATO_VERTICE
#define Vertice DATO_VERTICE
#endif

#ifdef DATO_ARISTA
#define Arista DATO_ARISTA
#endif

#ifndef Vertice
#define Vertice int
#endif

#ifndef Arista
#define Arista int
#endif

#ifndef peso_t
#define peso_t int
#define PESO_NO_ARISTA INT_MAX
#endif
    
#ifndef PESO_NO_ARISTA
#error "Defina la MACRO PESO_NO_ARISTA para el peso_t definido" 
#endif

#ifndef STRUCT_VERTICE
static bool _grafo_d_cmp_vt_default(Vertice* v1, Vertice* v2) {return *v1==*v2;}
#define cmp_vt_default _grafo_d_cmp_vt_default
#endif

#ifndef STRUCT_ARISTA
static bool _grafo_d_cmp_ar_default(Arista* a1, Arista* a2) {return *a1==*a2;}
#define cmp_ar_default _grafo_d_cmp_ar_default
#endif

static peso_t _grafo_d_calc_peso_default(Arista* arista) {return (peso_t)1;}
#define calc_peso_default _grafo_d_calc_peso_default

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
    Nodo_V* lista_fin;
    size_t orden;
    size_t tamano;
    bool (*cmp_vt)(Vertice*, Vertice*);
    bool (*cmp_ar)(Arista*, Arista*);
    peso_t (*calc_peso)(Arista*);
} Grafo_D;

/*---------------------------Operaciones basicas del grafo------------------------------*/

/*  Crear un grafo vacio para empezar a realizar operaciones, algunas funciones requieren
    que el grafo tenga asociado un comportamineto en particular, mediante las funciones
    grafo_d_set_cmp_vt(), grafo_d_set_cmp_ar() y grafo_d_set_calc_peso() 
*/
static Grafo_D* grafo_d_crear(void) {
    Grafo_D* grafo = (Grafo_D*)malloc(sizeof(Grafo_D));
    if(!grafo) return NULL;
    grafo->lista_ady=NULL;
    grafo->lista_fin=NULL;
    grafo->orden=0;
    grafo->tamano=0;
    grafo->cmp_vt=cmp_vt_default;
    grafo->cmp_ar=cmp_ar_default;
    grafo->calc_peso=calc_peso_default;
    return grafo;
}

/*  Asigna una funcion bool cmp_vt(Vertice*, Vertice*) implementada por el usario de
    la libreria responsable de comparar las vertices del grafo. Esta funcion se debe
    registrarse obligatorimente cuando el usario ha definido los vertices del grafo
    como tipo struct y se va hacer uso de la funcion grafo_d_buscar_vertice(). 
*/
static inline void grafo_d_set_cmp_vt(Grafo_D* grafo,
    bool (*cmp_vt)(Vertice*, Vertice*)) {grafo->cmp_vt=cmp_vt;}

/* Regresa el comportamiento de comparacion de vertices a su estado por defecto*/
static inline void grafo_d_unset_cmp_vt(Grafo_D* grafo) {
    grafo->cmp_vt=cmp_vt_default;}

/*  Asigna una funcion bool cmp_ar(Arista*, Arista*) implementada por el usuario de
    la libreria responsable de comparar las aristas de el grafo. Esta funcion debe
    registrarse obligatoriamente cuando el usuario ha definido las aristas del grafo
    como tipo struct y se va hacer uso de alguna de las funciones grafo_d_buscar_arista(s)_...()
    a excepcion de las funciones grafo_d_buscar_arista(s)_entre_vert()
*/
static inline void grafo_d_set_cmp_ar(Grafo_D* grafo,
    bool (*cmp_ar)(Arista*, Arista*)) {grafo->cmp_ar=cmp_ar;}

/* Regresa el comportamiento de comparacion de aristas a sus estado por defecto*/
static inline void grafo_d_unset_cmp_ar(Grafo_D* grafo) {
    grafo->cmp_ar=cmp_ar_default;}

/*  Asigna una funcion peso_t calc_peso(Arista*) implementada por el usuario de la
    libreria reponsable de asignar un peso en base a la informacion de una arista del grafo
    Esta funcion nunca es obligatoria de registrar, su comportamiento por default es regresar
    (peso_t)1 para toda arista elemento del grafo.
*/
static inline void grafo_d_set_calc_peso(Grafo_D* grafo,
    peso_t (*calc_peso)(Arista*)) {grafo->calc_peso=calc_peso;}

/* Regresa el comportamiento de asignacion de pesos a su estado pro defecto*/
static inline void grafo_d_unset_calc_peso(Grafo_D* grafo) {
    grafo->calc_peso=calc_peso_default;}

/*  Regresa si el grafo se encuentra vacio*/
static inline bool grafo_d_isempty(const Grafo_D* grafo) {
    return grafo->lista_ady==NULL
        && grafo->lista_fin==NULL 
        && grafo->orden==0;
}

/*  Libera la memoria reservada dinamicamente para el grafo, el puntero a el grafo
    pasado a la funcion queda invalidado despues de esta operacion
*/
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

/*  Inserta un vertice con los datos que contenga el agumento vt, devuelve la direccion
    de memoria que identifica al vertice como elemento unico del grafo, las demas operaciones
    que esperan un argumento vertice toman este valor de retorno para identificar el vertice
*/
static Vertice* grafo_d_insertar_vertice(Grafo_D* grafo, Vertice vt) {
    Nodo_V* nuevo = (Nodo_V*)malloc(sizeof(Nodo_V));
    if(!nuevo) return NULL;
    nuevo->lista_ady=NULL;
    nuevo->sig=NULL;
    nuevo->vt=vt;
    nuevo->grado_s=0;
    //Vertices se insetan por el final en tiempo constante
    if(grafo->lista_fin==NULL) {
        grafo->lista_ady=nuevo;
        grafo->lista_fin=nuevo;
    }
    else {
        grafo->lista_fin->sig=nuevo;
        grafo->lista_fin=nuevo;
    }
    ++(grafo->orden);
    return &(nuevo->vt);
}

/*  Regresa la direccion de memoria (identificador) del primer vertice que haga que
    la funcion grafo->cmp_vt() regrese como verdadero
*/
static Vertice* grafo_d_buscar_vertice(const Grafo_D* grafo, Vertice ref) {
    assert(grafo->cmp_vt!=NULL);
    Nodo_V* vptr = grafo->lista_ady;
    while(vptr!=NULL) {
        if(grafo->cmp_vt(&(vptr->vt),&ref))
            return &(vptr->vt);
        vptr=vptr->sig;
    }
    return NULL;
}

/*  Regresa un vector almacenado dinamicamente que contine los vertices que hacen que
    funcion grafo->cmp_vt() regrese verdadero. El vector esta reservado con memoria dinamica
    por lo que necesita ser liberado con una llamada a free()
*/
static const Vect_V* grafo_d_buscar_vertices(const Grafo_D* grafo, Vertice ref) {
    assert(grafo->cmp_vt!=NULL);
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
    if(!vector) {free(vectmp); return NULL;}
    for(int i=0; i<vectmp->tamano; ++i) vector->vertices[i]=vectmp->vertices[i];
    vector->tamano=vectmp->tamano;
    free(vectmp);
    return (const Vect_V*)vector;
}


/*  Elimna un unico vertice cuyo identificador esta dado por el argumento vt. Las aristas
    que concetan con el vertice vt tambien son eliminadas. Esta funcion espera como argumento vt
    un puntero valido a un vertice del grafo, resultado de las funciones crear o buscar vertices.
    De no ser asi el comportamiento de la funcion es indefinido.
*/
static void grafo_d_eliminar_vertice(Grafo_D* grafo, Vertice* vt) {
    Nodo_V* vptr = grafo->lista_ady, *vprev=NULL, *vtmp;
    //Recorremos todos los vertices
    while(vptr!=NULL) {
        Nodo_A* aptr = vptr->lista_ady, *aprev=NULL, *atmp;
        //Si el nodo actual contiene el vertice a eliminar
        if(vt==&(vptr->vt)) {
            //Borramos todas las aristas que salen del vertice
            while(aptr!=NULL) {
                atmp=aptr;
                aptr=aptr->sig;
                free(atmp);
            }
            //Actualizamos el numero de aristas del grafo
            grafo->tamano-=vptr->grado_s;
            //Movemos solamente el puntero de vertice actual
            vtmp=vptr;
            vptr=vptr->sig;
            /*Si el vertice a elminar es el ultimo eliminamos
            actualizamos grafo->lista->fin*/
            if(vtmp==grafo->lista_fin) 
                grafo->lista_fin=vprev;
            //Conectamos la lista saltando el vertice eliminado
            if(vprev!=NULL) vprev->sig=vptr; 
            else grafo->lista_ady=vptr;
            //Eliminamos el vertice y actualizamos orden
            free(vtmp);
            --(grafo->orden);
        }
        //Si es cualquier otro vertice
        else {
            //Recorremos todas sus aristas
            while(aptr!=NULL) {
                //Si la arista termina en el vertice a eliminar
                if(vt==&(aptr->fin->vt)) {
                    //Movemos solamente el puntero de arista actual
                    atmp=aptr;
                    aptr=aptr->sig;
                    //Conectamos la lista saltando el vertice eliminado
                    if(aprev!=NULL) aprev->sig=aptr;
                    else vptr->lista_ady=aptr;
                    //Eliminamos la arista y actualizamos grado, tamano
                    free(atmp);
                    --(vptr->grado_s);
                    --(grafo->tamano);
                }
                else {
                    //Si fue una arista regular avanzamos aptr y aprev
                    aprev=aptr;
                    aptr=aptr->sig;
                }
            }
            //Si fue un vertice regular avanzamos vptr y vprev
            vprev=vptr; 
            vptr=vptr->sig;
        }
    }
    return;
}

/*----------------------------Operaciones sobre las aristas------------------------------*/

/*  Inserta una arista entre el vertice ini y el vertice fin que va de ini a fin. Los datos de la
    arista seran los suministrados mediante el argumento ar. La funcion devuelve la direccion de memoria
    (identificador) de la arista como elemento unico del grafo.
*/
static Arista* grafo_d_insertar_arista(Grafo_D* grafo, Arista ar, const Vertice* ini, const Vertice* fin) {
    Nodo_A* nuevo = (Nodo_A*)malloc(sizeof(Nodo_A));
    if(!nuevo) return NULL;
    nuevo->ar=ar; nuevo->fin=NULL;
    Nodo_V* vptr=grafo->lista_ady, *inicio=NULL;
    //Encontramos los nodos correspondientes a ini y a fin
    while(nuevo->fin==NULL || inicio==NULL) {
        if(ini==&(vptr->vt)) inicio=vptr;
        if(fin==&(vptr->vt)) nuevo->fin=vptr;
        vptr=vptr->sig;
    }
    //La arista se inserta por el principio
    nuevo->sig=inicio->lista_ady;
    inicio->lista_ady=nuevo;
    ++(inicio->grado_s);
    ++(grafo->tamano);
    return &(nuevo->ar);
}

/*  Inseta un par de aristas conjugadas (una arista de ini a fin y otra de fin a ini) entre dos vertices.
    Los datos para ambas aristas estan dados por el paramentro ar. La funcion devuelve un vector alamacenado
    dinamicamente de tamano 2 que contine ambas aristas. Debido a que el vector que regresa la funcion es
    dinamicamente alojado en memoria debe ser liberado con una llamada free()
*/
static Vect_A* grafo_d_insertar_arista_par(Grafo_D* grafo, Arista ar, const Vertice* ini, const Vertice* fin) {
    Nodo_A* ini_fin = (Nodo_A*)malloc(sizeof(Nodo_A));
    if(!ini_fin) return NULL;
    Nodo_A* fin_ini = (Nodo_A*)malloc(sizeof(Nodo_A));
    if(!fin_ini) {free(ini_fin); return NULL;}
    Vect_A* vect = (Vect_A*)malloc(sizeof(Vect_A)+sizeof(Arista*)*2);
    if(!vect) {free(ini_fin); free(fin_ini); return NULL;}
    vect->tamano=2;
    ini_fin->ar=fin_ini->ar=ar;
    ini_fin->fin=fin_ini->fin=NULL;
    Nodo_V* vptr=grafo->lista_ady;
    while(ini_fin->fin==NULL || fin_ini->fin==NULL) {
        if(ini==&(vptr->vt)) {
            fin_ini->fin=vptr;
            ini_fin->sig=vptr->lista_ady;
            vptr->lista_ady=ini_fin;
            ++(vptr->grado_s);
        }
        if(fin==&(vptr->vt)) {
            ini_fin->fin=vptr;
            fin_ini->sig=vptr->lista_ady;
            vptr->lista_ady=fin_ini;
            ++(vptr->grado_s);
        }
        vptr=vptr->sig;
    }

    grafo->tamano+=2;
    vect->aristas[0]=&(ini_fin->ar);
    vect->aristas[1]=&(fin_ini->ar);
    return vect;
}

/*  Inserta un par de vertices conjugadas (una de ini a fin y otra de fin a ini) en el grafo ambos con
    la informacion suministrada al paramentro ar. La funcion regresa si la insersion se realizo o no.
*/
static bool grafo_d_insertar_arpar(Grafo_D* grafo, Arista ar, const Vertice* ini, const Vertice* fin) {
    Nodo_A* ini_fin = (Nodo_A*)malloc(sizeof(Nodo_A));
    if(!ini_fin) return false;
    Nodo_A* fin_ini = (Nodo_A*)malloc(sizeof(Nodo_A));
    if(!fin_ini) {free(ini_fin); return false;}
    ini_fin->ar=fin_ini->ar=ar;
    ini_fin->fin=fin_ini->fin=NULL;
    Nodo_V* vptr=grafo->lista_ady;
    while(ini_fin->fin==NULL || fin_ini->fin==NULL) {
        if(ini==&(vptr->vt)) {
            fin_ini->fin=vptr;
            ini_fin->sig=vptr->lista_ady;
            vptr->lista_ady=ini_fin;
            ++(vptr->grado_s);
        }
        if(fin==&(vptr->vt)) {
            ini_fin->fin=vptr;
            fin_ini->sig=vptr->lista_ady;
            vptr->lista_ady=fin_ini;
            ++(vptr->grado_s);
        }
        vptr=vptr->sig;
    }
    grafo->tamano+=2;
    return true;
}

/*  Regresa la direccion de memoria(identificador) del la primer arista que haga que la
    funcion grafo->cmp_ar() regrese verdadero.
*/
static Arista* grafo_d_buscar_arista(const Grafo_D* grafo, Arista ref) {
    assert(grafo->cmp_ar!=NULL);
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

/*  Regresa un vector almacenado dinamicamente con las direcciones de memoria de todas
    las aristas en el grafo que hagan que la funcion grafo->cmp_ar() regrese verdader.
    El vector debera ser liberado mediante una llamada a la funcion free()
*/
static const Vect_A* grafo_d_buscar_aristas(const Grafo_D* grafo, Arista ref) {
    assert(grafo->cmp_ar!=NULL);
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
    if(!vector) {free(vectmp); return NULL;}
    for(int i=0; i<vectmp->tamano; ++i) vector->aristas[i]=vectmp->aristas[i];
    vector->tamano=vectmp->tamano;
    free(vectmp);
    return (const Vect_A*)vector;
}

/*  Regresa la primera arista que tenga como inicio al vertice ini y de fin al vertice fin*/
static Arista* grafo_d_buscar_arista_entre_vert(const Grafo_D* grafo, const Vertice* ini, const Vertice* fin) {
    Nodo_V* vptr = grafo->lista_ady;
    while(ini!=&(vptr->vt)) vptr=vptr->sig;
    Nodo_A* aptr = vptr->lista_ady;
    while(aptr!=NULL) {
        if(fin==&(aptr->fin->vt))
            return &(aptr->ar);
    }
    return NULL;
}

/*  Regresa un vector alamacenado dinamicamente con todas las aristas del grafo que tengan como
    inicio al vertice ini y de fin al vertice fin. El vector debera ser liberado mediante 
    una llamada a la funcion free()
*/
static const Vect_A* grafo_d_buscar_aristas_entre_vert(const Grafo_D* grafo, const Vertice* ini, const Vertice* fin) {
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
    if(!vector) {free(vectmp); return NULL;}
    for(int i=0; i<vectmp->tamano; ++i) vector->aristas[i]=vectmp->aristas[i];
    vector->tamano=vectmp->tamano;
    free(vectmp);
    return (const Vect_A*)vector;
}

/*  Regresa la primera arista que haga que la funcion grafo->cmp_ar() regrese verdadero asi
    como que tenga como inicio al vertice ini y de fin al vertice fin
*/
static Arista* grafo_d_buscar_arista_estricto(const Grafo_D* grafo, Arista ref, const Vertice* ini, const Vertice* fin) {
    assert(grafo->cmp_ar!=NULL);
    Nodo_V* vptr = grafo->lista_ady;
    while(ini!=&(vptr->vt)) vptr=vptr->sig;
    Nodo_A* aptr = vptr->lista_ady;
    while(aptr!=NULL) {
        if(fin==&(aptr->fin->vt) && grafo->cmp_ar(&(aptr->ar), &ref))
            return &(aptr->ar);
    }
    return NULL;
}

/*  Regresa un vector alamacenado dinamicamente con todas las aristas del grafo que hagan que la
    funcion grafo->cmp_ar() regrese verdadero asi como que tenga como inicio al vertice ini y de fin
    al vertice fin. El vector debera ser liberado mediante una llamada a la funcion free()
*/
static const Vect_A* grafo_d_buscar_aristas_estricto(const Grafo_D* grafo, Arista ref, const Vertice* ini, const Vertice* fin) {
    assert(grafo->cmp_ar!=NULL);
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
    if(!vector) {free(vectmp); return NULL;}
    for(int i=0; i<vectmp->tamano; ++i) vector->aristas[i]=vectmp->aristas[i];
    vector->tamano=vectmp->tamano;
    free(vectmp);
    return (const Vect_A*)vector;
}

/*  Elimina la arista del grafo pasada como agumento ar. El argumento ar debe ser el
    resultado de llamar a alguna funcion de crear o buscar arista. En caso contrario, el
    comportamiento de esta funcio es indefinido
*/
static void grafo_d_elminar_arista(Grafo_D* grafo, Arista* ar) {
    Nodo_V* vptr = grafo->lista_ady;
    while(vptr!=NULL) {
        Nodo_A* aptr = vptr->lista_ady, *aprev;
        if(aptr!=NULL && ar==&(aptr->ar)) {
            vptr->lista_ady=aptr->sig;
            free(aptr);
            --(vptr->grado_s);
            --(grafo->tamano);
            return;
        }
        aprev=aptr; aptr=aptr->sig;        
        while(aptr!=NULL) {
            if(ar==&(aptr->ar)) {
                aprev->sig=aptr->sig;
                free(aptr);
                --(vptr->grado_s);
                --(grafo->tamano);
                return;
            }
            aprev=aptr;
            aptr=aptr->sig;
        }
        vptr=vptr->sig;
    }
    return;
}

/*----------------------------Operaciones de Representacion Matricial------------------------------*/
typedef struct matriz_ady {
    size_t orden;
    Arista* datos[];
} Matriz_Ady;

typedef struct matriz_peso {
    size_t orden;
    peso_t datos[];
} Matriz_Peso;

/*  Devuelve el elemento de una matriz de adyacencia o de pesos que corresponde a la arista del
    grafo que tiene inicio en el vertice agrado en la posicion i y fin el vertice agregado en
    la posicion j.
*/
#define GD_MATRIZ_INDEX(matriz, i, j) (*(matriz->datos+i*matriz->orden+j))

/*  Regresa una matiz de adyacencia de (grafo->orden)x(grafo->orden) almacenada dinamicamente cuyos
    ejes estan ordenados de acuerdo a el orden en el que los vertices fueron insertados en el grafo.
    La matriz contine la direccion de memoria a un dato tipo Arista en cada celda de la matriz.
    Todo acceso a la matriz de hacerse mediante la MACRO GD_MATRIZ_INDEX(matriz, i, j). La memoria
    reservada para la matriz debe de ser liberada mediante una llamada a la funcion free()
*/
static Matriz_Ady* grafo_d_crear_mat_ady(const Grafo_D* grafo) {
    Matriz_Ady* matriz_ady=(Matriz_Ady*)malloc(
        sizeof(Matriz_Ady)+sizeof(Arista*)*grafo->orden*grafo->orden
    );
    if(!matriz_ady) return NULL;
    matriz_ady->orden=grafo->orden;
    Nodo_V* ini=grafo->lista_ady;
    for(int i=0; i<grafo->orden; ++i, ini=ini->sig) {
        Nodo_V* fin=grafo->lista_ady;
        for(int j=0; j<grafo->orden; ++j, fin=fin->sig) {
            Arista** ptr_celda=(matriz_ady->datos)+i*grafo->orden+j;
            Nodo_A* aptr=ini->lista_ady;
            *ptr_celda=NULL;
            while(aptr!=NULL) {
                if(fin==aptr->fin) { 
                    *ptr_celda=&(aptr->ar);
                    break;
                }
                aptr=aptr->sig;
            }
        }
    }
    return matriz_ady;
}

/*  Regresa una matiz de adyacencia de (grafo->orden)x(grafo->orden) almacenada dinamicamente cuyos
    ejes estan ordenados de acuerdo a el orden en el que los vertices fueron insertados en el grafo.
    La matriz contine la direccion de memoria a un dato tipo peso_t en cada celda de la matriz. El
    valor de cada celda esta determinado por el resultado de la funcion grafo->calc_peso() sobre la
    arista correspondiente a cada posicion. Todo acceso a la matriz de hacerse mediante la MACRO
    GD_MATRIZ_INDEX(matriz, i, j). La memoria reservada para la matriz debe de ser liberada mediante
    una llamada a la funcion free()
*/
static Matriz_Peso* grafo_d_crear_mat_peso(const Grafo_D* grafo) {
    Matriz_Peso* matriz_peso=(Matriz_Peso*)malloc(
        sizeof(Matriz_Peso)+sizeof(peso_t)*grafo->orden*grafo->orden
    );
    if(!matriz_peso) return NULL;
    matriz_peso->orden=grafo->orden;
    Nodo_V* ini=grafo->lista_ady;
    for(int i=0; i<grafo->orden; ++i, ini=ini->sig) {
        Nodo_V* fin=grafo->lista_ady;
        for(int j=0; j<grafo->orden; ++j, fin=fin->sig) {
            peso_t* ptr_celda=(matriz_peso->datos)+i*grafo->orden+j;
            Nodo_A* aptr=ini->lista_ady;
            *ptr_celda=PESO_NO_ARISTA;
            while(aptr!=NULL) {
                if(fin==aptr->fin) { 
                    *ptr_celda=grafo->calc_peso(&(aptr->ar));
                    break;
                }
                aptr=aptr->sig;
            }
        }
    }
    return matriz_peso;
}

/*------------------------------Opraciones de busqueda de Caminos--------------------------------*/
//  !!!ESTRUCTURA DE USO INTERNO!!!
typedef struct _nodo_c {
    struct _nodo_c* sig;
    Nodo_V* vt_actual;
    Nodo_A* ar_puente;
    struct _nodo_c* nd_padre;
    peso_t dist_orig;
} _Nodo_C;

/*  !!!FUNCION DE USO INTERNO!!!
    Busca en la lista de Nodos_C el Nodo_C que hace referencia a el Nodo_V
    pasado como segundo argumento.
*/
static _Nodo_C* _get_nodo_c_vertice(_Nodo_C* cptr, Nodo_V* vt) {
    while(cptr!=NULL) {
        if(cptr->vt_actual==vt) return cptr;
        cptr=cptr->sig;
    }
    return NULL;
}

/*  !!!FUNCION DE USO INTERNO!!!
    Busca en la lista de Nodos_C el Nodo_C con el valor de dist_orig minimo 
    (y finito) en toda la lista.
*/
static _Nodo_C* _get_nodo_c_minimo(_Nodo_C* cptr) {
    _Nodo_C* minimo = cptr;
    while(cptr!=NULL) {
        if(cptr->dist_orig < minimo->dist_orig)
            minimo=cptr;
        cptr=cptr->sig;
    }
    if(minimo->dist_orig!=PESO_NO_ARISTA)
        return minimo;
    return NULL;
}

/*  !!!FUNCION DE USO INTERNO!!!
    Libera el espacio de memoria de la lista de Nodos_C apuntada por
    el puntero cptr. cptr debe ser la cabeza de la lista para evitar
    perdida de memoria
*/
static void _liberar_lista_c(_Nodo_C* cptr) {
    _Nodo_C* ctmp;
    while(cptr!=NULL) {
        ctmp=cptr;
        cptr=cptr->sig;
        free(ctmp);
    }
    return;
}

typedef struct camino_d {
    Vertice** vts;
    Arista** ars;
    size_t saltos;
    peso_t longitud;
} Camino_D;

static inline bool es_camino_valido(Camino_D* camino) {
    return camino->vts!=NULL;
}

/*  Libera la memoria reservada para el camino que se pasa como argumento
    de la funcion. El puntero que apuntaba al camino se vuelve invalido
    despues de esta operacion.
*/
static void camino_d_destruir(Camino_D* camino) {
    if(camino->vts) free(camino->vts);
    if(camino->ars) free(camino->ars);
    free(camino);
    return;
}

/*  Encuentra el camino mas corto entre el vertice ini y fin utilizando el algoritmo
    de Dijkstra. El camino se regresa como un struct tipo Camino_D dinamicamente almacenado
    en memoria y por lo tanto debera de liberarse utilizando la funcion camino_d_destruir.
    En caso de error la funcion regresa NULL. En caso de que no exista ningun camino entre
    el los vertice ini y fin, la funcion regresa un camino invalido, no NULL, e igualmente
    debera ser liberado usando camino_d_destruir. 
*/
static Camino_D* grafo_d_dijkstra(const Grafo_D* grafo, const Vertice* ini, const Vertice* fin) {
    if(!grafo->calc_peso) return NULL;

    Nodo_V* vptr; Nodo_A* aptr; _Nodo_C* cptr;
    _Nodo_C* visitados=NULL;
    _Nodo_C* no_visitados=NULL;
    vptr=grafo->lista_ady;
    //Inicializamos listas de visitados y no visitados
    while(vptr!=NULL) {
        cptr=(_Nodo_C*)malloc(sizeof(_Nodo_C));
        if(!cptr) {
            _liberar_lista_c(visitados);
            _liberar_lista_c(no_visitados);
            return NULL;
        }
        if(ini!=&(vptr->vt)) {
            cptr->sig=no_visitados;
            cptr->vt_actual=vptr;
            cptr->ar_puente=NULL;
            cptr->nd_padre=NULL;
            cptr->dist_orig=PESO_NO_ARISTA;
            no_visitados=cptr;
        }
        else {
            cptr->sig=NULL;
            cptr->vt_actual=vptr;
            cptr->ar_puente=NULL;
            cptr->nd_padre=NULL;
            cptr->dist_orig=0;
            visitados=cptr;
        }
        vptr=vptr->sig;
    }

    /*----------------Parte Iterativa----------------
        Ejecutamos Dijkstra hasta encontrar el camino
        mas corto hasta el vertice fin */
    _Nodo_C* nodo_ref = visitados;
    while(fin!=&(nodo_ref->vt_actual->vt)) {

        //Para cada arista de salida del vertice
        aptr = nodo_ref->vt_actual->lista_ady;
        while(aptr!=NULL) {
            _Nodo_C* nodo_fin = _get_nodo_c_vertice(no_visitados, aptr->fin);
            if(nodo_fin!=NULL) {
                peso_t dp=nodo_ref->dist_orig+grafo->calc_peso(&(aptr->ar));
                if(dp < nodo_fin->dist_orig) {
                    nodo_fin->dist_orig = dp;
                    nodo_fin->nd_padre  = nodo_ref;
                    nodo_fin->ar_puente = aptr;
                }
            }
            aptr=aptr->sig;
        }

        //Acualizar nodo_ref
        nodo_ref = _get_nodo_c_minimo(no_visitados);

        //Si no existen vertices alcanzables regresamos un camino invalido
        if(nodo_ref==NULL) {
            _liberar_lista_c(visitados);
            _liberar_lista_c(no_visitados);
            Camino_D* camino=(Camino_D*)malloc(sizeof(Camino_D));
            if(!camino) return NULL;
            camino->vts=NULL; camino->ars=NULL;
            camino->saltos=0; camino->longitud=0;
            return camino;
        };

        //Pasar nuevo nodo_ref de no_visitados a visitados
        if(nodo_ref==no_visitados) {
            no_visitados=no_visitados->sig;
            nodo_ref->sig=visitados;
            visitados=nodo_ref;
        }
        else {
            cptr=no_visitados;
            while(cptr->sig!=nodo_ref)
                cptr=cptr->sig;
            cptr->sig=nodo_ref->sig;
            nodo_ref->sig=visitados;
            visitados=nodo_ref;
        }
    }

    _liberar_lista_c(no_visitados);

    Camino_D* camino = (Camino_D*)malloc(sizeof(Camino_D));
    if(!camino) {_liberar_lista_c(visitados); return NULL;}
    camino->longitud=nodo_ref->dist_orig;
    camino->saltos=0;
    cptr=nodo_ref;
    while(cptr->nd_padre!=NULL) {
        ++(camino->saltos);
        cptr=cptr->nd_padre;
    }

    camino->vts=(Vertice**)malloc(sizeof(Vertice*)*(camino->saltos+1));
    if(!camino->vts) {
        _liberar_lista_c(visitados);
        free(camino);
        return NULL;
    }
    camino->vts[0]=(Vertice*)ini;
    
    //Si el camino no tiene aristas regresamos un camino trivial
    if(camino->saltos==0) return camino;

    camino->ars=(Arista**)malloc(sizeof(Arista*)*camino->saltos);
    if(!camino->ars) {
        _liberar_lista_c(visitados);
        free(camino->vts); free(camino);
        return NULL;
    }
    cptr=nodo_ref;
    for(size_t i=camino->saltos; i>0; --i) {
        camino->vts[i]=&(cptr->vt_actual->vt);
        camino->ars[i-1]=&(cptr->ar_puente->ar);
        cptr=cptr->nd_padre;
    }
    
    _liberar_lista_c(visitados);
    return camino;
}

/*-------------------------------Operaciones Miscelaneas---------------------------------*/

/*  Imprime una representacion cruda del grafo en su forma de lista de adyacencia
    Los vertices y aristas del grafo estan identificados por su direccion de memoria
    el cual es unico para cada vertice/arista.
*/
static void grafo_d_print_debug(const Grafo_D* grafo) {
    printf("G: %p | orden:%lu, tamano:%lu\n",
        grafo, grafo->orden, grafo->tamano);
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

#endif