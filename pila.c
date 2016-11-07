#include "pila.h"
#include <stdlib.h>
#define TAM_INICIAL 1
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

pila_t* pila_crear(void) {
    pila_t* pila = malloc(sizeof(pila_t));
    if (pila == NULL) {
        return NULL;
    }
    pila->datos = malloc(TAM_INICIAL * sizeof(void*));

    if (pila->datos == NULL) {
        free(pila);
        return NULL;
    }
    pila->capacidad = TAM_INICIAL;
    pila->cantidad = 0;
    return pila;
}

void pila_destruir(pila_t *pila){
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila){
    if (pila->cantidad == 0)
        return true;
    return false;
}

bool pila_redimensionar(pila_t* pila, size_t tam_nuevo) {
    void** datos_nuevo = realloc(pila->datos, tam_nuevo * sizeof(void*));

    if (tam_nuevo > 0 && datos_nuevo == NULL) {
        return false;
    }

    pila->datos = datos_nuevo;
    pila->capacidad = tam_nuevo;
    return true;
}

bool pila_apilar(pila_t *pila, void* valor){
    bool se_puede_apilar = true;
    if (pila->cantidad == pila->capacidad)
        se_puede_apilar = pila_redimensionar(pila, pila->capacidad * 2);
    if (!se_puede_apilar)
        return false;
    pila->datos[pila->cantidad] = valor;
    pila->cantidad++;
    return true;
}

void* pila_ver_tope(const pila_t *pila){
    if (pila->cantidad == 0)
        return NULL;
    return pila->datos[pila->cantidad - 1];
}

void* pila_desapilar(pila_t *pila){
    bool se_puede_desapilar = true;
    if (pila->cantidad > 0 && pila->cantidad <= pila->capacidad/4)
        se_puede_desapilar = pila_redimensionar(pila, pila->capacidad / 2);
    if (!se_puede_desapilar || pila->cantidad == 0)
        return NULL;
    void* valor = pila->datos[pila->cantidad - 1];
    pila->cantidad--;
    return valor;
}




