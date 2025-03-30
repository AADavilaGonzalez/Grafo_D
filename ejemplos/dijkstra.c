#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef struct {
    char nombre[32];
    size_t poblacion;
} Ciudad;

typedef struct {
    char nombre[16];
    double longitud;
} Carretera;

#define STRUCT_VERTICE Ciudad
#define STRUCT_ARISTA Carretera
#define peso_t double
#define PESO_NO_ARISTA HUGE_VAL
#include "grafo_d.h"

double calc_distancia(Carretera* c) {
    return c->longitud;
}

void camino_print(Camino_D* camino) {
    printf("Vts: %p | Ars: %p\n", camino->vts, camino->ars);
    printf("Saltos: %u | Longitud: %.1f\n", camino->saltos, camino->longitud);
    fflush(stdout);
    printf("Camino mas corto entre %s y %s:\n",
        camino->vts[0]->nombre, camino->vts[camino->saltos]->nombre);
    printf("%s\n", camino->vts[0]->nombre);
    for(int i=0; i<camino->saltos; ++i) {
        printf("%s\n", camino->ars[i]->nombre);
        printf("%s\n", camino->vts[i+1]->nombre);
    }
    printf("FIN\n");
    return;
}

int main(void) {
    Grafo_D* grafo = grafo_d_crear();
    grafo_d_set_calc_peso(grafo, calc_distancia);
    
    const Vertice* monterrey = grafo_d_insertar_vertice(grafo, (Ciudad){"Monterrey", 5322177});
    const Vertice* saltillo = grafo_d_insertar_vertice(grafo, (Ciudad){"Salitillo",  879958});
    const Vertice* montemorelos = grafo_d_insertar_vertice(grafo, (Ciudad){"Montemorelos", 67428});
    const Vertice* linares = grafo_d_insertar_vertice(grafo, (Ciudad){"Linares", 84666});
    const Vertice* monclova = grafo_d_insertar_vertice(grafo, (Ciudad){"Monclova", 237951});
    const Vertice* nuevo_laredo = grafo_d_insertar_vertice(grafo, (Ciudad){"Nuevo Laredo", 425058});
    const Vertice* reynosa = grafo_d_insertar_vertice(grafo, (Ciudad){"Reynosa", 967627});
    const Vertice* cd_victoria = grafo_d_insertar_vertice(grafo, (Ciudad){"Ciudad Victoria", 432100});
    const Vertice* matehuala = grafo_d_insertar_vertice(grafo, (Ciudad){"Matehuala", 102199});
    const Vertice* san_luis = grafo_d_insertar_vertice(grafo, (Ciudad){"San Luis Potosi", 911908});

    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-85", 219}, monterrey, nuevo_laredo);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-53", 194}, monterrey, monclova);
    grafo_d_insertar_arpar(grafo, (Carretera){"Fd-40", 85.7}, monterrey, saltillo);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-85", 81.8}, monterrey, montemorelos);
    grafo_d_insertar_arpar(grafo, (Carretera){"Fd-40", 220}, monterrey, reynosa);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-57", 194}, saltillo, monclova);
    grafo_d_insertar_arpar(grafo, (Carretera){"COAH-30", 246}, monclova, nuevo_laredo);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-2", 255}, nuevo_laredo, reynosa);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-97", 348}, reynosa, cd_victoria);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-101", 330}, cd_victoria, san_luis);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-57", 192}, san_luis, matehuala);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-57", 258}, matehuala, saltillo);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-85", 154}, cd_victoria, linares);
    grafo_d_insertar_arpar(grafo, (Carretera){"Mx-85", 51.4}, linares, montemorelos);

    Nodo_V* ini = grafo->lista_ady;
    while(ini!=NULL) {
        Nodo_V* fin = ini->sig;
        while(fin!=NULL) {
            Camino_D* camino = grafo_d_dijkstra(grafo, &(ini->vt), &(fin->vt));
            camino_print(camino);
            printf("\n\n");
            camino_d_destruir(camino);
            fin=fin->sig;
        }
        ini=ini->sig;
    }

    grafo_d_destruir(grafo);
    return 0;
}