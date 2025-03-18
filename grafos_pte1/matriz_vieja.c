#include <stdlib.h>
#include <stdio.h>

#define TAMANO_ARREGLO(arr) (sizeof(arr)/sizeof(arr[0]))

typedef struct {
    char nombre[32];
    size_t poblacion;
} Ciudad;
typedef Ciudad Vertice;

Vertice ciudades[] = {
    {"Monterrey", 5322177},
    {"Salitillo",  879958},
    {"Montemorelos", 67428},
    {"Linares", 84666},
    {"Monclova", 237951},
    {"Nuevo Laredo", 425058},
    {"Reynosa", 967627},
    {"Ciudad Victoria", 432100},
    {"Matehuala", 102199},
    {"San Luis Potosi", 911908}
};

typedef enum {
    MONTERREY,
    SALTILLO,
    MONTEMORELOS,
    LINARES,
    MONCLOVA,
    NUEVO_LAREDO,
    REYNOSA,
    CIUDAD_VICTORIA,
    MATEHUALA,
    SAN_LUIS_POTOSI,
} idVert;

typedef struct {
    char nombre[8];
    double longitud;
} Carretera;

typedef struct {
    idVert origen;
    idVert destino;
    Carretera datos;
} Arista;

Arista carreteras[] = {
    {MONTERREY, NUEVO_LAREDO, {"Mx-85", 219} },
    {MONTERREY, MONCLOVA, {"Mx-53", 194} },
    {MONTERREY, SALTILLO, {"Fd-40", 85.7} },
    {MONTERREY, MONTEMORELOS, {"Mx-85", 81.8} },
    {MONTERREY, REYNOSA, {"Fd-40", 220} },
    {SALTILLO, MONCLOVA, {"Mx-57", 194} },
    {MONCLOVA, NUEVO_LAREDO, {"COAH-30", 246} },
    {NUEVO_LAREDO, REYNOSA, {"Mx-2", 255} },
    {REYNOSA, CIUDAD_VICTORIA, {"Mx-97", 348} },
    {CIUDAD_VICTORIA, SAN_LUIS_POTOSI, {"Mx-101", 330} },
    {SAN_LUIS_POTOSI, MATEHUALA, {"Mx-57", 192} },
    {MATEHUALA, SALTILLO, {"Mx-57", 258} },
    {CIUDAD_VICTORIA, LINARES, {"Mx-85", 154} },
    {LINARES, MONTEMORELOS, {"Mx-85", 51.4} }
};

typedef struct {
    Vertice* vertices;
    size_t num_vt;
    Arista* aristas;
    size_t num_ar;
    Carretera** matriz_ady;
} Grafo_M;

void grafo_m_matriz_set(Grafo_M* grafo, size_t i, size_t j, Carretera* dato) {
    *((grafo->matriz_ady)+(j*grafo->num_vt)+i)=dato;
    return;
}

Carretera* grafo_m_matriz_get(Grafo_M* grafo, size_t i, size_t j) {
    return *((grafo->matriz_ady)+(j*grafo->num_vt)+i);
}

Grafo_M* grafo_m_crear(Vertice* vertices, size_t v, Arista* aristas, size_t a) {
    Grafo_M* grafo = (Grafo_M*)malloc(sizeof(Grafo_M));
    if(!grafo) return NULL;
    grafo->vertices=vertices; grafo->num_vt=v;
    grafo->aristas=aristas; grafo->num_ar=a;
    grafo->matriz_ady=(Carretera**)malloc(sizeof(Carretera*)*v*v);
    if(!grafo->matriz_ady) {free(grafo); return NULL;}
    for(int i=0; i<grafo->num_vt*grafo->num_vt; ++i) {grafo->matriz_ady[i]=NULL;}
    for(Arista* arista = aristas; arista < aristas+a; ++arista) {
        grafo_m_matriz_set(grafo, arista->origen, arista->destino, &(arista->datos));
        grafo_m_matriz_set(grafo, arista->destino, arista->origen, &(arista->datos));
    }
    return grafo;
}

void grafo_m_destruir(Grafo_M* grafo) {
    free(grafo->matriz_ady);
    free(grafo);
    return;
}

void grafo_m_print(Grafo_M* grafo) {
    static const char* nombres[] = {
        "MTY ", "SALT", "MONT", "LNRS", "MONC",
        "NVOL", "REYN", "CDVI", "MTHL", "SLP "
    };
    Carretera** ptr=grafo->matriz_ady;
    printf("     | ");
    for(int i=0; i<grafo->num_vt; ++i)
        printf(" %-5s", nombres[i]);
    printf("|\n");
    for(int i=0; i<grafo->num_vt; ++i) {
        printf("%s | ", nombres[i]);
        for(int j=0; j<grafo->num_vt; ++j) {
            if(*(ptr)!=NULL)
                printf("%5.1f ", (*ptr)->longitud);
            else
                printf(" NaN  ");
            ++ptr;
        }
        printf("|\n");
    }
    return;
}

int main(void) {
    Grafo_M* grafo = grafo_m_crear(ciudades, TAMANO_ARREGLO(ciudades),
        carreteras, TAMANO_ARREGLO(carreteras));
    grafo_m_print(grafo);
    grafo_m_destruir(grafo);
    return 0;
}