/*Libraria de Grafo Dinamico Implementado Mediante Lista de Adyacencia*/
#ifndef _GRAFO_D_H
#define _GRAFO_D_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

/*------------------------Macros y Customizacion de la Libreria------------------------*/
#ifndef Vertice
#define Vertice int
#endif

#ifndef Arista
#define Arista int
#endif

#ifndef peso_t
    #define peso_t double
    #define PESO_NO_ARISTA HUGE_VAL
#else
    #ifndef PESO_NO_ARISTA
    #error "Defina la MACRO PESO_NO_ARISTA para el peso_t definido" 
    #endif
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
    Nodo_V* lista_fin;
    size_t orden;
    size_t tamano;
    bool (*cmp_vt)(Vertice*, Vertice*);
    bool (*cmp_ar)(Arista*, Arista*);
    peso_t (*calc_peso)(Arista*);
} Grafo_D;

typedef struct matriz_ady {
    size_t orden;
    Arista* datos[];
} Matriz_Ady;

typedef struct matriz_peso {
    size_t orden;
    peso_t datos[];
} Matriz_Peso;

/*---------------------------Operaciones basicas del grafo------------------------------*/

/*  Crear un grafo vacio para empezar a realizar operaciones
    El grafo debe contar con algunas funciones para realizar algunas operaciones
    bool (*cmp_vt)(Vertice*, Vertice*) comparar si dos vertices son iguales
    bool (*cmp_ar)(Arista*, Arista*) comparar sis dos aristas son iguales
    peso_t (*calc_peso)(Arista*) regresa el peso de una arista
*/
static Grafo_D* grafo_d_crear(bool (*cmp_vt)(Vertice*, Vertice*),
    bool (*cmp_ar)(Arista*, Arista*), peso_t (*calc_peso)(Arista*)) {
    
    Grafo_D* grafo = (Grafo_D*)malloc(sizeof(Grafo_D));
    if(!grafo) return NULL;
    grafo->lista_ady=NULL;
    grafo->lista_fin=NULL;
    grafo->orden=0;
    grafo->tamano=0;
    grafo->cmp_vt=cmp_vt;
    grafo->cmp_ar=cmp_ar;
    grafo->calc_peso=calc_peso;
    return grafo;
}

/*  Regresa si el grafo se encuentra vacio*/
static inline bool grafo_d_isempty(const Grafo_D* grafo) {
    return grafo->lista_ady==NULL
        && grafo->lista_fin==NULL 
        && grafo->orden==0;
}

/*  Libera la memoria reservada dinamicamente para el grafo,
    el puntero a el grafo pasado a la funcion queda invalidado despues de esta operacion
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
    if(!grafo->cmp_vt) return NULL;
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
    if(!grafo->cmp_vt) return NULL;
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
    if(!grafo->cmp_ar) return NULL;
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
    if(!grafo->cmp_ar) return NULL;
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
    if(!grafo->cmp_ar) return NULL;
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
    if(!grafo->cmp_ar) return NULL;
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
    if(!grafo->calc_peso) return NULL;
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